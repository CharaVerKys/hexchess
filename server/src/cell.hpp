#pragma once

#include <types.hpp>

class Figure;

struct Cell{
    Figure* figure = nullptr;
    Color color{Color::invalid};
};