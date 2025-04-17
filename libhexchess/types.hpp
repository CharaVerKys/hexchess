#pragma once

#include <cstdint>
#include <cstdlib>

// ? no nms
enum class Color : std::uint8_t{
    invalid,
    first,
    second,
    third,
};

enum class figure_type : std::int8_t{
    invalid = -1,
    pawn    =  0,
    bishop  =  1,
    knight  =  2,
    rook    =  3,
    queen   =  4,
    king    =  5,
};

namespace lhc{
    struct position{
        std::uint8_t column;
        std::uint8_t row;
    };
}

namespace lhc::color{
    inline void next(Color/*mut*/& curColor){
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
    }
}