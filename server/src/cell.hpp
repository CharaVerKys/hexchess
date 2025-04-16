#pragma once

#include <cstdint>

class Figure;

enum class Color : std::int8_t{
    invalid,
    first,
    second,
    third,
};

struct Cell{
    Figure* figure = nullptr;
    Color color{Color::invalid};
};