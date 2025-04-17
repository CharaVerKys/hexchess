#pragma once

#include "types.hpp"
#include <array>
namespace lhc::constants{

inline std::array<Color,11> firstColor = {
    Color::first,
    Color::second,
    Color::third,

    Color::first,
    Color::second,

    Color::third,

    Color::second,
    Color::first,

    Color::third,
    Color::second,
    Color::first,
}; //firstColor

} // nms