#pragma once

#include "types.hpp"
#include <cstdint>
#include <vector>

namespace lhc::protocol{
    
    enum class action : std::uint8_t{
        invalid = 0,
        sendAllBoardPeaces = 1,
        movePeaceBroadcast = 2,
        requestMovePeace = 3,
        wrongMovePeace = 4,
        gameEnd = 5,
        createMatch = 6,
        connectToMatch = 7, // reconnect call this
        requestListOfAllMatches = 8,
        sendListOfAllMatches = 9,
    };

    namespace payload{
        struct peace{figure_type type; lhc::position position; figure_side side;};
        class allBoardPeaces{
            std::vector<peace> v;
          public:
            std::vector<peace> const& getAllPeaces(){return v;}
            void setPeaces(std::vector<peace>&&vec){v = vec;};
            // void parseFromStream(std::span<std::byte,1000>);
            // std::vector<std::byte> convertToStream();
        };
    }
}
