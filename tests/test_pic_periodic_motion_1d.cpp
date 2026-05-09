#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

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

void require_inside_domain(const pidc::pic::Grid1D& grid, double x, const char* message)
{
    pidc::test::require(std::isfinite(x), "particle position must be finite");
    pidc::test::require(x >= grid.xmin && x < grid.xmax, message);
}

} // namespace

int main()
{
    const pidc::pic::Grid1D grid{0.0, 1.0, 32};

    // Test 1: one-step crossings wrap through both sides and keep the semi-open convention.
    {
        pidc::Particle right = make_particle(0.95, 0.30);
        pidc::pic::initialize_leapfrog_velocity_1d(right, 0.0, 1.0, 0.5);
        pidc::pic::leapfrog_advance_1d(right, grid, 0.0, 1.0, 0.5);
        require_close(right.position.x, 0.10, 1.0e-12, "right crossing wrap mismatch");
        require_inside_domain(grid, right.position.x, "right crossing must remain in [xmin, xmax)");

        pidc::Particle left = make_particle(0.05, -0.30);
        pidc::pic::initialize_leapfrog_velocity_1d(left, 0.0, 1.0, 0.5);
        pidc::pic::leapfrog_advance_1d(left, grid, 0.0, 1.0, 0.5);
        require_close(left.position.x, 0.90, 1.0e-12, "left crossing wrap mismatch");
        require_inside_domain(grid, left.position.x, "left crossing must remain in [xmin, xmax)");

        pidc::Particle exact_xmax = make_particle(0.75, 0.50);
        pidc::pic::initialize_leapfrog_velocity_1d(exact_xmax, 0.0, 1.0, 0.5);
        pidc::pic::leapfrog_advance_1d(exact_xmax, grid, 0.0, 1.0, 0.5);
        require_close(exact_xmax.position.x, 0.0, 1.0e-12, "xmax image should wrap to xmin");
        require_inside_domain(grid, exact_xmax.position.x, "xmax image must remain semi-open");
    }

    // Test 2: multiple particles keep count, order, velocities and wrapped analytic positions.
    {
        std::vector<pidc::Particle> particles{
            make_particle(0.02, 0.37),
            make_particle(0.25, -0.22),
            make_particle(0.49, 1.14),
            make_particle(0.73, -0.91),
            make_particle(0.98, 0.0)};

        std::vector<double> initial_positions;
        std::vector<double> velocities;
        initial_positions.reserve(particles.size());
        velocities.reserve(particles.size());
        for (const auto& particle : particles) {
            initial_positions.push_back(particle.position.x);
            velocities.push_back(particle.velocity.x);
        }

        const std::size_t initial_count = particles.size();
        const double dt = 0.17;
        const int steps = 50;

        for (auto& particle : particles) {
            pidc::pic::initialize_leapfrog_velocity_1d(particle, 0.0, 1.0, dt);
        }

        for (int step = 0; step < steps; ++step) {
            for (auto& particle : particles) {
                pidc::pic::leapfrog_advance_1d(particle, grid, 0.0, 1.0, dt);
            }
        }

        pidc::test::require(particles.size() == initial_count, "periodic advance must preserve particle count");
        for (std::size_t i = 0; i < particles.size(); ++i) {
            const double expected =
                grid.wrap_position(initial_positions[i] + velocities[i] * dt * static_cast<double>(steps));

            require_inside_domain(grid, particles[i].position.x, "advanced particle must remain in [xmin, xmax)");
            require_close(
                particles[i].position.x,
                expected,
                1.0e-12,
                "wrapped multi-step position mismatch");
            require_close(
                particles[i].velocity.x,
                velocities[i],
                1.0e-12,
                "zero-field periodic motion should preserve velocity");
        }
    }

    std::cout << "pic_periodic_motion_1d test passed\n";
    return 0;
}
