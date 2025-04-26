#include "protocol.hpp"
#include <iostream>

int main(){
    std::vector<lhc::protocol::payload::piece> pieces;
    pieces.emplace_back(figure_type::pawn, lhc::position{6,5}, figure_side::white);
    pieces.emplace_back(figure_type::king, lhc::position{6,1}, figure_side::white);
    pieces.emplace_back(figure_type::bishop, lhc::position{6,9}, figure_side::black);
    lhc::protocol::payload::allBoardPieces all(std::move(pieces));
    assert(all.getAllPieces().size() == 3);
    auto pld = all.convertToStream();

    assert(12 == all.binSize());
    lhc::protocol::payload::allBoardPieces all2;
    all2.parseFromStream(pld);
    std::cout << all2.getAllPieces().size();
    assert(12 == all2.binSize());
    assert(all2.getAllPieces().size() == 3);
    
}