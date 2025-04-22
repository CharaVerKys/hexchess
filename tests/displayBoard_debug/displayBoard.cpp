#include "board.hpp"
#include "boardscene.hpp"
#include "movement.hpp"
#include <QApplication>
#include <iostream>


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    BoardSceneWidget w;
    w.setFixedSize(1200,800);
    

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

    return app.exec();
}