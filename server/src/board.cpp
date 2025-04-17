#include "board.hpp"
#include "figurespositions.hpp"
#include "figures.hpp"

Board Board::initBoard(Variant variant){
    assert(variant == Variant::default_);
    Board board;

    board.o_field = std::array<Cell, 91>();
    initColors(board.o_field.value());
    setWhiteFigures(board.o_field.value()); // ? (down)
    setBlackFigures(board.o_field.value()); // ? (down)

    return board;
}

cvk::generator<Board::pair_depth> Board::fieldRanges(){
    std::uint8_t dropped = 0;
    constexpr std::array<std::uint8_t,11> rowSize = {6,7,8,9,10,11,10,9,8,7,6}; 

    for(std::uint8_t const& size : rowSize){
        co_yield {.drop = dropped, .take = size}; 
        dropped += size;
    }
    assert(dropped == 91);
}

std::ranges::take_view<
    std::ranges::drop_view<
        std::ranges::ref_view<
            std::array<Cell,91>
        >
    >
>
Board::getColumn(std::uint8_t column){
    assert(column <12);
    auto range = fieldRanges();
    auto depth = range.begin();
    for(std::uint8_t i{0};i<column;++i){++depth;}
    auto pair = *depth;
    return *o_field | std::views::drop(pair.drop) | std::views::take(pair.take);
}

void Board::initColors(std::array<Cell,91> field){
    auto ranges = fieldRanges();
    std::array<Color,11> firstColor = {
        Color::first,
        Color::second,
        Color::third,
    
        Color::first,
        Color::second,

        Color::third,
    
        Color::second,
        Color::first,
    
        Color::third,
        Color::second,
        Color::first,
    };
    auto curFirstColor = firstColor.begin();

    for (pair_depth range : ranges) {
        Color curColor = *curFirstColor;
        auto view = field | std::views::drop(range.drop) | std::views::take(range.take);
        for(Cell& cell: view){
            cell.color = curColor;
            switch (curColor) {
                case Color::first:{
                    curColor = Color::second;
                }break;
                case Color::second:{
                    curColor = Color::third;
                }break;
                case Color::third:{
                    curColor = Color::first;
                }break;
                /*unreachable*/case Color::invalid: std::abort();
            }
        }// for curent range
        ++curFirstColor;
    }
}

// ? function relate to: setWhiteFigures, setBlackFigures, and maybe other
namespace details{
    void setFiguresSameStyle(std::array<Cell,91>& field,std::map<figures_positions::position, Figure::type> const& figures){
        auto ranges = Board::fieldRanges();
        auto currentRange = ranges.begin();

        for (std::uint8_t column =0; currentRange not_eq ranges.end(); ++column, ++currentRange) {
            for(std::uint8_t row = 0; row < currentRange->take; ++row){
                if(figures.contains({column,row})){
                    Figure*& todoname = field.at(currentRange->drop + row).figure;
                    assert(todoname == nullptr);
                    //todo delete asserts
                    assert(todoname = new figures::Bishop);
                    assert(todoname);
                    assert(field.at(currentRange->drop + row).figure);
                    assert(field.at(currentRange->drop + row).figure == todoname);
                    assert(field.at(currentRange->drop + row).figure->getType() == todoname->getType());
                    delete todoname;
                    todoname = nullptr;
                    //todo ----
                    switch (figures.at({column,row})) {
                        case Figure::type::pawn:{
                                todoname = new figures::Pawn;
                        }break;
                        case Figure::type::bishop:{
                                todoname = new figures::Bishop;
                        }break;
                        case Figure::type::knight:{
                                todoname = new figures::Knight;
                        }break;
                        case Figure::type::rook:{
                                todoname = new figures::Rook;
                        }break;
                        case Figure::type::queen:{
                                todoname = new figures::Queen;
                        }break;
                        case Figure::type::king:{
                                todoname = new figures::King;
                        }break;
                        /*unreachable*/case Figure::type::invalid: std::abort();
                    }//switch
                }//if exist
            }//row
        }//col
    }//function
}

void Board::setWhiteFigures(std::array<Cell,91>& field){
    auto figures = figures_positions::getWhiteStandardPosition();
    details::setFiguresSameStyle(field, figures);
}
void Board::setBlackFigures(std::array<Cell,91>& field){
    auto figures = figures_positions::getBlackStandardPosition();
    details::setFiguresSameStyle(field, figures);
}