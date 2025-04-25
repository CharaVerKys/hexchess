#include "interfaceasio.hpp"
#include "asio.hpp"
// #include "asio.tpp"
#include "coroutinesthings.hpp"
#include "threadcheck.hpp"

void InterfaceAsio::initAsioContext(){
    checkThreadIsNOT(&mainThreadID);
    assert(asio_);asio_->runContext();
    checkThread(&asioThreadID);
}

void InterfaceAsio::onRequestAllMatches(){
    [](Asio* p_asio, InterfaceAsio* interfaceAsio)->cvk::coroutine_t{
        co_await p_asio->switchContextToAsio();
        auto res = co_await p_asio->requestAllMatches();
        interfaceAsio->sendAllMatches(res);
    }(asio_,this);
}

void InterfaceAsio::onCreateMatch(figure_side side){
    [](Asio* p_asio, InterfaceAsio* interfaceAsio, figure_side side)->cvk::coroutine_t{
        co_await p_asio->switchContextToAsio();
        bool res = co_await p_asio->createMatch(side);
        if(not res){
            interfaceAsio->cantCreateMatch();
        }
    }(asio_,this,side);
}

void InterfaceAsio::onDeleteMatch(){
    [](Asio* p_asio)->cvk::coroutine_t{
        co_await p_asio->switchContextToAsio();
        co_await p_asio->deleteMatch();
    }(asio_);
}