#pragma once

#include "figure.hpp"

namespace figures{

class Pawn : public Figure{
    type getType() override{return type::pawn;}
};
class Bishop : public Figure{
    type getType() override{return type::bishop;}
};
class Knight : public Figure{
    type getType() override{return type::knight;}
};
class Rook : public Figure{
    type getType() override{return type::rook;}
};
class Queen : public Figure{
    type getType() override{return type::queen;}
};
class King : public Figure{
    type getType() override{return type::king;}
};

}