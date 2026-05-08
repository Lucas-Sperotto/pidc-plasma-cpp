#pragma once

#include "pidc/Vec2.hpp"

namespace pidc {

class Domain2D {
public:
    Domain2D(Vec2 lower, Vec2 upper);

    const Vec2& lower() const noexcept;
    const Vec2& upper() const noexcept;

    double width() const noexcept;
    double height() const noexcept;

    bool contains(Vec2 point) const noexcept;
    Vec2 wrapPeriodic(Vec2 point) const noexcept;

private:
    Vec2 lower_{};
    Vec2 upper_{};

    static double wrapValue(double value, double lower, double upper) noexcept;
};

} // namespace pidc
