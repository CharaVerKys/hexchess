#pragma once

#include <generator.hpp>
#include <array>
#include <cassert>
#include <cell.hpp>
#include <cstdlib>
#include <optional>
#include <ranges>
#include <somemacroses.hpp>

class Board{
public:
    enum class Variant : std::int8_t{default_};

private:
    std::optional<std::array<Cell,91>> o_field;

public:
    static Board initBoard(Variant variant){
        assert(variant == Variant::default_);
        Board board;
    
        board.o_field = std::array<Cell, 91>();
        std::abort();

        return board;
    }

    PRIVATE_CONTEXT(struct pair_depth{std::uint8_t drop; std::uint8_t take;};)
    //todo probably function should be private
    static cvk::generator<pair_depth> fieldRanges(){
        std::uint8_t dropped = 0;
        constexpr std::array<std::uint8_t,11> rowSize = {6,7,8,9,10,11,10,9,8,7,6}; 

        for(std::uint8_t const& size : rowSize){
            co_yield {.drop = dropped, .take = size}; 
            dropped += size;
        }
        assert(dropped == 91);
    }

    [[nodiscard]]
    std::ranges::take_view<
        std::ranges::drop_view<
            std::ranges::ref_view<
                std::array<Cell,91>
            >
        >
    >
    getColumn(std::uint8_t column){
        assert(column <12);
        auto range = fieldRanges();
        auto depth = range.begin();
        for(std::uint8_t i{0};i<column;++i){++depth;}
        auto pair = *depth;
        return *o_field | std::views::drop(pair.drop) | std::views::take(pair.take);
    }


private:
    Board(){}

    void initColors(){
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
            auto view = *o_field | std::views::drop(range.drop) | std::views::take(range.take);
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
};