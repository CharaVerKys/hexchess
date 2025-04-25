#include "asio.hpp"
#include "coroutinesthings.hpp"
#include <asio/connect.hpp>
#include <filesystem>

template<class InterfaceAsio>
cvk::future<std::optional<cvk::socket::packet_t>> Asio<InterfaceAsio>::waitPacket(asio::ip::tcp::socket& socket){
    if(not socket.is_open()){
        co_return std::nullopt;
    }
    cvk::socket::await::readPacket::expected_t packet = co_await cvk::socket::await::readPacket(socket);
    // auto packet = co_await cvk::socket::await::readPacket(socket);
    if(not packet){
        co_return std::nullopt;
    }
    auto header = packet->get()->getHeader();
    if(header.userID == 0){
        assert(header.action_ == lhc::protocol::action::error_noID);
        clientId.reset(); // player id re-get in next request
        std::filesystem::remove("playerId");
        co_return std::nullopt;
    }
    using lhc::protocol::action;
    switch (header.action_) {
        case action::abortGame:{
            //!signal
            sessionSocket.reset();
        }break;
        case action::enemyTurn:{
            // todo
        }break;
        case action::gameEnd_winBlack:{
            //!signal
            sessionSocket.reset();
        }break;
        case action::gameEnd_winWhite:{
            //!signal
            sessionSocket.reset();
        }break;
        case action::movePieceBroadcast:{
            if(sessionSocket and sessionSocket->native_handle() == socket.native_handle()){
                //!signal
                co_await waitPacket(socket);
            }
        }break;
        case action::sendAllBoardPieces:{
            if(sessionSocket and sessionSocket->native_handle() == socket.native_handle()){
                //!signal
                co_await waitPacket(socket);
            }
        }break;
        case action::sendListOfAllMatches:{
            //!signal
            co_return packet.value();
        }break;
        case action::wrongMovePiece:{
            //todo
        }break;
        case action::createMatch:{
            co_return packet.value();
        }break;
        default:
    }
    co_return std::nullopt;
}

template<class InterfaceAsio>
cvk::future<Unit> Asio<InterfaceAsio>::createMatch(figure_side side){
    auto o_socket = co_await connectToServer();
    if(not o_socket){
        //todo ...
        co_return{};
    }
    asio::ip::tcp::socket socket  = std::move(o_socket.value());

    lhc::protocol::payload::createMatch createMatch_;
    createMatch_.side = side;
    std::vector<std::byte> payload;
    payload.resize(sizeof(createMatch_));
    std::memcpy(payload.data(), &createMatch_, payload.size());

    lhc::protocol::PacketHeader header{
        sizeof(lhc::protocol::PacketHeader) + payload.size(),
        co_await getId(),
        lhc::protocol::action::createMatch,
    };
    
    std::error_code ec = co_await cvk::socket::await::sendPacket(socket,header,payload);

    if(ec){
        // todo
        co_return{};
    }

    auto res = co_await waitPacket(socket);
    auto echo = res.value_or(
        (co_await waitPacket(socket)).value_or(nullptr)
    );
    if(echo){
        lhc::protocol::payload::createMatch createMatch_echo;
        std::memcpy(&payload,echo->getPayload().data(),sizeof(payload));
        assert(createMatch_echo.side == createMatch_.side);
        assert(echo->getHeader() <=> header == std::strong_ordering::equal);
        assert(not sessionSocket);
        sessionSocket = std::move(socket);
    }
    co_return{};
}

template<class InterfaceAsio>
cvk::future<std::optional<asio::ip::tcp::socket>> Asio<InterfaceAsio>::connectToServer(){
    asio::ip::tcp::resolver resolver(static_objects::asio_context());
    std::optional<asio::ip::tcp::resolver::results_type> servers;
    std::error_code ec_;

    std::coroutine_handle<> this_coro = co_await cvk::co_getHandle();
    resolver.async_resolve(serverDomain,std::to_string(serverPort),[&servers, &ec_, this_coro](const std::error_code& ec, asio::ip::tcp::resolver::results_type results){
        ec_ = ec;
        if(ec){
            this_coro.resume();
            return;
        }
        servers = results;
        this_coro.resume();
    });
        co_await std::suspend_always{};
    if(not servers){
        // ? prevent block of resolver
        asio::post(static_objects::asio_context(),[this_coro](){this_coro.resume();});
        co_await std::suspend_always{};
        co_return std::nullopt;
    }
    asio::ip::tcp::socket socket(static_objects::asio_context());
    asio::async_connect(socket,*servers,[&ec_, this_coro](const std::error_code& ec, const asio::ip::tcp::endpoint&){
        ec_ = ec;
        this_coro.resume();
    });
    co_await std::suspend_always{};

    // ? paranoia
    asio::post(static_objects::asio_context(),[this_coro](){this_coro.resume();});
    co_await std::suspend_always{};
    if(ec_){
        co_return std::nullopt;
    }else{
        // co_return std::move(socket);
        co_return socket; // wtf??? copy???
    }
}

template<class InterfaceAsio>
cvk::future<lhc::unique_id> Asio<InterfaceAsio>::getId(){
    if(clientId){
        co_return std::move(clientId.value());
    }
    if(std::filesystem::exists("playerId")){
        std::ifstream stream("playerId");
        if(stream.is_open() and stream.good()){
            stream >> *clientId;
            co_return std::move(clientId.value());
        }
    }
    auto socket = co_await connectToServer();
    if(not socket){
        socket = co_await connectToServer();
    }
    if(not socket){
        throw std::runtime_error("connect to server error");
    }
    co_return co_await requestId(*socket);
}

template<class InterfaceAsio>
cvk::future<lhc::unique_id> Asio<InterfaceAsio>::requestId(asio::ip::tcp::socket& socket){
    lhc::protocol::PacketHeader header{
        sizeof(lhc::protocol::PacketHeader),
        0,
        lhc::protocol::action::requestID
    };
    auto ec = co_await cvk::socket::await::sendPacket(socket,header,{});
    if(ec){
        ec = co_await cvk::socket::await::sendPacket(socket,header,{});
    }
    if(ec){
        throw std::runtime_error("connect to server error");
    }
    auto answer = co_await cvk::socket::await::readPacket(socket);
    if(not answer){
        answer = co_await cvk::socket::await::readPacket(socket);
    }
    if(not answer){
        throw std::runtime_error("connect to server error");
    }
    co_return answer.value()->getHeader().userID;
}