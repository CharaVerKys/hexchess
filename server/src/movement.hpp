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
        bool isValidFigureMove(Board&, lhc::position const& from, figure_type const&, lhc::position const& to, figure_side const& side);
        bool isValidPawnMove(Board&, lhc::position const& from, lhc::position const& to, figure_side const& side);
    }
}