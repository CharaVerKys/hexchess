#pragma once

#include "flat_map.hpp"
#include "idpool.hpp"
#include "matchcontrol.hpp"

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
    using upgradeToSocket = bool;
    tl::expected<upgradeToSocket,std::error_code> processSuccess(cvk::socket::packet_t);
    tl::expected<upgradeToSocket,std::error_code> processError(std::error_code const&); // same for acceptor and reading, at least for now
    

    void event_deleteSession();
    void event_openSession();
    void event_connectToSession();
    void event_requestListOfAllSessions();
    void event_reconnectToMatch();
};