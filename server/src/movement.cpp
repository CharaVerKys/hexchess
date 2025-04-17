#include "movement.hpp"
#include "figure.hpp"

namespace movement{
    moveResult entryMove(Board& board, lhc::protocol::payload::peace_move const& move_){
        if(not details::isValidSideAndType(board, move_.from, move_.ver_type, move_.ver_side)){
            return disallowAction;
        }
        // test is it valid for figure to point this position

        // test is there no any peaces in front of figure
        
        std::abort();
    }
    namespace details{
        bool isValidSideAndType(Board& b, lhc::position const& p, figure_type const& t, figure_side const& s){
            auto column = b.getColumn(p.column);
            Cell* cell = std::ranges::next(column.begin(),p.row);
            return cell->figure and cell->figure->getType() == t and cell->figure->getSide() == s;
        }

    }
}