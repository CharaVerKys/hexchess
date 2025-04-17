#pragma once

#include "figure.hpp"

namespace figures{

class Pawn : public Figure{
    figure_type getType() const override{return figure_type::pawn;}
};
class Bishop : public Figure{
    figure_type getType() const override{return figure_type::bishop;}
};
class Knight : public Figure{
    figure_type getType() const override{return figure_type::knight;}
};
class Rook : public Figure{
    figure_type getType() const override{return figure_type::rook;}
};
class Queen : public Figure{
    figure_type getType() const override{return figure_type::queen;}
};
class King : public Figure{
    figure_type getType() const override{return figure_type::king;}
};

}