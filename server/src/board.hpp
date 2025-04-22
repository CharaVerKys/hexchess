#pragma once

#include "protocol.hpp"
#include <generator.hpp>
#include <array>
#include <cassert>
#include <cell.hpp>
#include <cstdlib>
#include <optional>
#include <ranges>
#include <somemacroses.hpp>

class Board{
public:
    enum class Variant : std::int8_t{default_};

private:
    std::optional<std::array<Cell,91>> o_field;

public:
    static Board initBoard(Variant variant);

    [[nodiscard]] // readable type in cpp
    std::ranges::take_view<std::ranges::drop_view<std::ranges::ref_view<std::array<Cell,91>>>>
    getColumn(std::uint8_t column);
    bool isAnyPieceAt(lhc::position const&);
    Color colorOfCell(lhc::position const&);
    std::optional<figure_side> checkVictory_and_movePiece(lhc::position const&, lhc::position const&);
    void promoteToQueen(lhc::position const&);

    lhc::protocol::payload::allBoardPieces getAllPieces();
    operator bool(){
        return o_field.has_value();
    }

private:
    Board(){}

    static void initColors(std::array<Cell,91>& field);
    static void setWhiteFigures(std::array<Cell,91>& field);
    static void setBlackFigures(std::array<Cell,91>& field);
};