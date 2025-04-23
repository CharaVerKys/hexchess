#pragma once

#include <qobject.h>

class InterfaceQt : public QObject{
    Q_OBJECT

public slots:
    // api communication
    void onAllMatchesReceived();
    
    // socket
    void onAllPiecesReceived();
    void onGameEnd();
    void onGameAborted();
    void onMoveMade();
    // void onInvalidMoveAnswer();

signals:

    // api communication
    void requestAllMatches();
    void createMatch();
    void deleteMatch();
    void connectToMatch();
    
    // socket
    void commitMove();
    void abortGame();


};