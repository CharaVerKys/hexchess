#include "interfaceasio.hpp"
#include "asio.hpp"
#include "asio.tpp"
#include "coroutinesthings.hpp"
#include "threadcheck.hpp"

void InterfaceAsio::initAsioContext(){
    checkThreadIsNOT(&mainThreadID);
    assert(asio_);asio_->runContext();
    checkThread(&asioThreadID);
}

void InterfaceAsio::onCreateMatch(figure_side side){
    [](Asio<InterfaceAsio>* p_asio, figure_side side)->cvk::coroutine_t{
        co_await p_asio->switchContextToAsio();
        co_await p_asio->createMatch(side);
        co_return;
    }(asio_,side);
}

void InterfaceAsio::setAsioPtr(Asio<InterfaceAsio>* ptr){asio_ = ptr;}

