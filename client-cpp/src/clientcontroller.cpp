#include "clientcontroller.hpp"
#include "boardscene.hpp"
#include "controlwindow.hpp"

ClientController::ClientController()
    : QMainWindow{nullptr}
    , control(new ControlWindow(this))
    , board(new BoardSceneWidget(this))
{
    board->setVisible(false);
    this->setCentralWidget(control);
    setStyleSheet("QWidget{background-color: #c98c46;}");
    setFixedWidth(control->sizeHint().width());

    connect(control, &ControlWindow::createMatch, this, &ClientController::onCreateMatch);
    connect(control, &ControlWindow::deleteMatch, this, &ClientController::onDeleteMatch);
    connect(control, &ControlWindow::requestListOfAllMatches,this, &ClientController::requestListOfAllMatches);
    connect(control, &ControlWindow::connectToMatch, this, &ClientController::onConnectToMatch);
}

void ClientController::receiveListOfAllMatches(lhc::protocol::payload::listOfAllMatches all){
    control->onListOfAllMatchesReceived(all);
    setFixedWidth(control->size().width());
}

void ClientController::receiveListOfAllBoardPieces(lhc::protocol::payload::allBoardPieces pieces){
    if(not waitForAllBoardPieces){return;} //todo log
    control->setVisible(false);
    this->setCentralWidget(board);
    setFixedSize(700,800);
    board->setAllPieces(pieces.getAllPieces());
    waitForAllBoardPieces = false;
}

void ClientController::onCreateMatch(figure_side side){
    if(waitForAllBoardPieces){return;}
    emit createMatch(side);
    assert(not waitForAllBoardPieces);
    waitForAllBoardPieces = true;
}

void ClientController::onDeleteMatch(){
    if(not waitForAllBoardPieces){return;}
    emit deleteMatch();
    assert(waitForAllBoardPieces);
    waitForAllBoardPieces = false;
}

void ClientController::onConnectToMatch(lhc::unique_id id){
    // ? canceling created match inside control window
    if(waitForAllBoardPieces){return;}
    emit connectToMatch(id);
    assert(not waitForAllBoardPieces);
    waitForAllBoardPieces = true;
}