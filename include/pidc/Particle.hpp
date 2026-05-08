#pragma once

#include "pidc/Vec2.hpp"

namespace pidc {

struct Particle {
    Vec2 position{};
    Vec2 velocity{};
    double charge{0.0};
    double mass{1.0};
};

} // namespace pidc
