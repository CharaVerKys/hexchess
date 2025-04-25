#pragma once

#include <qmainwindow.h>
#include "protocol.hpp"
#include "types.hpp"

class ControlWindow;
class BoardSceneWidget;

class ClientController : public QMainWindow{
    Q_OBJECT
public:
    ClientController();

public slots:
    // void receiveMove();
    void receiveListOfAllMatches(lhc::protocol::payload::listOfAllMatches);
    void receiveListOfAllBoardPieces(lhc::protocol::payload::allBoardPieces);
    // void receiveAbort();
    // void receiveWin(figure_side);

    void onCantCreateMatch();

signals:
    void connectToMatch(lhc::unique_id);
    void createMatch(figure_side);
    void requestListOfAllMatches();
    void deleteMatch();
    void abortGame();
    void makeMove(lhc::protocol::payload::piece_move);

private slots:
    void onCreateMatch(figure_side);
    void onDeleteMatch();
    void onConnectToMatch(lhc::unique_id);

private:
    bool waitForAllBoardPieces = false;
    ControlWindow* control;
    BoardSceneWidget* board;
};