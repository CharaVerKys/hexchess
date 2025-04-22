#pragma once

#include "flat_map.hpp"
#include "matchcontrol.hpp"

class SessionsControl{
    std::vector<MatchControl> matches;
    cvk::flat_map<lhc::player_t,figure_side> allOpenSessions;

    void callback_incomingConnection(); // also accumulate all data in packet

    void event_deleteSession();
    void event_openSession();
    void event_connectToSession();
    void event_requestListOfAllSessions();
    void event_reconnectToMatch();
};