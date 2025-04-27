#include "matchcontrol.hpp"
#include "movement.hpp"

cvk::coroutine_t MatchControl::initDefaultMatch(lhc::player_t white, lhc::player_t black){
    assert(white->socket.has_value() and black->socket.has_value());
    if(not white->socket->is_open()
    or not black->socket->is_open()
    ){
        throw std::logic_error("one of sockets closed");
    }
    assert(white->side == figure_side::white);
    assert(black->side == figure_side::black);
    assert(white->id);
    assert(black->id);
    assert(white->id not_eq black->id);

    players.white = std::move(white);
    players.black = std::move(black);

    board = Board::initBoard(Board::Variant::default_);
    auto allPieces = board->getAllPieces();
    
    lhc::protocol::PacketHeader headerW{
        sizeof(lhc::protocol::PacketHeader) + allPieces.binSize(),
        players.white->id,
        lhc::protocol::action::sendAllBoardPieces
    };
    lhc::protocol::PacketHeader headerB{
        sizeof(lhc::protocol::PacketHeader) + allPieces.binSize(),
        players.black->id,
        lhc::protocol::action::sendAllBoardPieces
    };

    std::error_code error_code = co_await cvk::socket::await::sendPacket(
        players.white->socket.value(), headerW, allPieces.convertToStream());
    if(error_code){
        throw std::runtime_error(std::string("error during sending all board pieces in initDefaultMatch: ")+=error_code.message());
    }
    error_code = co_await cvk::socket::await::sendPacket(
        players.black->socket.value(), headerB, allPieces.convertToStream());
    if(error_code){
        throw std::runtime_error(std::string("error during sending all board pieces in initDefaultMatch: ")+=error_code.message());
    }

    socketReceiveProcessLifetimeHandle.black = receivedFromBlack();
    socketReceiveProcessLifetimeHandle.white = receivedFromWhite();
}

std::error_code MatchControl::reconnectPlayer(lhc::player_t&& disconnected){
    auto sendAllPieces = [](std::reference_wrapper<lhc::player_t> pl,
                    const auto allPieces,[[maybe_unused]] const lhc::unique_id id
                )->cvk::coroutine_t
    {
        assert(pl.get()->id == id);
        lhc::protocol::PacketHeader header{
            sizeof(lhc::protocol::PacketHeader) + allPieces.binSize(),
            pl.get()->id,
            lhc::protocol::action::sendAllBoardPieces
        };

        std::error_code error_code = co_await cvk::socket::await::sendPacket(
            pl.get()->socket.value(), header, allPieces.convertToStream());
        if(error_code){
            //todo log
            // throw std::runtime_error(std::string("error during sending all board pieces in initDefaultMatch: ")+=error_code.message());
        }
    };

    if(aborted){
        return std::make_error_code(std::errc::connection_aborted);
    }else if(disconnected->id == players.black->id){
        assert(disconnected->side == figure_side::invalid);
        disconnected->side = figure_side::black;
        players.black = std::move(disconnected);
        sendAllPieces(std::ref(players.black), board->getAllPieces(),players.black->id);
    }else if(disconnected->id == players.white->id){
        assert(disconnected->side == figure_side::invalid);
        disconnected->side = figure_side::white;
        players.white = std::move(disconnected);
        sendAllPieces(std::ref(players.white), board->getAllPieces(),players.white->id);
    }else{
        return std::make_error_code(std::errc::invalid_argument);
    }
    return std::make_error_code(std::errc());
}

bool MatchControl::isIdForReconnect(lhc::unique_id const& id){
    if(id == players.black->id){
        return true;
    }else if(id == players.white->id){
        return true;
    }
    return false;
}

DefaultCoroutine MatchControl::receivedFromWhite(){
    while(true){
        auto expected = co_await cvk::socket::await::readPacket(players.white->socket.value());
        if(aborted){co_return;}
        
        auto packet = expected.value();
        co_await processPacket(players.black, packet);
    }
}

