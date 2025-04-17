#include "board.hpp"

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