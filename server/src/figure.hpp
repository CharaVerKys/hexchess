#pragma once

#include "types.hpp"

class Figure{
protected:
    figure_side side;
public:
    virtual figure_type getType()const = 0;
    figure_side getSide()const{return side;}
    virtual ~Figure(){}
    Figure(figure_side side):side(side){}
};