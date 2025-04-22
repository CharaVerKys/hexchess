#include "matchcontrol.hpp"
#include "coroutinesthings.hpp"
#include "movement.hpp"

cvk::future<game_winner> MatchControl::initDefaultMatch(lhc::player_t&& white, lhc::player_t&& black){
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

    board = Board::initBoard(Board::Variant::default_);
    auto allPeaces = board.getAllPeaces();
    
    lhc::protocol::PacketHeader headerW{
        sizeof(lhc::protocol::PacketHeader) + allPeaces.binSize(),
        players.white->id,
        lhc::protocol::action::sendAllBoardPeaces
    };
    lhc::protocol::PacketHeader headerB{
        sizeof(lhc::protocol::PacketHeader) + allPeaces.binSize(),
        players.white->id,
        lhc::protocol::action::sendAllBoardPeaces
    };

    std::error_code error_code = co_await cvk::socket::await::sendPacket(
        players.white->socket.value(), headerW, allPeaces.convertToStream());
    if(error_code){
        throw std::runtime_error(std::string("error during sending all board peaces in initDefaultMatch: ")+=error_code.message());
    }
    error_code = co_await cvk::socket::await::sendPacket(
        players.black->socket.value(), headerB, allPeaces.convertToStream());
    if(error_code){
        throw std::runtime_error(std::string("error during sending all board peaces in initDefaultMatch: ")+=error_code.message());
    }

    socketReceiveProcessLifetimeHandle.black = receivedFromBlack();
    socketReceiveProcessLifetimeHandle.white = receivedFromWhite();

    finishGame.coroutine = co_await cvk::co_getHandle();
    co_await std::suspend_always();
    if(aborted){assert(finishGame.winner == invalid_game_winner);}
    players.black->socket->close();
    players.white->socket->close();
    co_return std::move(finishGame.winner);
}

std::error_code MatchControl::reconnectPlayer(lhc::player_t&& disconnected){
    if(aborted or finishGame.coroutine.done()){
        return std::make_error_code(std::errc::connection_aborted);
    }else if(disconnected->id == players.black->id){
        players.black = std::move(disconnected);
    }else if(disconnected->id == players.white->id){
        players.white = std::move(disconnected);
    }else{
        return std::make_error_code(std::errc::invalid_argument);
    }
    return std::make_error_code(std::errc());
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
    asio::post(static_objects::asio_context(),[h = (finishGame.coroutine)]{assert(not h.done()); h.resume();});
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
cvk::future<Unit> MatchControl::broadcastPeaceMove(lhc::protocol::payload::peace_move const& move){
    lhc::protocol::PacketHeader headerW{
        sizeof(lhc::protocol::PacketHeader),
        players.white->id,
        lhc::protocol::action::movePeaceBroadcast
    };
    lhc::protocol::PacketHeader headerB{
        sizeof(lhc::protocol::PacketHeader),
        players.white->id,
        lhc::protocol::action::movePeaceBroadcast
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
    if(header.action_ == lhc::protocol::action::requestMovePeace){
        if(turn not_eq playerTurn(player)){
            co_await answerOnlyAction(player, lhc::protocol::action::enemyTurn);
            co_return{};
        }
        lhc::protocol::payload::peace_move peace_move;
        if(header.totalSize not_eq sizeof(header) + sizeof(peace_move)){
            co_await abortGame();
            co_return{};
        }
        // std::memcpy(&peace_move,packet->getPayload().data(),sizeof(peace_move));            
        std::ranges::copy(packet->getPayload(),reinterpret_cast<std::byte*>(&peace_move));
        moveResult res = movement::entryMove(board, peace_move);
        if(res == disallowAction){
            co_await answerOnlyAction(player, lhc::protocol::action::wrongMovePeace);
        }else if(res == allowAction){
            co_await broadcastPeaceMove(peace_move);
        }
    }
    co_return{};
}
cvk::future<Unit> MatchControl::broadcastWin(game_winner const& win){
    lhc::protocol::action action;
    switch (win) {
        case invalid_game_winner: std::abort();
        case black_win_the_game:
            action = lhc::protocol::action::gameEnd_winBlack;
        break;
        case white_win_the_game:
            action = lhc::protocol::action::gameEnd_winWhite;
        break;
    }
    co_await answerOnlyAction(players.black, action);
    co_await answerOnlyAction(players.white, action);
    finishGame.winner = win;
    asio::post(static_objects::asio_context(),[h = (finishGame.coroutine)]{assert(not h.done()); h.resume();});
    co_return {};
}