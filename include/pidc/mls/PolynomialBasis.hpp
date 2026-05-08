#pragma once

#include <array>

#include "pidc/Vec2.hpp"

namespace pidc {

inline std::array<double, 3> linear_basis(Vec2 x) noexcept
{
    return {1.0, x.x, x.y};
}

inline std::array<double, 3> linear_basis_dx() noexcept
{
    return {0.0, 1.0, 0.0};
}

inline std::array<double, 3> linear_basis_dy() noexcept
{
    return {0.0, 0.0, 1.0};
}

} // namespace pidc
