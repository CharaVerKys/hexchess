#pragma once

#include <cstdint>

class Figure{
public:
    enum class type : std::int8_t{
        invalid = -1,
        pawn    =  0,
        bishop  =  1,
        knight  =  2,
        rook    =  3,
        queen   =  4,
        king    =  5,
    };
    virtual type getType() = 0;
    virtual ~Figure(){}
};