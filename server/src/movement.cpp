#include "movement.hpp"
#include "figure.hpp"

namespace movement{
    moveResult entryMove(Board& board, lhc::protocol::payload::peace_move const& move_){
        if(not details::isValidSideAndType(board, move_.from, move_.ver_type, move_.ver_side)){
            return disallowAction;
        }
        std::optional<std::vector<lhc::position>> pathToCheck = 
        details::isValidFigureMove(board, move_.from, move_.ver_type, move_.to, move_.ver_side);
        if(not pathToCheck){
            return disallowAction;
        }

        // test is there no any peaces in front of figure (except pawn)
        
        board.movePeace(move_.from, move_.to);
        details::checkPromoting(board, move_.to, move_.ver_type, move_.ver_side);
        return allowAction;
        std::abort();
    }
    namespace details{
        bool isValidSideAndType(Board& b, lhc::position const& p, figure_type const& t, figure_side const& s){
            auto column = b.getColumn(p.column);
            Cell* cell = std::ranges::next(column.begin(),p.row);
            return cell->figure and cell->figure->getType() == t and cell->figure->getSide() == s;
        }
        std::optional<std::vector<lhc::position>> isValidFigureMove(Board& board, lhc::position const& from, figure_type const& type, lhc::position const& to, figure_side const& side){
            std::vector<lhc::position> returnType_pathToCheck;
            switch(type){
                case figure_type::pawn:{
                    if(isValidPawnMove(board, from, to, side)){
                        return returnType_pathToCheck;
                    }else{
                        return std::nullopt;
                    }
                }break;
                case figure_type::bishop:{
                    
                }break;
                case figure_type::knight:{
                    
                }break;
                case figure_type::rook:{
                    
                }break;
                case figure_type::queen:{
                    
                }break;
                case figure_type::king:{
                    
                }break;
                /*unreachable*/case figure_type::invalid: std::abort();
            }
            std::abort();
        }
        bool isValidPawnMove(Board& board, lhc::position const& from, lhc::position const& to, figure_side const& side){
            if(from.column == to.column){
                const std::array<std::int8_t,11> validDoubleStepBlack = {-1,0,1,2,3,4,3,2,1,0,-1};
                (void)board; //!!!!!!
                if(side == figure_side::white and from.row - to.row  == 2){
                    if(from.column == 0 or from.column == 10){
                        return false;
                    }
                    if(from.row not_eq 6){
                        return false;
                    }
                    if(board.isAnyPeaceAt(to) or board.isAnyPeaceAt({to.column,static_cast<uint8_t>(to.row+1u)})){
                        return false;
                    }
                    return true;
                }
                else if(side == figure_side::black and from.row +2 == to.row){
                    if(from.column == 0 or from.column == 10){
                        return false;
                    }
                    if(from.row not_eq validDoubleStepBlack[from.column]){
                        return false;
                    }
                    if(board.isAnyPeaceAt(to) or board.isAnyPeaceAt({to.column,static_cast<uint8_t>(to.row-1u)})){
                        return false;
                    }
                    return true;
                }
                else if(side == figure_side::white and from.row - to.row  == 1){
                    if(board.isAnyPeaceAt(to)){
                        return false;
                    }
                    return true;
                }
                else if(side == figure_side::black and from.row +1 == to.row){
                    if(board.isAnyPeaceAt(to)){
                        return false;
                    }
                    return true;
                }
            } // if same column
            // ? cast to prevent overflow, when from 0 to 1 column
            if(std::abs(static_cast<int>(from.column) - to.column) == 1){
                if(not board.isAnyPeaceAt(to)){
                    return false;
                }
                if(from.column == 5){
                    if(side == figure_side::white){
                        if(from.row - to.row == 1){
                            return true;
                        }
                    }else{
                        if(from.row == to.row){
                            return true;
                        }
                    }
                }else if(from.column <5){
                    if(side == figure_side::white){
                        if(from.column -1 == to.column and from.row - to.row == 1){
                            return true;
                        }else if(from.column +1 == to.column and from.row == to.row){
                            return true;
                        }
                    }else{
                        if(from.column -1 == to.column and from.row == to.row){
                            return true;
                        }else if(from.column +1 == to.column and from.row +1 == to.row){
                            return true;
                        }
                    }
                }else if(from.column >5){
                    if(side == figure_side::white){
                        if(from.column +1 == to.column and from.row - to.row == 1){
                            return true;
                        }else if(from.column -1 == to.column and from.row == to.row){
                            return true;
                        }
                    }else{
                        if(from.column -1 == to.column and from.row +1 == to.row){
                            return true;
                        }else if(from.column +1 == to.column and from.row == to.row){
                            return true;
                        }
                    }
                }
            }// if abs
            return false; // all other path
        }// is valid pawn move

        void checkPromoting(Board& board, lhc::position const& pos, figure_type const& type, figure_side const& side){
            if(type == figure_type::pawn){
                if(side == figure_side::white){
                    if(pos.row == 0){
                        board.promoteToQueen(pos);
                    }
                }else{
                    const std::array<std::uint8_t,11> boardEdge = {5,6,7,8,9,10,9,8,7,6,5};
                    if(pos.row == boardEdge.at(pos.column)){
                        board.promoteToQueen(pos);
                    }
                }
            }
        }// promoting
    }//nms details
}