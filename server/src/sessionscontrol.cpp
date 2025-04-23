#include "sessionscontrol.hpp"
#include <algorithm>
#include <asio/use_awaitable.hpp> 
#include <call_.hpp>

void SessionsControl::init(std::uint16_t serverPort){
    asio::ip::tcp::endpoint endpoint(asio::ip::address_v4::loopback(),serverPort);
    acceptor = asio::ip::tcp::acceptor(static_objects::asio_context());
    acceptor->open(endpoint.protocol());
    acceptor->set_option(asio::socket_base::reuse_address(true));
    acceptor->bind(endpoint);
    acceptor->listen(asio::socket_base::max_listen_connections);
    acceptorCoroutine = acceptorLoop();
    bool res = acceptorCoroutine.resume();
    if(not res){
        throw std::domain_error("cant run coroutine");
    }
}

DefaultCoroutine SessionsControl::acceptorLoop(){
    while(not static_objects::asio_context().stopped()){
        asio::ip::tcp::socket socket(static_objects::asio_context());

        std::coroutine_handle<> this_coro = co_await cvk::co_getHandle();
        std::error_code ec_;
        acceptor->async_accept(socket,[&ec_, cont = this_coro](std::error_code ec){ec_ = ec; cont();});
        co_await std::suspend_always{};
        if(ec_){
            processError(ec_);
            continue;
        }
        // ? coroutine
        acceptorCallback(std::move(socket));
    }//while
    acceptor->cancel();
    acceptor->close();
}
cvk::coroutine_t SessionsControl::acceptorCallback(asio::ip::tcp::socket socket){
    auto packet = co_await cvk::socket::await::readPacket(socket);

    deleteOldMatches();// ?just call it here, no reason to exactly here

    if(not packet.has_value()){
        processError(packet.error());
        socket.close(); // ? if not already
        co_return;
    }
    upgradeToSocket_bool upgrade;
    try{
        upgrade = co_await processSuccess(packet.value(), socket);
    }catch(std::logic_error const&e){
        //log
    }
    if(upgrade){
        upgradeToSocket(packet.value(), std::move(socket));
    }
    co_return;
}
cvk::coroutine_t SessionsControl::upgradeToSocket(cvk::socket::packet_t packet, asio::ip::tcp::socket socket){
    lhc::unique_id userID = packet->getHeader().userID;
    assert(userID);
    if(packet->getHeader().action_ == lhc::protocol::action::createMatch){
        lhc::protocol::payload::createMatch payload;
        std::memcpy(&payload,packet->getPayload().data(),sizeof(payload));
        lhc::player_t player = std::make_unique<lhc::z_detail_player_type>(userID, payload.side);
        player->socket = std::move(socket);
        allOpenSessions.insert(userID, std::move(player));
    }
    else if(packet->getHeader().action_ == lhc::protocol::action::connectToMatch){
        auto pred = [&userID](MatchControl & val){
                return val.isIdForReconnect(userID);
            };
        if(allOpenSessions.contains(userID)){
            auto ptr = std::move(allOpenSessions.at(userID));
            allOpenSessions.remove(userID);
            matches.emplace_back();
            assert(not matches.back().canBeDestroyed());
            lhc::player_t player = std::make_unique<lhc::z_detail_player_type>(userID);
            player->socket = std::move(socket);
            if(ptr->side == figure_side::white){
                player->side = figure_side::black;
                matches.back().initDefaultMatch(std::move(ptr), std::move(player));
            }
            else if(ptr->side == figure_side::black){
                player->side = figure_side::white;
                matches.back().initDefaultMatch(std::move(player), std::move(ptr));
            }else{
                std::abort(); //todo log
            }
        }
        else if(auto it = std::ranges::find_if(matches,pred); it not_eq matches.end()){
            lhc::player_t player = std::make_unique<lhc::z_detail_player_type>(userID);
            player->socket = std::move(socket);
            std::error_code ec = it->reconnectPlayer(std::move(player));
            assert(not ec);
        }else{
            //todo log, echo "no such game"
            socket.close();
        }
    }else{
        std::abort();
    }
    co_return;
}

cvk::future<SessionsControl::upgradeToSocket_bool> SessionsControl::processSuccess(cvk::socket::packet_t packet, asio::ip::tcp::socket& socket){
    bool completed = co_await checkGenId(packet, socket);
    if(completed){co_return false;}
    auto header = packet->getHeader();
    switch(header.action_){
        {case lhc::protocol::action::connectToMatch:
        case lhc::protocol::action::createMatch:
        co_return true;}

        case lhc::protocol::action::requestListOfAllMatches:{
            event_requestListOfAllSessions(packet->getHeader(), std::move(socket));
        }break;
        case lhc::protocol::action::deleteMatch:{
            event_deleteSession(packet->getHeader(), std::move(socket));
        }break;
        default:
        throw std::logic_error("not supported action");
    }
    co_return false;
}

//? return 'completed'
cvk::future<bool> SessionsControl::checkGenId(cvk::socket::packet_t packet, asio::ip::tcp::socket& socket){
    if(packet->getHeader().userID == 0 and packet->getHeader().action_ == lhc::protocol::action::requestID){
        lhc::unique_id id = pool->getFreeId();
        lhc::protocol::PacketHeader header{
            sizeof(lhc::protocol::PacketHeader),
            id,
            lhc::protocol::action::answerID
        };
        auto ec = co_await cvk::socket::await::sendPacket(socket,header,{});
        if(ec){
            //todo log
        }
        socket.close();
        co_return true;
    }else if(packet->getHeader().userID == 0){
        //todo log
    }else if(pool->isIdValid(packet->getHeader().userID)){
        co_return false;
    }
    lhc::protocol::PacketHeader header{
        sizeof(lhc::protocol::PacketHeader),
        0,
        lhc::protocol::action::error_noID
    };
    auto ec = co_await cvk::socket::await::sendPacket(socket,header,{});
    if(ec){
        //todo log
    }
    socket.close();
    co_return true;
}

void SessionsControl::processError(std::error_code const& ec){
    (void)ec;
}
void SessionsControl::event_deleteSession(lhc::protocol::PacketHeader header, asio::ip::tcp::socket socket){
    try{
        allOpenSessions.remove(header.userID);
    }catch(std::out_of_range const& e){
        //todo log
        socket.close();
    }
}
void SessionsControl::event_requestListOfAllSessions(lhc::protocol::PacketHeader header, asio::ip::tcp::socket socket){
    sendListOfAllSessions(header, std::move(socket));
}
cvk::coroutine_t SessionsControl::sendListOfAllSessions(lhc::protocol::PacketHeader header, asio::ip::tcp::socket socket){

    lhc::protocol::payload::listOfAllMatches matches;
    for(auto const& each : allOpenSessions.getUnderlineValueVector()){
        matches.vec.emplace_back(each->side, each->id);
    }
    header.action_ = lhc::protocol::action::sendListOfAllMatches;
    std::vector<std::byte> payload;
    payload.resize(matches.vec.size() * sizeof(lhc::protocol::payload::match));
    std::memcpy(payload.data(),matches.vec.data(),payload.size());
    header.totalSize = sizeof(header) + payload.size();
    std::error_code ec = co_await cvk::socket::await::sendPacket(socket,header,payload);
    if(ec){
        // todo log
    }
    socket.close();
}
void SessionsControl::deleteOldMatches(){
    std::erase_if(matches, [](MatchControl const& match) {
        return match.canBeDestroyed();
    });
}