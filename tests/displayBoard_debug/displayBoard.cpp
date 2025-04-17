#include "board.hpp"
#include "boardscene.hpp"
#include <QApplication>
#include <iostream>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    BoardSceneWidget w;
    w.setFixedSize(1200,800);
    

    Board board = Board::initBoard(Board::Variant::default_);
    auto prabp = board.getAllPeaces();
    
    QObject::connect(&w,&BoardSceneWidget::clicked,&app,[pw = &w](lhc::position pos, std::optional<figure_type>t, std::optional<figure_side>s){
        std::cout << int(pos.column) << " " << int(pos.row) << std::endl;
        (void)pw; (void)t; (void)s;
        // pw->deletePeace(pos, *t, *s);
        // pw->setPeace(pos, figure_type::king, figure_side::black);
    });


    w.setAllPeaces(prabp.getAllPeaces());

    w.show();

    return app.exec();
}