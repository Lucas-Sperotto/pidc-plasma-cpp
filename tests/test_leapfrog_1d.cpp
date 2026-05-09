#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "pidc/Particle.hpp"
#include "pidc/pic/Grid1D.hpp"
#include "pidc/pic/LeapFrog1D.hpp"
#include "test_utils.hpp"

namespace {

pidc::Particle make_particle(double x, double vx)
{
    pidc::Particle particle;
    particle.position.x = x;
    particle.velocity.x = vx;
    return particle;
}

void require_close(double actual, double expected, double tolerance, const char* message)
{
    pidc::test::require(pidc::test::approx_equal(actual, expected, tolerance), message);
}

} // namespace

int main()
{
    // Test 1: zero electric field gives uniform motion.
    {
        const pidc::pic::Grid1D grid{0.0, 1.0, 16};
        pidc::Particle particle = make_particle(0.2, 0.1);
        const double dt = 0.05;

        pidc::pic::initialize_leapfrog_velocity_1d(particle, 0.0, 1.0, dt);
        require_close(particle.velocity.x, 0.1, 1.0e-12, "zero field should not shift velocity");

        for (int step = 0; step < 10; ++step) {
            pidc::pic::leapfrog_advance_1d(particle, grid, 0.0, 1.0, dt);
        }

        require_close(particle.position.x, 0.25, 1.0e-12, "uniform motion position mismatch");
        require_close(particle.velocity.x, 0.1, 1.0e-12, "uniform motion velocity mismatch");
    }

    // Test 2: constant field matches the analytic constant-acceleration solution.
    {
        const pidc::pic::Grid1D grid{-10.0, 10.0, 32};
        const double x0 = 0.2;
        const double v0 = 0.4;
        const double electric_field = 2.0;
        const double charge_over_mass = -0.5;
        const double acceleration = electric_field * charge_over_mass;
        const double dt = 0.01;

        pidc::Particle particle = make_particle(x0, v0);
        pidc::pic::initialize_leapfrog_velocity_1d(
            particle,
            electric_field,
            charge_over_mass,
            dt);

        require_close(
            particle.velocity.x,
            v0 - 0.5 * acceleration * dt,
            1.0e-12,
            "initial half-step velocity mismatch");

        for (int step = 0; step < 2; ++step) {
            pidc::pic::leapfrog_advance_1d(
                particle,
                grid,
                electric_field,
                charge_over_mass,
                dt);
        }

        const double time = 2.0 * dt;
        const double expected_position = x0 + v0 * time + 0.5 * acceleration * time * time;
        const double expected_half_velocity = v0 + acceleration * (time - 0.5 * dt);

        require_close(
            particle.position.x,
            expected_position,
            1.0e-12,
            "constant-field position mismatch after two steps");
        require_close(
            particle.velocity.x,
            expected_half_velocity,
            1.0e-12,
            "constant-field half-step velocity mismatch after two steps");
    }

    // Test 3: drift-only leap-frog is reversible with dt and -dt.
    {
        const pidc::pic::Grid1D grid{-10.0, 10.0, 32};
        const double x0 = 0.2;
        const double v0 = 0.03;
        const double dt = 0.07;

        pidc::Particle particle = make_particle(x0, v0);
        pidc::pic::initialize_leapfrog_velocity_1d(particle, 0.0, 1.0, dt);

        for (int step = 0; step < 100; ++step) {
            pidc::pic::leapfrog_advance_1d(particle, grid, 0.0, 1.0, dt);
        }
        for (int step = 0; step < 100; ++step) {
            pidc::pic::leapfrog_advance_1d(particle, grid, 0.0, 1.0, -dt);
        }

        require_close(particle.position.x, x0, 1.0e-10, "reversible drift position mismatch");
        require_close(particle.velocity.x, v0, 1.0e-10, "reversible drift velocity mismatch");
    }

    // Test 4: particle positions are wrapped periodically after an advance.
    {
        const pidc::pic::Grid1D grid{0.0, 1.0, 16};
        pidc::Particle particle = make_particle(0.95, 0.2);
        const double dt = 0.5;

        pidc::pic::initialize_leapfrog_velocity_1d(particle, 0.0, 1.0, dt);
        pidc::pic::leapfrog_advance_1d(particle, grid, 0.0, 1.0, dt);

        require_close(particle.position.x, 0.05, 1.0e-12, "periodic wrap mismatch");
        pidc::test::require(
            particle.position.x >= grid.xmin && particle.position.x < grid.xmax,
            "wrapped position must remain in [xmin, xmax)");
    }

    // Test 5: non-finite inputs fail explicitly.
    {
        const pidc::pic::Grid1D grid{0.0, 1.0, 16};
        const double nan = std::numeric_limits<double>::quiet_NaN();

        bool initialize_threw = false;
        try {
            pidc::Particle particle = make_particle(0.1, 0.2);
            pidc::pic::initialize_leapfrog_velocity_1d(particle, 1.0, 1.0, nan);
        } catch (const std::invalid_argument&) {
            initialize_threw = true;
        }
        pidc::test::require(initialize_threw, "non-finite dt must throw during initialization");

        bool advance_threw = false;
        try {
            pidc::Particle particle = make_particle(0.1, 0.2);
            pidc::pic::initialize_leapfrog_velocity_1d(particle, 0.0, 1.0, 0.1);
            pidc::pic::leapfrog_advance_1d(particle, grid, nan, 1.0, 0.1);
        } catch (const std::invalid_argument&) {
            advance_threw = true;
        }
        pidc::test::require(advance_threw, "non-finite electric field must throw during advance");
    }

    std::cout << "leapfrog_1d test passed\n";
    return 0;
}
