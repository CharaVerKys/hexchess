#include "interfaceasio.hpp"
#include "asio.hpp"
// #include "asio.tpp"
#include "coroutinesthings.hpp"
#include "overload.hpp"
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
        interfaceAsio->setId(p_asio->getUsedID());
    }(asio_,this);
}

void InterfaceAsio::onCreateMatch(figure_side side){
    [](Asio* p_asio, InterfaceAsio* interfaceAsio, figure_side side)->cvk::coroutine_t{
        co_await p_asio->switchContextToAsio();
        bool res = co_await p_asio->createMatch(side);
        if(not res){
            interfaceAsio->cantCreateMatch();
        }else{
            interfaceAsio->sessionWaitLoop();
        }
        interfaceAsio->setId(p_asio->getUsedID());
    }(asio_,this,side);
}

void InterfaceAsio::onDeleteMatch(){
    [](Asio* p_asio)->cvk::coroutine_t{
        co_await p_asio->switchContextToAsio();
        co_await p_asio->deleteMatch();
    }(asio_);
}
void InterfaceAsio::onConnectToMatch(lhc::unique_id id){
    [](Asio* p_asio, InterfaceAsio* interfaceAsio, uint id)->cvk::coroutine_t{
        co_await p_asio->switchContextToAsio();
        auto res = co_await p_asio->connectToMatch(id);
        if(not res){
            interfaceAsio->connectToMatchFail();
        }else{
            interfaceAsio->sessionWaitLoop();
        }
        interfaceAsio->setId(p_asio->getUsedID());
    }(asio_,this, id);
}

void InterfaceAsio::onCommitMove(lhc::protocol::payload::piece_move move_){
    [](Asio* p_asio, lhc::protocol::payload::piece_move move)->cvk::coroutine_t{
        co_await p_asio->switchContextToAsio();
        co_await p_asio->commitMove(move);
    }(asio_,move_);
}
void InterfaceAsio::onAbortGame(){
    [](Asio* p_asio)->cvk::coroutine_t{
        co_await p_asio->switchContextToAsio();
        co_await p_asio->abortGame();
    }(asio_);
}

cvk::coroutine_t InterfaceAsio::sessionWaitLoop(){
    std::variant<Asio::abortType, Asio::errorType, Asio::winGame,
               lhc::protocol::payload::piece_move,
               lhc::protocol::payload::allBoardPieces>
    res = co_await asio_->waitSession();
    auto overload = cvk::overload{
        [this](Asio::abortType){
            emit triggerGameAbort();
        },
        [](Asio::errorType){
            //todo handle
        },
        [this](Asio::winGame win){
            emit triggerGameEnd(win.s);
        },
        [this](lhc::protocol::payload::piece_move move_){
            emit sendNextMove(move_);
            sessionWaitLoop();
        },
        [this](lhc::protocol::payload::allBoardPieces all){
            emit sendAllPieces(all);
            sessionWaitLoop();
        },
    };
    std::visit(overload,res);
}