#pragma once

#include <cmath>
#include <stdexcept>

#include "pidc/Particle.hpp"
#include "pidc/pic/Grid1D.hpp"

namespace pidc::pic {
namespace detail {

inline void require_finite_1d(double value, const char* message)
{
    if (!std::isfinite(value)) {
        throw std::invalid_argument{message};
    }
}

inline double acceleration_1d(double electric_field, double charge_over_mass, double dt)
{
    require_finite_1d(electric_field, "electric_field must be finite");
    require_finite_1d(charge_over_mass, "charge_over_mass must be finite");
    require_finite_1d(dt, "dt must be finite");

    const double acceleration = charge_over_mass * electric_field;
    require_finite_1d(acceleration, "leapfrog acceleration must be finite");
    return acceleration;
}

} // namespace detail

inline void initialize_leapfrog_velocity_1d(
    pidc::Particle& particle,
    double electric_field,
    double charge_over_mass,
    double dt)
{
    detail::require_finite_1d(particle.velocity.x, "particle velocity.x must be finite");

    const double acceleration = detail::acceleration_1d(electric_field, charge_over_mass, dt);
    particle.velocity.x -= 0.5 * acceleration * dt;

    detail::require_finite_1d(
        particle.velocity.x,
        "initialized leapfrog velocity.x must be finite");
}

inline void leapfrog_advance_1d(
    pidc::Particle& particle,
    const Grid1D& grid,
    double electric_field,
    double charge_over_mass,
    double dt)
{
    detail::require_finite_1d(particle.position.x, "particle position.x must be finite");
    detail::require_finite_1d(particle.velocity.x, "particle velocity.x must be finite");

    const double acceleration = detail::acceleration_1d(electric_field, charge_over_mass, dt);
    particle.velocity.x += acceleration * dt;
    detail::require_finite_1d(particle.velocity.x, "advanced leapfrog velocity.x must be finite");

    particle.position.x = grid.wrap_position(particle.position.x + particle.velocity.x * dt);
}

} // namespace pidc::pic
