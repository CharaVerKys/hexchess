#include "protocol.hpp"
#include <iostream>

void lhc::protocol::payload::allBoardPieces::parseFromStream(std::vector<std::byte> const& data){
    v.resize(data.size()/sizeof(piece));
    std::memcpy(v.data(),data.data(),data.size());
}
std::vector<std::byte> lhc::protocol::payload::allBoardPieces::convertToStream()const{
    std::vector<std::byte> data;
    data.resize(v.size()*sizeof(piece));
    assert(data.size() < 1000);
    std::memcpy(data.data(),v.data(),data.size());
    return data;
}