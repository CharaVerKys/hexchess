#include "board.hpp"
#include "constants.hpp"
#include "figurespositions.hpp"
#include "figures.hpp"
#include <field_ranges.hpp>

Board Board::initBoard(Variant variant){
    assert(variant == Variant::default_);
    Board board;

    board.o_field = std::array<Cell, 91>();
    initColors(board.o_field.value());
    setWhiteFigures(board.o_field.value()); // ? (down)
    setBlackFigures(board.o_field.value()); // ? (down)

    return board;
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
    auto range = lhc::field_ranges();
    auto depth = range.begin();
    for(std::uint8_t i{0};i<column;++i){++depth;}
    auto pair = *depth;
    return *o_field | std::views::drop(pair.drop) | std::views::take(pair.take);
}

lhc::protocol::payload::allBoardPeaces Board::getAllPeaces()
{
    lhc::protocol::payload::allBoardPeaces allBoardPeaces;
    std::vector<lhc::protocol::payload::peace> allPeaces;
    auto ranges = lhc::field_ranges();
    auto currentRange = ranges.begin();
    for (std::uint8_t column =0; currentRange not_eq ranges.end(); ++column, ++currentRange) {
        for(std::uint8_t row = 0; row < currentRange->take; ++row){
            Figure const*const figure = o_field->at(currentRange->drop + row).figure;
            if(figure not_eq nullptr){
                allPeaces.emplace_back(figure->getType(), lhc::position{column,row});
            }//if exist
        }//row
    }//col
    assert(allPeaces.size() == 36);
    allBoardPeaces.setPeaces(std::move(allPeaces));
    return allBoardPeaces;
}

void Board::initColors(std::array<Cell,91>& field){
    auto ranges = lhc::field_ranges();
    auto curFirstColor = lhc::constants::firstColor.begin();

    for (lhc::pair_depth range : ranges) {
        Color curColor = *curFirstColor;
        auto view = field | std::views::drop(range.drop) | std::views::take(range.take);
        for(Cell& cell: view){
            cell.color = curColor;
            lhc::color::next(curColor);
        }// for curent range
        ++curFirstColor;
    }
}

// ? function relate to: setWhiteFigures, setBlackFigures, and maybe other
namespace details{
    void setFiguresSameStyle(std::array<Cell,91>& field,std::map<figures_positions::position, figure_type> const& figures){
        auto ranges = lhc::field_ranges();
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
                        case figure_type::pawn:{
                                todoname = new figures::Pawn;
                        }break;
                        case figure_type::bishop:{
                                todoname = new figures::Bishop;
                        }break;
                        case figure_type::knight:{
                                todoname = new figures::Knight;
                        }break;
                        case figure_type::rook:{
                                todoname = new figures::Rook;
                        }break;
                        case figure_type::queen:{
                                todoname = new figures::Queen;
                        }break;
                        case figure_type::king:{
                                todoname = new figures::King;
                        }break;
                        /*unreachable*/case figure_type::invalid: std::abort();
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