#pragma once

#include <cmath>

#include "pidc/Domain2D.hpp"
#include "pidc/Vec2.hpp"

namespace pidc {

class PeriodicBoundary2D {
public:
    explicit PeriodicBoundary2D(Domain2D domain)
        : domain_{domain}
    {
    }

    Vec2 wrap(Vec2 point) const noexcept
    {
        return domain_.wrapPeriodic(point);
    }

    Vec2 minimum_image(Vec2 displacement) const noexcept
    {
        return Vec2{
            minimum_image_component(displacement.x, domain_.width()),
            minimum_image_component(displacement.y, domain_.height()),
        };
    }

private:
    Domain2D domain_;

    static double minimum_image_component(double value, double length) noexcept
    {
        const double half_length = 0.5 * length;
        double wrapped = std::fmod(value, length);

        if (wrapped > half_length) {
            wrapped -= length;
        } else if (wrapped < -half_length) {
            wrapped += length;
        }

        return wrapped;
    }
};

} // namespace pidc
