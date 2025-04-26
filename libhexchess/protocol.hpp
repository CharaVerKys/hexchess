#pragma once

#include "types.hpp"
#include <cstdint>
#include <span>
#include <vector>

namespace lhc::protocol{
    
    enum class action : std::uint8_t{
        invalid = 0,
        sendAllBoardPieces = 1,
        movePieceBroadcast = 2,
        requestMovePiece = 3,
        wrongMovePiece = 4,
        gameEnd [[deprecated]] = 5,
        createMatch = 6,
        connectToMatch = 7, // reconnect call this
        requestListOfAllMatches = 8,
        sendListOfAllMatches = 9,
        abortGame = 10,
        enemyTurn = 11,
        gameEnd_winWhite = 12,
        gameEnd_winBlack = 13,
        requestID = 14,
        answerID = 15,
        error_noID = 16,
        deleteMatch = 17, // from list of all matches
    };

    namespace payload{
        struct piece{figure_type type; lhc::position position; figure_side side;};
        static_assert(sizeof(piece)==4); // rely while memcpy
        class allBoardPieces{
            std::vector<piece> v;
          public:
            explicit allBoardPieces(std::vector<piece>&& vec) : v(std::move(vec)) {}
            allBoardPieces(){}
            std::vector<piece> const &getAllPieces() { return v; }
            void parseFromStream(std::span<std::byte,1000>);
            std::vector<std::byte> convertToStream() const;
            std::size_t binSize()const{return v.size()*sizeof(piece);}
        };
    
        struct piece_move{
            piece_move(lhc::position from, figure_type ver_type, figure_side ver_side, lhc::position to)
            : from(from), ver_type(ver_type), ver_side(ver_side), to(to) {}
            piece_move(){}
            piece_move(const piece_move &) = default;
            piece_move(piece_move &&) = default;
            piece_move &operator=(const piece_move &other){from = other.from; to = other.to; ver_side = other.ver_side; ver_type = other.ver_type; return*this;}
            piece_move &operator=(piece_move &&other){from = other.from; to = other.to; ver_side = other.ver_side; ver_type = other.ver_type; return*this;}
            lhc::position from;
            figure_type ver_type;
            figure_side ver_side;
            lhc::position to;
          protected:
            const std::uint8_t unused[2]={0};
        };
        static_assert(sizeof(piece_move) == 8);
        struct createMatch{
          figure_side side;
          const std::uint8_t reserved[3] = {0};
        };
        static_assert(sizeof(createMatch) == 4);

        struct match{figure_side side; lhc::unique_id id;};
        struct listOfAllMatches{
            std::vector<match> vec;
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
      std::strong_ordering operator<=>(const PacketHeader& other) const {
          if (auto cmp = userID <=> other.userID; cmp not_eq std::strong_ordering::equal){ return cmp;}
          if (auto cmp = totalSize <=> other.totalSize; cmp not_eq std::strong_ordering::equal){ return cmp;}
          return action_ <=> other.action_;
      }
    };
}
