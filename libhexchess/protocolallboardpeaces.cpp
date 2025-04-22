#pragma once

#include "protocol.hpp"

void lhc::protocol::payload::allBoardPeaces::parseFromStream(std::span<std::byte,1000> data){
    v.resize(data.size()/sizeof(peace));
    std::memcpy(v.data(),data.data(),data.size());
}
std::vector<std::byte> lhc::protocol::payload::allBoardPeaces::convertToStream(){
    std::vector<std::byte> data;
    data.resize(v.size()*sizeof(peace));
    assert(data.size() < 1000);
    std::memcpy(data.data(),v.data(),data.size());
    return data;
}