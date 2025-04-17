#pragma once

#include <generator.hpp>

namespace lhc{

struct pair_depth{std::uint8_t drop; std::uint8_t take;};
cvk::generator<pair_depth> field_ranges();

}