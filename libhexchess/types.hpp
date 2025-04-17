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