
#include <board.hpp>

int main(){
    Board board = Board::initBoard(Board::Variant::default_);
    auto result = board.getColumn(5);
    assert(result.size() == 11);

}