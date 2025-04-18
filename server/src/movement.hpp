#pragma once

#include "board.hpp"

enum moveResult : std::int8_t{
    invalid = -1,
    allowAction = 0,
    disallowAction = 1,
};

namespace movement{
    moveResult entryMove(Board&, lhc::protocol::payload::peace_move const&);
    bool seqClearToDo(Board&, std::vector<lhc::position> const& toTest);
    namespace details{
        bool isValidSideAndType(Board&, lhc::position const&, figure_type const&, figure_side const&);
        // ? return type is path to check is possible for figure, nullopt means figure cant more to given position
        std::optional<std::vector<lhc::position>> isValidFigureMove(Board&, lhc::position const& from, figure_type const&, lhc::position const& to, figure_side const& side);
        bool isValidPawnMove(Board&, lhc::position const& from, lhc::position const& to, figure_side const& side);
        void checkPromoting(Board&, lhc::position const&, figure_type const&, figure_side const&);
        std::optional<std::vector<lhc::position>> isValidBishopMove(Board&, lhc::position const& from, lhc::position const& to);
        namespace bishop{
            enum direction{
                left, right, left_top, right_top, left_bot, right_bot
            };
            std::unique_ptr<std::vector<lhc::position>> makeOneMoveInDirection(direction const& dir, lhc::position const& from);
            std::unique_ptr<std::vector<lhc::position>> addToPath(std::unique_ptr<std::vector<lhc::position>>&& nextSteps, lhc::position const& nextStep);
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toRight(direction const& dir, lhc::position const& from);
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toRight_top(direction const& dir, lhc::position const& from);
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toRight_bot(direction const& dir, lhc::position const& from);
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toLeft(direction const& dir, lhc::position const& from);
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toLeft_top(direction const& dir, lhc::position const& from);
            std::unique_ptr<std::vector<lhc::position>> imp_makeOneMoveInDirection_toLeft_bot(direction const& dir, lhc::position const& from);
            std::optional<std::vector<lhc::position>> tryRunTo(direction const&, lhc::position const& from, lhc::position const& to);
        }
        std::optional<std::vector<lhc::position>> isValidKnightMove(lhc::position const& from, lhc::position const& to);
        namespace knight{
            bool testRightTop(lhc::position const& from, lhc::position const& to);
            bool testLeftBot(lhc::position const& from, lhc::position const& to);
            bool testRightBot(lhc::position const& from, lhc::position const& to);
            bool testLeftTop(lhc::position const& from, lhc::position const& to);
            bool testRight(lhc::position const& from, lhc::position const& to);
            bool testLeft(lhc::position const& from, lhc::position const& to);
        }
    }
}