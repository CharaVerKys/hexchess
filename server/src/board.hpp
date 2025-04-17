#pragma once

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

    PRIVATE_CONTEXT(struct pair_depth{std::uint8_t drop; std::uint8_t take;};)
    //todo probably function should be private
    static cvk::generator<pair_depth> fieldRanges();

    [[nodiscard]] // readable type in cpp
    std::ranges::take_view<std::ranges::drop_view<std::ranges::ref_view<std::array<Cell,91>>>>
    getColumn(std::uint8_t column);


private:
    Board(){}

    static void initColors(std::array<Cell,91> field);
};