#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

#include "pidc/Vec2.hpp"

namespace pidc {

class CircularInfluenceDomain {
public:
    explicit CircularInfluenceDomain(double radius) : radius_{radius}
    {
        if (!(radius_ > 0.0) || !std::isfinite(radius_)) {
            throw std::invalid_argument{"CircularInfluenceDomain requires a positive finite radius"};
        }
    }

    double radius() const noexcept
    {
        return radius_;
    }

    double normalized_distance(Vec2 center, Vec2 point) const noexcept
    {
        const double dx = point.x - center.x;
        const double dy = point.y - center.y;
        return std::sqrt(dx * dx + dy * dy) / radius_;
    }

    bool contains(Vec2 center, Vec2 point) const noexcept
    {
        return normalized_distance(center, point) <= 1.0 + 1.0e-14;
    }

private:
    double radius_{1.0};
};

class RectangularInfluenceDomain {
public:
    RectangularInfluenceDomain(double half_width, double half_height)
        : half_width_{half_width},
          half_height_{half_height}
    {
        if (!(half_width_ > 0.0) || !std::isfinite(half_width_) ||
            !(half_height_ > 0.0) || !std::isfinite(half_height_)) {
            throw std::invalid_argument{
                "RectangularInfluenceDomain requires positive finite half widths"};
        }
    }

    double half_width() const noexcept
    {
        return half_width_;
    }

    double half_height() const noexcept
    {
        return half_height_;
    }

    double normalized_distance(Vec2 center, Vec2 point) const noexcept
    {
        const double rx = std::abs(point.x - center.x) / half_width_;
        const double ry = std::abs(point.y - center.y) / half_height_;
        return std::max(rx, ry);
    }

    bool contains(Vec2 center, Vec2 point) const noexcept
    {
        return normalized_distance(center, point) <= 1.0 + 1.0e-14;
    }

private:
    double half_width_{1.0};
    double half_height_{1.0};
};

} // namespace pidc
