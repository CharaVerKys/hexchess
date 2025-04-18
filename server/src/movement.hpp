#pragma once

#include "board.hpp"

enum moveResult : std::int8_t{
    invalid = -1,
    allowAction = 0,
    disallowAction = 1,
};

namespace movement{
    moveResult entryMove(Board&, lhc::protocol::payload::peace_move const&);
    namespace details{
        bool isValidSideAndType(Board&, lhc::position const&, figure_type const&, figure_side const&);
        // ? return type is path to check is possible for figure, nullopt means figure cant more to given position
        std::optional<std::vector<lhc::position>> isValidFigureMove(Board&, lhc::position const& from, figure_type const&, lhc::position const& to, figure_side const& side);
        bool isValidPawnMove(Board&, lhc::position const& from, lhc::position const& to, figure_side const& side);
        void checkPromoting(Board&, lhc::position const&, figure_type const&, figure_side const&);
    }
}