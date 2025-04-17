#include "field_ranges.hpp"
#include <array>
namespace lhc{

cvk::generator<pair_depth> field_ranges(){
    std::uint8_t dropped = 0;
    constexpr std::array<std::uint8_t,11> rowSize = {6,7,8,9,10,11,10,9,8,7,6}; 

    for(std::uint8_t const& size : rowSize){
        co_yield {.drop = dropped, .take = size}; 
        dropped += size;
    }
    assert(dropped == 91);
}

}