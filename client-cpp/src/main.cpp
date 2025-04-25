#include "asio.hpp"
#include "clientcontroller.hpp"
#include "interfaceasio.hpp"
#include "threadcheck.hpp"
#include <QApplication>
#include <qthread.h>

#include <qtimer.h>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    checkThread(&mainThreadID);

    Asio asio_;
    InterfaceAsio interfaceAsio;
    // asio_.setInterfaceAsioPtr(&interfaceAsio);
    asio_.setServerDomain("localhost"); //todo config
    interfaceAsio.setAsioPtr(&asio_);
    QThread asioThread;
    interfaceAsio.moveToThread(&asioThread);

    asioThread.start();
    QMetaObject::invokeMethod(&interfaceAsio,"initAsioContext",Qt::QueuedConnection);

    ClientController clientController;

    //from main context to main context and then redirect to asio context
    QObject::connect(&clientController,&ClientController::createMatch,&interfaceAsio,&InterfaceAsio::onCreateMatch, Qt::DirectConnection);
    QObject::connect(&clientController,&ClientController::deleteMatch,&interfaceAsio,&InterfaceAsio::onDeleteMatch, Qt::DirectConnection);
    QObject::connect(&clientController,&ClientController::requestListOfAllMatches,&interfaceAsio,&InterfaceAsio::onRequestAllMatches, Qt::DirectConnection);

    //from asio context to main context
    QObject::connect(&interfaceAsio,&InterfaceAsio::cantCreateMatch,&clientController,&ClientController::onCantCreateMatch, Qt::QueuedConnection);
    QObject::connect(&interfaceAsio,&InterfaceAsio::sendAllMatches,&clientController,&ClientController::receiveListOfAllMatches, Qt::QueuedConnection);


    clientController.show();
    

    interfaceAsio.onRequestAllMatches();


    /*
    

    Board board = Board::initBoard(Board::Variant::default_);
    auto prabp = board.getAllPieces();
    
    QObject::connect(&w,&BoardSceneWidget::clicked,&app,[pb = &board, pw = &w](std::optional<lhc::position> pos, std::optional<figure_type>t, std::optional<figure_side>s){
        static lhc::position posFrom;
        static figure_type tFrom = figure_type::invalid;
        static figure_side sFrom = figure_side::invalid;
        if(pos){
            std::cout << int(pos->column) << " " << int(pos->row) << std::endl;
        }else{
            std::cout <<"no cell" <<std::endl;
        }
    
        if(not pos.has_value()){
            tFrom = figure_type::invalid;
            sFrom = figure_side::invalid;
        }
        else if(tFrom == figure_type::invalid){
            if(t.has_value()){
                tFrom = *t;
                sFrom = *s;
                posFrom = *pos;
            }else{
                tFrom = figure_type::invalid;
                sFrom = figure_side::invalid;
            }
        }else if(s.has_value() and sFrom not_eq figure_side::invalid and s.value() == sFrom){
                tFrom = *t;
                sFrom = *s;
                posFrom = *pos;
        }else{
            moveResult res = movement::entryMove(*pb, lhc::protocol::payload::piece_move{posFrom,tFrom,sFrom,*pos});
            if(res == allowAction){
                if(t.has_value()){
                    pw->deletePiece(*pos, *t, *s);
                }
                pw->deletePiece(posFrom, tFrom, sFrom);
                // promote to queen
                if(tFrom == figure_type::pawn){
                    if(sFrom == figure_side::white){
                        if(pos->row == 0){
                            tFrom = figure_type::queen;
                        }
                    }else{
                        const std::array<std::uint8_t,11> boardEdge = {5,6,7,8,9,10,9,8,7,6,5};
                        if(pos->row == boardEdge.at(pos->column)){
                            tFrom = figure_type::queen;
                        }
                    }
                }
                // promote to queen
                pw->setPiece(*pos, tFrom, sFrom);
            }
            tFrom = figure_type::invalid;
            sFrom = figure_side::invalid;
        }
    });


    w.setAllPieces(prabp.getAllPieces());

    w.show();

*/    return app.exec();
}