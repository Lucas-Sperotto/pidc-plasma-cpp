#include "pidc/Domain2D.hpp"

#include <cmath>
#include <stdexcept>

namespace pidc {

Domain2D::Domain2D(Vec2 lower, Vec2 upper) : lower_{lower}, upper_{upper}
{
    if (upper_.x <= lower_.x || upper_.y <= lower_.y) {
        throw std::invalid_argument{"Domain2D requires upper bounds greater than lower bounds"};
    }
}

const Vec2& Domain2D::lower() const noexcept
{
    return lower_;
}

const Vec2& Domain2D::upper() const noexcept
{
    return upper_;
}

double Domain2D::width() const noexcept
{
    return upper_.x - lower_.x;
}

double Domain2D::height() const noexcept
{
    return upper_.y - lower_.y;
}

bool Domain2D::contains(Vec2 point) const noexcept
{
    return point.x >= lower_.x && point.x < upper_.x && point.y >= lower_.y && point.y < upper_.y;
}

Vec2 Domain2D::wrapPeriodic(Vec2 point) const noexcept
{
    return Vec2{
        wrapValue(point.x, lower_.x, upper_.x),
        wrapValue(point.y, lower_.y, upper_.y),
    };
}

double Domain2D::wrapValue(double value, double lower, double upper) noexcept
{
    const double length = upper - lower;
    const double shifted = value - lower;
    const double wrapped = shifted - length * std::floor(shifted / length);
    return lower + wrapped;
}

} // namespace pidc
