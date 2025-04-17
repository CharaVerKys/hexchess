#pragma once

#include "board.hpp"

enum moveResult : bool{
    allowAction = true,
    disallowAction = false
};

namespace movement{
    moveResult entryMove(Board&, lhc::protocol::payload::peace_move const&);
    namespace details{
        bool isValidSideAndType(Board&, lhc::position const&, figure_type const&, figure_side const&);
    }
}