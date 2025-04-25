#pragma once

#include "coroutinesthings.hpp"
#include "future.hpp"
#include "protocol.hpp"
#include "unittype.h"
#include <optional>
#include <asio/ip/tcp.hpp>
#include <packetsocketoperations.hpp>
#include <fstream>

//todo make another file

template<class InterfaceAsio>
class Asio{
    InterfaceAsio* interface = nullptr;
    std::optional<asio::ip::tcp::socket> sessionSocket;
    std::optional<lhc::unique_id> clientId;
    std::string serverDomain;
    const std::uint16_t serverPort = 2025;
    std::unique_ptr<asio::executor_work_guard<asio::io_context::executor_type>> execGuard;
public:
    ~Asio();
    cvk::details::MoveToAsioThreadAwaiter switchContextToAsio(){return {static_objects::asio_context()};}
    void setServerDomain(std::string const& domain){serverDomain=domain;}
    void setInterfaceAsioPtr(InterfaceAsio* ptr){interface = ptr;}
    void runContext(){
        execGuard = std::make_unique<asio::executor_work_guard<asio::io_context::executor_type>>(asio::make_work_guard(static_objects::asio_context()));
        static_objects::asio_context().run();
    }

    cvk::future<std::optional<cvk::socket::packet_t>> waitPacket(asio::ip::tcp::socket&);

    // [[nodiscard]] cvk::future<lhc::protocol::payload::listOfAllMatches> requestAllMatches();
    cvk::future<Unit> createMatch(figure_side);
    // cvk::future<Unit> deleteMatch();
    // cvk::future<Unit> connectToMatch();
    
    // cvk::future<Unit> commitMove();
    // cvk::future<Unit> abortGame();

private:
    cvk::future<std::optional<asio::ip::tcp::socket>> connectToServer();
    cvk::future<lhc::unique_id> getId();
    cvk::future<lhc::unique_id> requestId(asio::ip::tcp::socket&);

};

template<class InterfaceAsio>
inline Asio<InterfaceAsio>::~Asio(){
    if(clientId){
        std::ofstream stream("playerId");
        if(stream.is_open() and stream.good()){
            stream << *clientId;
        }
    }
}
