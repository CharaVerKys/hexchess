#pragma once

#include "board.hpp"
#include <future.hpp>
#include "defaultcoroutine.hpp"
#include "unittype.h"
#include "packetsocketoperations.hpp"

// ? class should control: turns, {time}, win trigger

class MatchControl{
    Board board;
    ///
    enum turn_t:bool{white_turn,black_turn};
    turn_t turn = white_turn;
    turn_t playerTurn(lhc::player_t const&p){if(p->side == figure_side::black){return black_turn;} return white_turn;}
    void nextTurn(){switch(turn){case white_turn: turn = black_turn; break; case black_turn: turn = white_turn;}}
    ///
    bool aborted = false;
    struct {
        lhc::player_t white, black;
    } players;
    struct{
        std::coroutine_handle<> coroutine;
        game_winner winner = game_winner::invalid_game_winner;
    } finishGame;
    struct{
        DefaultCoroutine white, black;
    } socketReceiveProcessLifetimeHandle;

public:
    cvk::future<game_winner> initDefaultMatch(lhc::player_t&& white, lhc::player_t&& black);
    std::error_code reconnectPlayer(lhc::player_t&& disconnected);
    bool canBeDestroyed(){return not players.black->socket->is_open() and not players.black->socket->is_open() and (aborted or finishGame.coroutine.done());}

private:
    DefaultCoroutine receivedFromWhite();
    DefaultCoroutine receivedFromBlack();
    cvk::future<Unit> abortGame();
    cvk::future<Unit> answerOnlyAction(lhc::player_t&, lhc::protocol::action const&);
    cvk::future<Unit> broadcastPeaceMove(lhc::protocol::payload::peace_move const&);
    cvk::future<Unit> processPacket(lhc::player_t&, cvk::socket::packet_t const&);
    cvk::future<Unit> broadcastWin(game_winner const&);
};