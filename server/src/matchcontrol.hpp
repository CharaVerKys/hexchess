#pragma once

#include "board.hpp"
#include <future.hpp>
#include "coroutinesthings.hpp"
#include "defaultcoroutine.hpp"
#include "unittype.h"
#include "packetsocketoperations.hpp"

// ? class should control: turns, {time}, win trigger

class MatchControl{
    std::optional<Board> board;
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
        DefaultCoroutine white, black;
    } socketReceiveProcessLifetimeHandle;

public:
  cvk::coroutine_t initDefaultMatch(lhc::player_t white,
                                            lhc::player_t black);
  std::error_code reconnectPlayer(lhc::player_t &&disconnected);
  bool isIdForReconnect(lhc::unique_id const &id);
  bool canBeDestroyed()const {
    //todo old matches should be terminated
    return(not cvk::socket::reliable_is_open(players.white->socket.value()) and
           not cvk::socket::reliable_is_open(players.black->socket.value()) ) or
           aborted;}

private:
    DefaultCoroutine receivedFromWhite();
    DefaultCoroutine receivedFromBlack();
    cvk::future<Unit> abortGame();
    cvk::future<Unit> answerOnlyAction(lhc::player_t&, lhc::protocol::action const&);
    cvk::future<Unit> broadcastPieceMove(lhc::protocol::payload::piece_move const&);
    cvk::future<Unit> processPacket(lhc::player_t&, cvk::socket::packet_t const&);
    cvk::future<Unit> broadcastWin(figure_side const&);
};