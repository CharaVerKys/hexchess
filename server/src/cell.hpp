#pragma once

#include "figure.hpp"
#include <memory>


struct Cell{
    std::unique_ptr<Figure> figure = nullptr;
    Color color{Color::invalid};
};