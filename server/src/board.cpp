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

bool Board::isAnyPieceAt(lhc::position const& pos){
    auto column = getColumn(pos.column);
    assert(column.size() > pos.row);
    return std::ranges::next(column.begin(),pos.row)->figure not_eq nullptr;
}

Color Board::colorOfCell(lhc::position const& pos){
    auto column = getColumn(pos.column);
    assert(column.size() > pos.row);
    return std::ranges::next(column.begin(),pos.row)->color;
}

std::optional<figure_side> Board::checkVictory_and_movePiece(lhc::position const& from, lhc::position const& to){
    auto column = getColumn(from.column);
    assert(column.size() > from.row);
    auto pieceToMove = std::move(std::ranges::next(column.begin(),from.row)->figure);
    assert(std::ranges::next(column.begin(),from.row)->figure == nullptr);

    column = getColumn(to.column);
    assert(column.size() > to.row);
    std::unique_ptr<Figure>& f = std::ranges::next(column.begin(),to.row)->figure;
    if(f->getType() == figure_type::king){
        figure_side side = f->getSide();
        o_field.reset();
        return side;
    }
    f = std::move(pieceToMove);
    return std::nullopt;
}

void Board::promoteToQueen(lhc::position const& where){
    auto column = getColumn(where.column);
    assert(column.size() > where.row);
    assert(std::ranges::next(column.begin(),where.row)->figure->getType() == figure_type::pawn);
    figure_side side = std::ranges::next(column.begin(),where.row)->figure->getSide();
    std::ranges::next(column.begin(),where.row)->figure = std::make_unique<figures::Queen>(side);
    assert(std::ranges::next(column.begin(),where.row)->figure->getType() == figure_type::queen);
}

lhc::protocol::payload::allBoardPieces Board::getAllPieces()
{
    std::vector<lhc::protocol::payload::piece> allPieces;
    auto ranges = lhc::field_ranges();
    auto currentRange = ranges.begin();
    for (std::uint8_t column =0; currentRange not_eq ranges.end(); ++column, ++currentRange) {
        for(std::uint8_t row = 0; row < currentRange->take; ++row){
            auto const& figure = o_field->at(currentRange->drop + row).figure;
            if(figure not_eq nullptr){
                allPieces.emplace_back(figure->getType(), lhc::position{column,row}, figure->getSide());
            }//if exist
        }//row
    }//col
    // assert(allPieces.size() == 36);
    return lhc::protocol::payload::allBoardPieces{std::move(allPieces)};
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
    void setFiguresSameStyle(std::array<Cell,91>& field,std::map<figures_positions::position, figure_type> const& figures, const figure_side side){
        auto ranges = lhc::field_ranges();
        auto currentRange = ranges.begin();

        for (std::uint8_t column =0; currentRange not_eq ranges.end(); ++column, ++currentRange) {
            for(std::uint8_t row = 0; row < currentRange->take; ++row){
                if(figures.contains({column,row})){
                  std::unique_ptr<Figure> &todoname = field.at(currentRange->drop + row).figure;
                  assert(todoname == nullptr);
                  switch (figures.at({column, row})) {
                  case figure_type::pawn: {
                    todoname = std::make_unique<figures::Pawn>(
                        side); // new figures::Pawn(side);
                        }break;
                        case figure_type::bishop:{
                                todoname = std::make_unique<figures::Bishop>(side);//new figures::Bishop(side);
                        }break;
                        case figure_type::knight:{
                                todoname = std::make_unique<figures::Knight>(side);//new figures::Knight(side);
                        }break;
                        case figure_type::rook:{
                                todoname = std::make_unique<figures::Rook>(side);//new figures::Rook(side);
                        }break;
                        case figure_type::queen:{
                                todoname = std::make_unique<figures::Queen>(side);//new figures::Queen(side);
                        }break;
                        case figure_type::king:{
                                todoname = std::make_unique<figures::King>(side);//new figures::King(side);
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
    details::setFiguresSameStyle(field, figures, figure_side::white);
}
void Board::setBlackFigures(std::array<Cell,91>& field){
    auto figures = figures_positions::getBlackStandardPosition();
    details::setFiguresSameStyle(field, figures, figure_side::black);
}