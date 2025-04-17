#include "board.hpp"
#include "boardscene.hpp"
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    BoardSceneWidget w;
    w.setFixedSize(1200,800);
    

    Board board = Board::initBoard(Board::Variant::default_);
    auto prabp = board.getAllPeaces();
    


    w.setAllPeaces(prabp.getAllPeaces());

    w.show();

    return app.exec();
}