DefaultCoroutine MatchControl::receivedFromBlack(){
    while(true){
        auto expected = co_await cvk::socket::await::readPacket(players.black->socket.value());
        if(aborted){co_return;}
        
        auto packet = expected.value();
        co_await processPacket(players.black, packet);
    }
}
cvk::future<Unit> MatchControl::abortGame(){
    lhc::protocol::PacketHeader headerW{
        sizeof(lhc::protocol::PacketHeader),
        players.white->id,
        lhc::protocol::action::abortGame
    };
    lhc::protocol::PacketHeader headerB{
        sizeof(lhc::protocol::PacketHeader),
        players.white->id,
        lhc::protocol::action::abortGame
    };

    std::error_code error_code = co_await cvk::socket::await::sendPacket(
        players.white->socket.value(), headerW, {});
    if(error_code){
        throw std::runtime_error(std::string("error during sending abort signal: ")+=error_code.message());
    }
    error_code = co_await cvk::socket::await::sendPacket(
        players.black->socket.value(), headerB, {});
    if(error_code){
        throw std::runtime_error(std::string("error during sending abort signal: ")+=error_code.message());
    }
    aborted = true;
    co_return {};
}
cvk::future<Unit> MatchControl::answerOnlyAction(lhc::player_t& player, lhc::protocol::action const& action_){
    lhc::protocol::PacketHeader header{
        sizeof(lhc::protocol::PacketHeader),
        player->id,
        action_
    };
    std::error_code error_code = co_await cvk::socket::await::sendPacket(
        player->socket.value(), header, {});
    if(error_code){
        throw std::runtime_error(
            std::string("error during sending action {")
            .append(std::to_string(int(action_)))
            .append("}: ")
            .append(error_code.message())
        );
    }
    co_return {};
}
cvk::future<Unit> MatchControl::broadcastPieceMove(lhc::protocol::payload::piece_move const& move){
    lhc::protocol::PacketHeader headerW{
        sizeof(lhc::protocol::PacketHeader)+sizeof(move),
        players.white->id,
        lhc::protocol::action::movePieceBroadcast
    };
    lhc::protocol::PacketHeader headerB{
        sizeof(lhc::protocol::PacketHeader)+sizeof(move),
        players.white->id,
        lhc::protocol::action::movePieceBroadcast
    };

    std::vector<std::byte> move_payload(8);
    assert(move_payload.size() == 8);
    std::memcpy(move_payload.data(),&move,sizeof(move));

    std::error_code error_code = co_await cvk::socket::await::sendPacket(
        players.white->socket.value(), headerW, move_payload);
    if(error_code){
        throw std::runtime_error(std::string("error during sending broadcast: ")+=error_code.message());
    }
    error_code = co_await cvk::socket::await::sendPacket(
        players.black->socket.value(), headerB, move_payload);
    if(error_code){
        throw std::runtime_error(std::string("error during sending broadcast: ")+=error_code.message());
    }
    nextTurn();
    co_return {};
}
cvk::future<Unit> MatchControl::processPacket(lhc::player_t& player, cvk::socket::packet_t const& packet){
    auto header = packet->getHeader();
    if(header.userID not_eq player->id
    or header.action_ == lhc::protocol::action::abortGame){
        //todo log
        co_await abortGame();
        co_return{};
    }
    if(header.action_ == lhc::protocol::action::requestMovePiece){
        if(turn not_eq playerTurn(player)){
            co_await answerOnlyAction(player, lhc::protocol::action::enemyTurn);
            co_return{};
        }
        lhc::protocol::payload::piece_move piece_move;
        if(header.totalSize not_eq sizeof(header) + sizeof(piece_move)){
            co_await abortGame();
            co_return{};
        }
        // std::memcpy(&piece_move,packet->getPayload().data(),sizeof(piece_move));            
        std::ranges::copy(packet->getPayload(),reinterpret_cast<std::byte*>(&piece_move));
        moveResult res = movement::entryMove(*board, piece_move);
        if(res == disallowAction){
            co_await answerOnlyAction(player, lhc::protocol::action::wrongMovePiece);
        }else if(res == allowAction){
            co_await broadcastPieceMove(piece_move);
        }else if(res == allowAction_andVictoryBlack){
            co_await broadcastPieceMove(piece_move);
            co_await broadcastWin(figure_side::black);
        }else if(res == allowAction_andVictoryWhite){
            co_await broadcastPieceMove(piece_move);
            co_await broadcastWin(figure_side::white);
        }
    }
    co_return{};
}
cvk::future<Unit> MatchControl::broadcastWin(figure_side const& win){
    lhc::protocol::action action;
    switch (win) {
        case figure_side::invalid: std::abort();
        case figure_side::black:
            action = lhc::protocol::action::gameEnd_winBlack;
        break;
        case figure_side::white:
            action = lhc::protocol::action::gameEnd_winWhite;
        break;
    }
    co_await answerOnlyAction(players.black, action);
    co_await answerOnlyAction(players.white, action);
    co_return {};
}