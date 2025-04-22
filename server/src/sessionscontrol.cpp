#include "sessionscontrol.hpp"
#include <asio/use_awaitable.hpp> 
#include <coroutinesthings.hpp>
#include <call_.hpp>

void SessionsControl::init(std::uint16_t serverPort){
    asio::ip::tcp::endpoint endpoint(asio::ip::address_v4::loopback(),serverPort);
    acceptor->open(endpoint.protocol());
    acceptor->set_option(asio::socket_base::reuse_address(true));
    acceptor->bind(endpoint);
    acceptor->listen(asio::socket_base::max_listen_connections);
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
        auto packet = co_await cvk::socket::await::readPacket(socket);
        // ! todo make process success and process error, if upgrade to socket then packet also should have userID
        auto upgrade = packet.and_then(call_this(processSuccess))
            .or_else(call_this(processError));
        if(upgrade.value_or(false)){
            lhc::unique_id userID = packet.value()->getHeader().userID;
            assert(userID);
            if(packet.value()->getHeader().action_ == lhc::protocol::action::createMatch){
                lhc::protocol::payload::createMatch payload;
                std::memcpy(&payload,packet.value()->getPayload().data(),sizeof(payload));
                lhc::player_t player = std::make_unique<lhc::z_detail_player_type>(userID, payload.side);
                player->socket = std::move(socket);
                allOpenSessions.insert(userID, player);
            }
            else if(packet.value()->getHeader().action_ == lhc::protocol::action::connectToMatch){
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
        }// if upgrade to socket session
    }//while
    acceptor->cancel();
    acceptor->close();
}