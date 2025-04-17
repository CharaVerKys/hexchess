#pragma once

#include "types.hpp"

class Figure{
public:
    virtual figure_type getType()const = 0;
    virtual ~Figure(){}
};