#pragma once

#include <cstddef>

#include "pidc/Vec2.hpp"

namespace pidc {

struct Particle {
    Vec2        position{};
    Vec2        velocity{};
    std::size_t species_id{0};
};

} // namespace pidc
