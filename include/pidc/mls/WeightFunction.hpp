#pragma once

namespace pidc {

inline double weight_quartic(double r) noexcept
{
    if (r >= 1.0) return 0.0;
    if (r <= 0.0) return 1.0;
    return 1.0 - 6.0*r*r + 8.0*r*r*r - 3.0*r*r*r*r;
}

inline double weight_quartic_deriv(double r) noexcept
{
    if (r >= 1.0 || r <= 0.0) return 0.0;
    return -12.0*r * (1.0 - r) * (1.0 - r);
}

} // namespace pidc
