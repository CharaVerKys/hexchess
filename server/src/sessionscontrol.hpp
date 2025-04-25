#pragma once

#include "flat_map.hpp"
#include "idpool.hpp"
#include "matchcontrol.hpp"
#include <coroutinesthings.hpp>

class SessionsControl{
    std::vector<MatchControl> matches;
    cvk::flat_map<lhc::unique_id,lhc::player_t> allOpenSessions;
    std::optional<IdPool> pool;

    std::optional<asio::ip::tcp::acceptor> acceptor;
    DefaultCoroutine acceptorCoroutine;

public:
    void init(std::uint16_t serverPort);

private:
    DefaultCoroutine acceptorLoop();
    cvk::coroutine_t acceptorCallback(asio::ip::tcp::socket);
    cvk::coroutine_t upgradeToSocket(cvk::socket::packet_t, asio::ip::tcp::socket);
    using upgradeToSocket_bool = bool;
    cvk::future<upgradeToSocket_bool> processSuccess(cvk::socket::packet_t, asio::ip::tcp::socket&/*consume socket if return false*/);
    cvk::future<bool> checkGenId(cvk::socket::packet_t, asio::ip::tcp::socket&);//?return 'completed'
    void processError(std::error_code const&, const int descriptor); // same for acceptor and reading, at least for now
    
    void event_deleteSession(lhc::protocol::PacketHeader header, asio::ip::tcp::socket);
    void event_requestListOfAllSessions(lhc::protocol::PacketHeader header, asio::ip::tcp::socket);
    cvk::coroutine_t sendListOfAllSessions(lhc::protocol::PacketHeader header, asio::ip::tcp::socket);
    void deleteOldMatches();
};