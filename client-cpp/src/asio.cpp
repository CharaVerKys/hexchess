#include "asio.hpp"
#include <fstream>
#include "coroutinesthings.hpp"
#include <asio/connect.hpp>
#include <filesystem>

Asio::~Asio(){
    if(clientId){
        std::ofstream stream("playerId");
        if(stream.is_open() and stream.good()){
            stream << *clientId;
        }
    }
}

// template<class InterfaceAsio>
cvk::future<std::optional<cvk::socket::packet_t>> Asio::waitPacket(asio::ip::tcp::socket& socket){
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
        {case action::sendListOfAllMatches:
         case action::createMatch:
            co_return packet.value();
         break;}
        default:
        [[maybe_unused]] int gccFixError;
    }
    co_return std::nullopt;
}
cvk::future<lhc::protocol::payload::listOfAllMatches> Asio::requestAllMatches(){
    auto o_socket = co_await connectToServer();
    if(not o_socket){
        //todo ...
        co_return{};
    }
    asio::ip::tcp::socket socket  = std::move(o_socket.value());

    lhc::protocol::PacketHeader header{
        sizeof(lhc::protocol::PacketHeader),
        co_await getId(),
        lhc::protocol::action::requestListOfAllMatches,
    };
    
    std::error_code ec = co_await cvk::socket::await::sendPacket(socket,header,{});

    if(ec){
        // todo
        co_return{};
    }

    auto packet = co_await waitPacket(socket);
    if(not packet){
        co_return{};
    } 
    lhc::protocol::payload::listOfAllMatches all;
    all.vec.resize(packet->get()->getPayload().size() / sizeof(lhc::protocol::payload::match));
    std::memcpy(all.vec.data(), packet->get()->getPayload().data(),packet->get()->getPayload().size());
    co_return all;
}

// template<class InterfaceAsio>
cvk::future<bool> Asio::createMatch(figure_side side){
    co_await deleteMatch();
    auto o_socket = co_await connectToServer();
    if(not o_socket){
        //todo ...
        co_return false;
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
        co_return false;
    }

    auto res = co_await waitPacket(socket);
    cvk::socket::packet_t echo;
    if(not res){
        res = co_await waitPacket(socket);
        echo = res.value_or(nullptr);
    }
    echo = res.value_or(nullptr);

    if(echo){
        lhc::protocol::payload::createMatch createMatch_echo;
        std::memcpy(reinterpret_cast<char*>(&createMatch_echo),echo->getPayload().data(),sizeof(createMatch_echo));
        assert(createMatch_echo.side == createMatch_.side);
        assert(echo->getHeader() <=> header == std::strong_ordering::equal);
        assert(not sessionSocket);
        sessionSocket = std::move(socket);
        co_return true;
    }
    co_return false;
}

cvk::future<Unit> Asio::deleteMatch(){
    if(not sessionSocket){
        co_return{};
    }
    sessionSocket->close();
    sessionSocket.reset();

    auto o_socket = co_await connectToServer();
    if(not o_socket){
        //todo ...
        co_return{};
    }
    lhc::protocol::PacketHeader header{
        sizeof(lhc::protocol::PacketHeader),
        co_await getId(),
        lhc::protocol::action::deleteMatch,
    };
    
    std::error_code ec = co_await cvk::socket::await::sendPacket(*o_socket,header,{});

    if(ec){
        // todo
    }
    co_return{};
}

