#pragma once

#include <qobject.h>
#include "coroutinesthings.hpp"
#include "protocol.hpp"
#include "types.hpp"

// template<class InterfaceAsio_t>
class Asio;

class InterfaceAsio : public QObject{
    Q_OBJECT

public slots:
    // initial
    void initAsioContext();

    // api communication
    void onRequestAllMatches();
    void onCreateMatch(figure_side);
    void onDeleteMatch();
    void onConnectToMatch(lhc::unique_id);
    
    // // socket
    void onCommitMove(lhc::protocol::payload::piece_move);
    void onAbortGame();

signals:

    // api communication
    void sendAllMatches(lhc::protocol::payload::listOfAllMatches);
    
    // socket
    void sendAllPieces(lhc::protocol::payload::allBoardPieces);
    void triggerGameEnd(figure_side);
    void triggerGameAbort();
    void sendNextMove(lhc::protocol::payload::piece_move);
    // void onInvalidMoveAnswer();

    void cantCreateMatch();
    void connectToMatchFail();
    void setId(lhc::unique_id);

public:
    void setAsioPtr(Asio* ptr){asio_ = ptr;}
    cvk::coroutine_t sessionWaitLoop();

private:
    Asio* asio_ = nullptr;
};