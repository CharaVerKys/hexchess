#pragma once

#include <cstdint>
#include <map>
#include "figure.hpp"

struct figures_positions {
    struct position{
        std::uint8_t column, row;
        std::strong_ordering operator<=>(position const& other) const noexcept{
            if(column <=> other.column not_eq std::strong_ordering::equal){
                return column <=> other.column; 
            }
            // assert(column <=> other.column == std::strong_ordering::equal);
            return row <=> other.row;
        }
    };
    // todo make it consteval
    static std::map<position,Figure::type> getWhiteStandardPosition(){
        std::map<position,Figure::type> figures = 
        {
            {{1,6},Figure::type::pawn},
            {{2,6},Figure::type::pawn},
            {{3,6},Figure::type::pawn},
            {{4,6},Figure::type::pawn},
            {{5,6},Figure::type::pawn},
            {{6,6},Figure::type::pawn},
            {{7,6},Figure::type::pawn},
            {{8,6},Figure::type::pawn},
            {{9,6},Figure::type::pawn},

            {{2,7},Figure::type::rook},
            {{3,8},Figure::type::knight},
            {{4,9},Figure::type::queen},

            {{5,8},Figure::type::bishop},
            {{5,9},Figure::type::bishop},
            {{5,10},Figure::type::bishop},

            {{6,9},Figure::type::king},
            {{7,8},Figure::type::knight},
            {{8,7},Figure::type::rook},
        };

        return figures;
    }
    static std::map<position,Figure::type> getBlackStandardPosition(){
        std::map<position,Figure::type> figures = 
        {
            {{1,0},Figure::type::pawn},
            {{2,1},Figure::type::pawn},
            {{3,2},Figure::type::pawn},
            {{4,3},Figure::type::pawn},
            {{5,4},Figure::type::pawn},
            {{6,3},Figure::type::pawn},
            {{7,2},Figure::type::pawn},
            {{8,1},Figure::type::pawn},
            {{9,0},Figure::type::pawn},

            {{2,0},Figure::type::rook},
            {{3,0},Figure::type::knight},
            {{4,0},Figure::type::queen},

            {{5,0},Figure::type::bishop},
            {{5,1},Figure::type::bishop},
            {{5,2},Figure::type::bishop},

            {{6,0},Figure::type::king},
            {{7,0},Figure::type::knight},
            {{8,0},Figure::type::rook},
        };

        return figures;
    }
};