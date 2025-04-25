#pragma once

#include <qobject.h>
#include "types.hpp"

template<class InterfaceAsio_t>
class Asio;

class InterfaceAsio : public QObject{
    Q_OBJECT

public slots:
    // initial
    void initAsioContext();

    // api communication
    // void onRequestAllMatches();
    void onCreateMatch(figure_side);
    // void onDeleteMatch();
    // void onConnectToMatch();
    
    // // socket
    // void onCommitMove();
    // void onAbortGame();

signals:

    // api communication
    void sendAllMatches();
    
    // socket
    void sendAllPieces();
    void triggerGameEnd();
    void triggerGameAbort();
    void sendNextMove();
    // void onInvalidMoveAnswer();

public:
    void setAsioPtr(Asio<InterfaceAsio>* ptr);

private:
    Asio<InterfaceAsio>* asio_;

};