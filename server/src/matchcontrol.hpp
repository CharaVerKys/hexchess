#pragma once

#include "board.hpp"
#include <future.hpp>

// ? class should control: turns, {time}, win trigger

class MatchControl{
    Board board;
    cvk::promise<game_winner> winner_trigger;
    enum turn_t:bool{white_turn,black_turn};
    turn_t turn;

public:
    cvk::future<game_winner> initDefaultMatch(lhc::player_t&& white, lhc::player_t&& black);
    void reconnectPlayer(lhc::player_t&& disconnected);

private:
    void callback_receivedFromWhite(); // accumulate data from socket to packet
    void callback_receivedFromBlack();
    void event_receivedFromWhite(); // trigger actual even on data
    void event_receivedFromBlack();
};