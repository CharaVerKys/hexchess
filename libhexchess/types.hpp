#pragma once

#include <compare>
#include <cstdint>
#include <cstdlib>
#include <optional>
#include <ostream>
#include <asio/ip/tcp.hpp>

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

enum class figure_side : std::int8_t{
    invalid = -1,
    white   =  0,
    black   =  1,
};

enum game_winner : std::int8_t{
    invalid_game_winner = -1,
    white_win_the_game =  0,
    black_win_the_game =  1,
};

namespace lhc{
    struct position{
        std::uint8_t column;
        std::uint8_t row;
        std::strong_ordering operator<=>(position const& other) const noexcept{
            if(column <=> other.column not_eq std::strong_ordering::equal){return column <=> other.column;}
            return row <=> other.row;
        }
        friend std::ostream& operator<<(std::ostream& stream, lhc::position const& pos) noexcept{
            stream << "{"<< int(pos.column) << ", " << int(pos.row) <<"}";
            return stream;
        }
        bool operator==(position const& other) const noexcept{
            return other <=> *this == std::strong_ordering::equal;
        }
    };
}

//todo move from this file to separated
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

namespace lhc{
    using unique_id = std::uint32_t; 
    struct z_detail_player_type{
        figure_side side;
        std::optional<asio::ip::tcp::socket> socket;
        const unique_id id;
        z_detail_player_type(unique_id id, figure_side side = figure_side::invalid):side(side),id(id){}
    };
    using player_t = std::unique_ptr<z_detail_player_type>;
}