cvk::future<bool> Asio::connectToMatch(lhc::unique_id id){
    assert(not sessionSocket);

    auto o_socket = co_await connectToServer();
    if(not o_socket){
        //todo ...
        co_return false;
    }
    std::vector<std::byte>pld_;
    pld_.resize(sizeof(id));
    std::memcpy(pld_.data(),&id,pld_.size());
    lhc::protocol::PacketHeader header{
        sizeof(lhc::protocol::PacketHeader) + pld_.size(),
        co_await getId(),
        lhc::protocol::action::connectToMatch,
    };
    
    std::error_code ec = co_await cvk::socket::await::sendPacket(*o_socket,header,std::move(pld_));
    if(ec){
        // todo
        co_return false;
    }

    sessionSocket = std::move(o_socket.value());
    asio::steady_timer timer(static_objects::asio_context(), std::chrono::seconds(10));
    auto h = co_await  cvk::co_getHandle();
    timer.async_wait([h](auto){h();});
    co_await std::suspend_always{};

    auto expected = cvk::socket::reliable_is_open(sessionSocket.value());
    if(expected){
        co_return std::move(expected.value());
    }
    //todo log error code
    co_return false;
}
cvk::future<Unit> Asio::commitMove(lhc::protocol::payload::piece_move move){
    if(not sessionSocket){
        throw std::runtime_error("noSocket");
    }
    std::vector<std::byte>pld_;
    pld_.resize(sizeof(move));
    std::memcpy(pld_.data(),&move,pld_.size());
    lhc::protocol::PacketHeader header{
        sizeof(lhc::protocol::PacketHeader) + pld_.size(),
        co_await getId(),
        lhc::protocol::action::requestMovePiece,
    };
    
    std::error_code ec = co_await cvk::socket::await::sendPacket(sessionSocket.value(),header,std::move(pld_));
    if(ec){
        // todo handle
    }
    co_return {};
}
cvk::future<Unit> Asio::abortGame(){
    if(not sessionSocket){
        throw std::runtime_error("noSocket");
    }
    lhc::protocol::PacketHeader header{
        sizeof(lhc::protocol::PacketHeader),
        co_await getId(),
        lhc::protocol::action::abortGame,
    };
    
    std::error_code ec = co_await cvk::socket::await::sendPacket(sessionSocket.value(),header,{});
    if(ec){
        // todo handle
    }
    co_return {};
}

cvk::future<std::variant<Asio::abortType,Asio::errorType,Asio::winGame,
    lhc::protocol::payload::piece_move,
    lhc::protocol::payload::allBoardPieces>>
Asio::waitSession(){
    if(not sessionSocket){
        co_return {errorType{}};
    }
    auto packet = co_await cvk::socket::await::readPacket(sessionSocket.value());
    if(not packet){
        //handle
        co_return {errorType{}};
    }

    auto header = packet->get()->getHeader();
    using lhc::protocol::action;
    switch (header.action_) {
        case action::abortGame:{
            sessionSocket.reset();
            co_return {abortType{}};
        }break;
        case action::enemyTurn:{
            // todo
        }break;
        case action::gameEnd_winBlack:{
            sessionSocket.reset();
            co_return {winGame{figure_side::black}};
        }break;
        case action::gameEnd_winWhite:{
            sessionSocket.reset();
            co_return {winGame{figure_side::white}};
        }break;
        case action::movePieceBroadcast:{
            lhc::protocol::payload::piece_move move_;
            auto payload = packet.value()->getPayload();
            assert(payload.size() == sizeof(move_));
            std::memcpy(reinterpret_cast<std::byte*>(&move_),payload.data(),payload.size());
            co_return {move_};
        }break;
        case action::sendAllBoardPieces:{
            lhc::protocol::payload::allBoardPieces allPieces;
            auto f = packet->get()->getPayload();
            allPieces.parseFromStream({f.begin(),f.end()});
            assert(allPieces.getAllPieces().size() == 36);
            co_return {allPieces};
        }break;
        case action::wrongMovePiece:{
            //todo
        }break;
        default:
        [[maybe_unused]] int gccFixError;
    }
    co_return {errorType{}};
}

// template<class InterfaceAsio>
cvk::future<std::optional<asio::ip::tcp::socket>> Asio::connectToServer(){
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

// template<class InterfaceAsio>
cvk::future<lhc::unique_id> Asio::getId(){
    if(clientId){
        uint ret = clientId.value();
        co_return std::move(ret);
    }
    if(std::filesystem::exists("playerId")){
        std::ifstream stream("playerId");
        if(stream.is_open() and stream.good()){
            stream >> *clientId;
            uint ret = clientId.value();
            co_return std::move(ret);
        }
    }
    auto socket = co_await connectToServer();
    if(not socket){
        socket = co_await connectToServer();
    }
    if(not socket){
        throw std::runtime_error("connect to server error");
    }
    uint res = (clientId = co_await requestId(*socket)).value();
    co_return res;
}

// template<class InterfaceAsio>
cvk::future<lhc::unique_id> Asio::requestId(asio::ip::tcp::socket& socket){
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