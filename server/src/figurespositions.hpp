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
    static std::map<position,figure_type> getWhiteStandardPosition(){
        std::map<position,figure_type> figures = 
        {
            {{1,6},figure_type::pawn},
            {{2,6},figure_type::pawn},
            {{3,6},figure_type::pawn},
            {{4,6},figure_type::pawn},
            {{5,6},figure_type::pawn},
            {{6,6},figure_type::pawn},
            {{7,6},figure_type::pawn},
            {{8,6},figure_type::pawn},
            {{9,6},figure_type::pawn},

            {{2,7},figure_type::rook},
            {{3,8},figure_type::knight},
            {{4,9},figure_type::queen},

            {{5,8},figure_type::bishop},
            {{5,9},figure_type::bishop},
            {{5,10},figure_type::bishop},

            {{6,9},figure_type::king},
            {{7,8},figure_type::knight},
            {{8,7},figure_type::rook},
        };

        return figures;
    }
    static std::map<position,figure_type> getBlackStandardPosition(){
        std::map<position,figure_type> figures = 
        {
            {{1,0},figure_type::pawn},
            {{2,1},figure_type::pawn},
            {{3,2},figure_type::pawn},
            {{4,3},figure_type::pawn},
            {{5,4},figure_type::pawn},
            {{6,3},figure_type::pawn},
            {{7,2},figure_type::pawn},
            {{8,1},figure_type::pawn},
            {{9,0},figure_type::pawn},

            {{2,0},figure_type::rook},
            {{3,0},figure_type::knight},
            {{4,0},figure_type::queen},

            {{5,0},figure_type::bishop},
            {{5,1},figure_type::bishop},
            {{5,2},figure_type::bishop},

            {{6,0},figure_type::king},
            {{7,0},figure_type::knight},
            {{8,0},figure_type::rook},
        };

        return figures;
    }
};