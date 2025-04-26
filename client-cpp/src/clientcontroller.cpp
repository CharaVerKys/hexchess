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
    connect(this, &ClientController::setId,control,&ControlWindow::setId);
}

void ClientController::receiveMove(lhc::protocol::payload::piece_move move){
    board->deletePiece(move.to,figure_type::invalid,
                        move.ver_side == figure_side::black ? figure_side::white : figure_side::black
                    );
    board->deletePiece(move.from,move.ver_type,move.ver_side);
    if(move.ver_type == figure_type::pawn){
        if(move.ver_side == figure_side::white){
            if(move.to.row == 0){
                move.ver_type = figure_type::queen;
            }
        }else{
            const std::array<std::uint8_t,11> boardEdge = {5,6,7,8,9,10,9,8,7,6,5};
            if(move.to.row == boardEdge.at(move.to.column)){
                move.ver_type = figure_type::queen;
            }
        }
    }
    board->setPiece(move.to,move.ver_type,move.ver_side);
}
void ClientController::receiveListOfAllMatches(lhc::protocol::payload::listOfAllMatches all){
    control->onListOfAllMatchesReceived(all);
    setFixedWidth(control->size().width());
}

void ClientController::receiveListOfAllBoardPieces(lhc::protocol::payload::allBoardPieces pieces){
    if(not waitForAllBoardPieces){return;} //todo log
    control->setVisible(false);
    board->setVisible(true);
    this->setCentralWidget(board);
    setFixedSize(700,800);
    board->setAllPieces(pieces.getAllPieces());
    waitForAllBoardPieces = false;
}
void ClientController::receiveAbort(){
    board->setVisible(false);
    control->setVisible(true);
    this->setCentralWidget(control);
    setFixedWidth(control->sizeHint().width());
    control->setText("last match was aborted");
}
void ClientController::receiveWin(figure_side side){
    board->setVisible(false);
    control->setVisible(true);
    this->setCentralWidget(control);
    setFixedWidth(control->sizeHint().width());
    QString winner = side == figure_side::black ? "black" : "white";
    control->setText(winner.append(" won the game"));
}

void ClientController::onCantCreateMatch(){
    control->cantCreateMatch();
}
void ClientController::onConnectToMatchFail(){
    control->setText("connect to match\nFAILED");
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