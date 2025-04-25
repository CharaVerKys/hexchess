#pragma once

#include <qobject.h>
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
    // void onCommitMove();
    // void onAbortGame();

signals:

    // api communication
    void sendAllMatches(lhc::protocol::payload::listOfAllMatches);
    
    // socket
    void sendAllPieces();
    void triggerGameEnd();
    void triggerGameAbort();
    void sendNextMove();
    // void onInvalidMoveAnswer();

    void cantCreateMatch();
    void connectToMatchFail();

public:
    void setAsioPtr(Asio* ptr){asio_ = ptr;}

private:
    Asio* asio_;

};