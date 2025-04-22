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
    
        struct peace_move{
            lhc::position from;
            figure_type ver_type;
            figure_side ver_side;
            lhc::position to;
        };
    }
    struct PacketHeader{
      PacketHeader() : totalSize(0), userID(0), action_(action::invalid) {}
      PacketHeader(std::size_t totalSize, lhc::unique_id userID, action action)
          : totalSize(totalSize), userID(userID), action_(action) {}
      PacketHeader(const PacketHeader &) = default;
      PacketHeader(PacketHeader &&) = default;
      PacketHeader &operator=(const PacketHeader &other){this->action_ = other.action_;this->totalSize = other.totalSize; this->userID = other.userID; return *this;}
      PacketHeader &operator=(PacketHeader &&other){*this = static_cast<PacketHeader&>(other); return *this;}
      std::size_t totalSize;
      lhc::unique_id userID;
      action action_;
      const std::uint8_t reserved[3] = {0};
    };
}
