#pragma once

#include "figure.hpp"

namespace figures{

class Pawn : public Figure{
    figure_type getType() const override{return figure_type::pawn;}
public:
    Pawn(figure_side side) : Figure(side) {}
};
class Bishop : public Figure{
    figure_type getType() const override{return figure_type::bishop;}
public:
    Bishop(figure_side side) : Figure(side) {}
};
class Knight : public Figure{
    figure_type getType() const override{return figure_type::knight;}
public:
    Knight(figure_side side) : Figure(side) {}
};
class Rook : public Figure{
    figure_type getType() const override{return figure_type::rook;}
public:
    Rook(figure_side side) : Figure(side) {}
};
class Queen : public Figure{
    figure_type getType() const override{return figure_type::queen;}
public:
    Queen(figure_side side) : Figure(side) {}
};
class King : public Figure{
    figure_type getType() const override{return figure_type::king;}
public:
    King(figure_side side) : Figure(side) {}
};

}