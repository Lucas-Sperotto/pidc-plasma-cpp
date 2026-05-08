#pragma once

#include <cstddef>

#include "pidc/Vec2.hpp"

namespace pidc {

struct Node {
    std::size_t id{0};
    Vec2 position{};
    double volume{0.0};
};

} // namespace pidc
