#pragma once

#include "future.hpp"
#include "protocol.hpp"
#include "unittype.h"
#include <optional>
#include <asio/ip/tcp.hpp>
#include <packetsocketoperations.hpp>

template<class InterfaceAsio>
class Asio{
    InterfaceAsio* interface = nullptr;
    std::optional<asio::ip::tcp::socket> sessionSocket;
    std::optional<lhc::unique_id> clientId;
    asio::io_context context;
    std::string serverDomain;
    const std::uint16_t serverPort = 2025;

public:
    void setServerDomain(std::string const& domain){serverDomain=domain;}
    void setInterfaceAsioPtr(InterfaceAsio* ptr){interface = ptr;}
    void runContext(){context.run();}

    cvk::future<std::optional<cvk::socket::packet_t>> waitPacket(asio::ip::tcp::socket&);

    cvk::future<lhc::protocol::payload::listOfAllMatches> requestAllMatches();
    cvk::future<Unit> createMatch();
    cvk::future<Unit> deleteMatch();
    cvk::future<Unit> connectToMatch();
    
    cvk::future<Unit> commitMove();
    cvk::future<Unit> abortGame();

private:
    cvk::future<std::optional<asio::ip::tcp::socket>> connectToServer();
    cvk::future<lhc::unique_id> getId();
    cvk::future<lhc::unique_id> requestId(asio::ip::tcp::socket&);

};