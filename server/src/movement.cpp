#include "movement.hpp"
#include "figure.hpp"
#include "simplealgorithm.hpp"
#include <algorithm>
#include <iostream>

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
        if(not seqClearToDo(board, *pathToCheck)){
            return disallowAction;
        }

        // test is there no any peaces in front of figure (except pawn)
        
        board.movePeace(move_.from, move_.to);
        details::checkPromoting(board, move_.to, move_.ver_type, move_.ver_side);
        return allowAction;
        std::abort();
    }

    bool seqClearToDo(Board& board, std::vector<lhc::position> const& toTest){
        int max = std::max(int(toTest.size())-1,0);
        for(auto& pos : std::views::take(toTest,max)){
            if(board.isAnyPeaceAt(pos)){
                return false;
            }
        }
        return true;
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
                    return isValidBishopMove(board,from,to);
                }break;
                case figure_type::knight:{
                    return isValidKnightMove(from,to);
                }break;
                case figure_type::rook:{
                    return isValidRookMove(board, from,to);
                }break;
                case figure_type::queen:{
                    if(auto res = isValidRookMove(board, from,to); res){
                        return res;
                    }
                    return isValidBishopMove(board,from,to);
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

        std::unique_ptr<std::vector<lhc::position>> addToPath(std::unique_ptr<std::vector<lhc::position>>&& nextSteps, lhc::position const& nextStep){
            if(nextSteps){
                nextSteps->push_back(nextStep);
                return nextSteps;
            }else{
                auto ptr = std::make_unique<std::vector<lhc::position>>();
                ptr->push_back(nextStep);
                return ptr;
            }
        }
        
        std::optional<std::vector<lhc::position>> isValidBishopMove(Board& board, lhc::position const& from, lhc::position const& to){
            if(board.colorOfCell(from) not_eq board.colorOfCell(to)){
                return std::nullopt;
            }
            if(from.column == to.column){
                return std::nullopt;
            }
            if(from.column > to.column){
                auto left = bishop::tryRunTo(bishop::left,from,to);
                auto left_top = bishop::tryRunTo(bishop::left_top,from,to);
                auto left_bot = bishop::tryRunTo(bishop::left_bot,from,to);
                // assert(foundOne == cvk::algo::only_one_true(std::array<decltype(left),3>{left,left_top,left_bot}));
                auto path = left ? left : (left_top ? left_top : (left_bot ? left_bot : std::nullopt));
                return path;
            }else{
                auto right = bishop::tryRunTo(bishop::right,from,to);
                auto right_top = bishop::tryRunTo(bishop::right_top,from,to);
                auto right_bot = bishop::tryRunTo(bishop::right_bot,from,to);
                // assert(foundOne == cvk::algo::only_one_true(std::array<decltype(right),3>{right,right_top,right_bot}));
                auto path = right ? right : (right_top ? right_top : (right_bot ? right_bot : std::nullopt));
                return path;
            }
        }// is valid bishop move
        namespace bishop{
            std::unique_ptr<std::vector<lhc::position>> makeOneMoveInDirection(direction const& dir, lhc::position const& from){
                switch (dir) {
                    case right:{
                        return imp_makeOneMoveInDirection_toRight(dir, from);
                    }break;
                    case left:{
                        return imp_makeOneMoveInDirection_toLeft(dir, from);
                    }break;
                    case right_top:{
                        return imp_makeOneMoveInDirection_toRight_top(dir, from);
                    }break;
                    case right_bot:{
                        return imp_makeOneMoveInDirection_toRight_bot(dir, from);
                    }break;
                    case left_top:{
                        return imp_makeOneMoveInDirection_toLeft_top(dir, from);
                    }break;
                    case left_bot:{
                        return imp_makeOneMoveInDirection_toLeft_bot(dir, from);
                    }break;
                }
                std::abort();
            }// make one move in dir


            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toRight(direction const& dir, lhc::position const& from){
                if(from.column < 4){
                    lhc::position nextStep{static_cast<uint8_t>(from.column+2),static_cast<uint8_t>(from.row+1)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }
                if(from.column == 4){
                    lhc::position nextStep{static_cast<uint8_t>(from.column+2),from.row};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }
                if(from.column > 8){
                    return nullptr;
                }
                assert(from.column <= 8 and from.column >=5);
                const std::array<std::uint8_t,9> max_context = {0,0,0,0,0,10,9,8,7};
                if(from.row == 0 or from.row == max_context.at(from.column)){
                    return nullptr;
                }
                lhc::position nextStep{static_cast<uint8_t>(from.column+2),static_cast<uint8_t>(from.row-1)};
                auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                return addToPath(std::move(nextSteps), nextStep);
            } // imp make one right

            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toRight_top(direction const& dir, lhc::position const& from){
                if(from.column == 10){
                    return nullptr;
                }
                if(from.row == 0){
                    return nullptr;
                }
                std::uint8_t difRow;
                if(from.column < 5){
                    difRow = 1;
                }else{
                    difRow = 2;
                }
                int newRow = from.row;
                newRow -= difRow;
                if(newRow >=0){
                    lhc::position nextStep{static_cast<uint8_t>(from.column+1),static_cast<uint8_t>(newRow)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }else{
                    return nullptr;
                }
            } // imp make one right top
            
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toRight_bot(direction const& dir, lhc::position const& from){
                if(from.column == 10){
                    return nullptr;
                }
                std::uint8_t difRow;
                if(from.column < 5){
                    difRow = 2;
                }else{
                    difRow = 1;
                }
                const std::array<std::uint8_t,11> max_depth = {6,7,8,9,10,11,10,9,8,7,6};
                if(from.row+difRow < max_depth.at(from.column+1)){
                    lhc::position nextStep{static_cast<uint8_t>(from.column+1),static_cast<uint8_t>(from.row+difRow)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }else{
                    return nullptr;
                }
            } // imp make one right bot

            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toLeft(direction const& dir, lhc::position const& from){
                if(from.column > 6){
                    lhc::position nextStep{static_cast<uint8_t>(from.column-2),static_cast<uint8_t>(from.row+1)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }
                if(from.column == 6){
                    lhc::position nextStep{static_cast<uint8_t>(from.column-2),from.row};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }
                if(from.column < 2){
                    return nullptr;
                }
                assert(from.column >= 2 and from.column <=5);
                const std::array<std::uint8_t,6> max_context = {0,0,7,8,9,10};
                if(from.row == 0 or from.row == max_context.at(from.column)){
                    return nullptr;
                }
                lhc::position nextStep{static_cast<uint8_t>(from.column-2),static_cast<uint8_t>(from.row-1)};
                auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                return addToPath(std::move(nextSteps), nextStep);
            } // imp make one left

            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toLeft_top(direction const& dir, lhc::position const& from){
                if(from.column == 0){
                    return nullptr;
                }
                if(from.row == 0){
                    return nullptr;
                }
                std::uint8_t difRow;
                if(from.column <= 5){
                    difRow = 2;
                }else{
                    difRow = 1;
                }
                int newRow = from.row;
                newRow -= difRow;
                if(newRow >=0){
                    lhc::position nextStep{static_cast<uint8_t>(from.column-1),static_cast<uint8_t>(newRow)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }else{
                    return nullptr;
                }

            }// imp make one left top

            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toLeft_bot(direction const& dir, lhc::position const& from){
                if(from.column == 0){
                    return nullptr;
                }
                std::uint8_t difRow;
                if(from.column > 5){
                    difRow = 2;
                }else{
                    difRow = 1;
                }
                const std::array<std::uint8_t,10> max_depth = {0,7,8,9,10,11,10,9,8,7};
                if(from.row+difRow < max_depth.at(from.column-1)){
                    lhc::position nextStep{static_cast<uint8_t>(from.column-1),static_cast<uint8_t>(from.row+difRow)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }else{
                    return nullptr;
                }
            }// imp make one left right
            std::optional<std::vector<lhc::position>> tryRunTo(direction const& dir, lhc::position const& from, lhc::position const& to){
                std::unique_ptr<std::vector<lhc::position>> result = makeOneMoveInDirection(dir, from);
                if(not result){
                    return std::nullopt;
                }
                std::ranges::reverse(*result);
                if (auto it = std::ranges::find(*result, to); it not_eq result->end()) {
                    result->erase(std::next(it), result->end());
                    return *result;
                }else{
                    return std::nullopt;
                }
            }
        } //nms bishop

        std::optional<std::vector<lhc::position>> isValidKnightMove(lhc::position const& from, lhc::position const& to){
            bool valid{
                knight::testRightTop(from, to) or
                knight::testLeftBot(from, to) or
                knight::testRightBot(from, to) or
                knight::testLeftTop(from, to) or
                knight::testRight(from, to) or
                knight::testLeft(from, to) 
            };
            if(valid){
                return {{}};
            }else{
                return std::nullopt;
            }
        }
        namespace knight{
            bool testRightTop(lhc::position const& from, lhc::position const& to){
                if(from.column+1 == to.column){
                    if(from.column >= 5){
                        return from.row == to.row+3;
                    }
                    if(from.column < 5){
                        return from.row == to.row+2;
                    }
                }

                if(from.column == 4 and to.column == 6){
                    return from.row == to.row+2;
                }
                if(from.column +2 == to.column){
                    if(from.column < 4){
                        return from.row == to.row+1;
                    }
                    if(from.column >= 5){
                        return from.row == to.row+3;
                    }
                }
                return false;
            }
            bool testLeftBot(lhc::position const& from, lhc::position const& to){
                if(from.column-1 == to.column){
                    if(from.column >5){
                        return from.row == to.row-3;
                    }
                    if(from.column <= 5){
                        return from.row == to.row-2;
                    }
                }

                if(from.column == 6 and to.column == 4){
                    return from.row == to.row-2;
                }
                if(from.column -2 == to.column){
                    if(from.column > 5){
                        return from.row == to.row-3;
                    }
                    if(from.column <= 5){
                        return from.row == to.row-1;
                    }
                }
                return false;
            }
            bool testRightBot(lhc::position const& from, lhc::position const& to){
                if(from.column+1 == to.column){
                    if(from.column < 5){
                        return from.row == to.row-3;
                    }
                    if(from.column >= 5){
                        return from.row == to.row-2;
                    }
                }

                if(from.column == 4 and to.column == 6){
                    return from.row == to.row-2;
                }
                if(from.column +2 == to.column){
                    if(from.column < 4){
                        return from.row == to.row-3;
                    }
                    if(from.column >= 5){
                        return from.row == to.row-1;
                    }
                }
                return false;

            }
            bool testLeftTop(lhc::position const& from, lhc::position const& to){
                if(from.column-1 == to.column){
                    if(from.column >5){
                        return from.row == to.row+2;
                    }
                    if(from.column <= 5){
                        return from.row == to.row+3;
                    }
                }

                if(from.column == 6 and to.column == 4){
                    return from.row == to.row+2;
                }
                if(from.column -2 == to.column){
                    if(from.column < 5){
                        return from.row == to.row+3;
                    }
                    if(from.column >= 5){
                        return from.row == to.row+1;
                    }
                }
                return false;
            }
            bool testRight(lhc::position const& from, lhc::position const& to){
                if(from.column+3 == to.column){
                    if(from.column == 5){
                        return (from.row == to.row+1) or (from.row == to.row+2);
                    }
                    if(from.column == 4 and to.column == 7){
                        return (from.row == to.row+1) or (from.row == to.row);
                    }
                    if(from.column == 3 and to.column == 6){
                        return (from.row == to.row-1) or (from.row == to.row);
                    }
                    if(from.column == 7 and to.column == 10){
                        return (from.row == to.row+1) or (from.row == to.row+2);
                    }
                    if(from.column == 6 and to.column == 9){
                        return (from.row == to.row+1) or (from.row == to.row+2);
                    }
                    if(from.column <= 2){
                        return (from.row == to.row-1) or (from.row == to.row-2);
                    }
                }
                return false;
            }
            bool testLeft(lhc::position const& from, lhc::position const& to){
                if(from.column-3 == to.column){
                    if(from.column == 5){
                        return (from.row == to.row+1) or (from.row == to.row+2);
                    }
                    if(from.column == 6 and to.column == 3){
                        return (from.row == to.row+1) or (from.row == to.row);
                    }
                    if(from.column == 7 and to.column == 4){
                        return (from.row == to.row-1) or (from.row == to.row);
                    }
                    if(from.column == 3 and to.column == 0){
                        return (from.row == to.row+1) or (from.row == to.row+2);
                    }
                    if(from.column == 4 and to.column == 1){
                        return (from.row == to.row+1) or (from.row == to.row+2);
                    }
                    if(from.column >= 8){
                        return (from.row == to.row-1) or (from.row == to.row-2);
                    }
                }
                return false;
            }

        }//nms knight

        std::optional<std::vector<lhc::position>> isValidRookMove(Board& board, lhc::position const& from, lhc::position const& to){
            if(from.column == to.column){
                std::size_t columnSize = board.getColumn(from.column).size();
                std::vector<lhc::position> moves;
                if(from.row > to.row){
                    // beeing signed critical because count down
                    for(std::int8_t it = from.row-1;it >= to.row; --it){
                        moves.emplace_back(from.column,it);
                    }
                }else{
                    for(std::uint8_t it = from.row+1;it <= to.row and to.row < columnSize; ++it){
                        moves.emplace_back(from.column,it);
                    }
                }
                if(not moves.empty()){
                    return moves;
                }else{
                    return std::nullopt;
                }
            }else if(from.column > to.column){
                auto left_top = rook::tryRunTo(rook::left_top,from,to);
                auto left_bot = rook::tryRunTo(rook::left_bot,from,to);
                auto path = left_top ? left_top : (left_bot ? left_bot : std::nullopt);
                return path;
            }else{
                auto right_top = rook::tryRunTo(rook::right_top,from,to);
                auto right_bot = rook::tryRunTo(rook::right_bot,from,to);
                auto path = right_top ? right_top : (right_bot ? right_bot : std::nullopt);
                return path;
            }
        }
        namespace rook{
            std::unique_ptr<std::vector<lhc::position>> makeOneMoveInDirection(direction const& dir, lhc::position const& from){
                switch (dir) {
                    case right_top:{
                        return imp_makeOneMoveInDirection_toRight_top(dir, from);
                    }break;
                    case right_bot:{
                        return imp_makeOneMoveInDirection_toRight_bot(dir, from);
                    }break;
                    case left_top:{
                        return imp_makeOneMoveInDirection_toLeft_top(dir, from);
                    }break;
                    case left_bot:{
                        return imp_makeOneMoveInDirection_toLeft_bot(dir, from);
                    }break;
                }
                std::abort();
            }
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toRight_top(direction const& dir, lhc::position const& from){
                if(from.column < 5){
                    lhc::position nextStep{static_cast<uint8_t>(from.column+1),static_cast<uint8_t>(from.row)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }else if(from.column < 10){
                    lhc::position nextStep{static_cast<uint8_t>(from.column+1),static_cast<uint8_t>(from.row-1)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }
                return nullptr;
            }
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toRight_bot(direction const& dir, lhc::position const& from){
                if(from.column < 5){
                    lhc::position nextStep{static_cast<uint8_t>(from.column+1),static_cast<uint8_t>(from.row+1)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }else if(from.column < 10){
                    lhc::position nextStep{static_cast<uint8_t>(from.column+1),static_cast<uint8_t>(from.row)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }
                return nullptr;
            }
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toLeft_top(direction const& dir, lhc::position const& from){
                if(from.column >= 11){return nullptr;}
                if(from.column > 5){
                    lhc::position nextStep{static_cast<uint8_t>(from.column-1),static_cast<uint8_t>(from.row)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }else if(from.column > 0){
                    lhc::position nextStep{static_cast<uint8_t>(from.column-1),static_cast<uint8_t>(from.row-1)};
                    auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                    return addToPath(std::move(nextSteps), nextStep);
                }
                return nullptr;
            }
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toLeft_bot(direction const& dir, lhc::position const& from){
                const std::array<std::uint8_t,10> maxSize = {6,7,8,9,10,11,10,9,8,7};
                std::uint8_t newRow;
                assert(from.column < 11);
                if(from.column == 0){
                    return nullptr;
                }
                if(from.column <= 5){
                    newRow = from.row;
                }else{
                    newRow = from.row+1;
                }
                if(newRow >= maxSize.at(newRow)){
                    return nullptr;
                }
                lhc::position nextStep{static_cast<uint8_t>(from.column-1),static_cast<uint8_t>(newRow)};
                auto nextSteps = makeOneMoveInDirection(dir, nextStep);
                return addToPath(std::move(nextSteps), nextStep);
            }
            std::optional<std::vector<lhc::position>> tryRunTo(direction const& dir, lhc::position const& from, lhc::position const& to){
                std::unique_ptr<std::vector<lhc::position>> result = makeOneMoveInDirection(dir, from);
                if(not result){
                    return std::nullopt;
                }
                std::ranges::reverse(*result);
                if (auto it = std::ranges::find(*result, to); it not_eq result->end()) {
                    result->erase(std::next(it), result->end());
                    return *result;
                }else{
                    return std::nullopt;
                }
            }
        }
    }//nms details
}