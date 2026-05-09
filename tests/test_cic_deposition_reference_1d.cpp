#include <cstdint>
#include <cstddef>
#include <iostream>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/pic/ChargeDeposition1D.hpp"
#include "pidc/pic/Grid1D.hpp"
#include "test_utils.hpp"

namespace {

struct FixedSeedRng {
    std::uint32_t state{0};

    explicit FixedSeedRng(std::uint32_t seed) : state{seed} {}

    double next_unit()
    {
        state = 1664525u * state + 1013904223u;
        return static_cast<double>(state) / 4294967296.0;
    }
};

pidc::Particle make_particle(double x, std::size_t species_id = 0)
{
    pidc::Particle particle;
    particle.position.x = x;
    particle.species_id = species_id;
    return particle;
}

std::vector<pidc::Particle> seeded_particles(std::size_t count, std::uint32_t seed)
{
    FixedSeedRng rng{seed};

    std::vector<pidc::Particle> particles;
    particles.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        particles.push_back(make_particle(rng.next_unit(), i % 2));
    }
    return particles;
}

std::vector<double> manual_cic_reference(
    const pidc::pic::Grid1D& grid,
    const std::vector<pidc::Particle>& particles,
    const std::vector<pidc::Species>& species)
{
    std::vector<double> expected(grid.size(), 0.0);
    for (const auto& particle : particles) {
        const double q = species[particle.species_id].charge;
        const double f = grid.fraction_in_cell(particle.position.x);
        const std::size_t left = grid.left_node_index(particle.position.x);
        const std::size_t right = grid.right_node_index(particle.position.x);

        expected[left] += q * (1.0 - f);
        expected[right] += q * f;
    }
    return expected;
}

double sum(const std::vector<double>& values)
{
    double total = 0.0;
    for (const double value : values) {
        total += value;
    }
    return total;
}

double total_particle_charge(
    const std::vector<pidc::Particle>& particles,
    const std::vector<pidc::Species>& species)
{
    double total = 0.0;
    for (const auto& particle : particles) {
        total += species[particle.species_id].charge;
    }
    return total;
}

void require_close(double actual, double expected, double tolerance, const char* message)
{
    pidc::test::require(pidc::test::approx_equal(actual, expected, tolerance), message);
}

void require_vector_close(
    const std::vector<double>& actual,
    const std::vector<double>& expected,
    double tolerance,
    const char* message)
{
    pidc::test::require(actual.size() == expected.size(), "reference vector size mismatch");
    for (std::size_t i = 0; i < actual.size(); ++i) {
        pidc::test::require(
            pidc::test::approx_equal(actual[i], expected[i], tolerance),
            message);
    }
}

} // namespace

int main()
{
    // Test 1: one particle in the center of a cell deposits 50/50 to neighbors.
    {
        const pidc::pic::Grid1D grid{0.0, 1.0, 8};
        const std::vector<pidc::Species> species{{2.0, 1.0, "reference"}};
        const double x_center_of_cell = grid.coordinate(3) + 0.5 * grid.dx;
        const std::vector<pidc::Particle> particles{make_particle(x_center_of_cell)};

        const std::vector<double> charges =
            pidc::pic::deposit_charge_cic_1d(grid, particles, species);

        for (std::size_t i = 0; i < charges.size(); ++i) {
            const double expected = (i == 3 || i == 4) ? 1.0 : 0.0;
            require_close(charges[i], expected, 1.0e-12, "single centered cell deposition mismatch");
        }
        require_close(sum(charges), 2.0, 1.0e-12, "single centered cell charge should be conserved");
    }

    // Test 2: one particle at the domain center node deposits all charge to that node.
    {
        const pidc::pic::Grid1D grid{0.0, 1.0, 8};
        const std::vector<pidc::Species> species{{-1.0, 1.0, "electron"}};
        const std::vector<pidc::Particle> particles{make_particle(0.5)};

        const std::vector<double> charges =
            pidc::pic::deposit_charge_cic_1d(grid, particles, species);

        for (std::size_t i = 0; i < charges.size(); ++i) {
            const double expected = (i == 4) ? -1.0 : 0.0;
            require_close(charges[i], expected, 1.0e-12, "single domain-center node deposition mismatch");
        }
        require_close(sum(charges), -1.0, 1.0e-12, "single domain-center charge should be conserved");
    }

    // Test 3: fixed-seed particle set is reproducible and globally conservative.
    {
        const pidc::pic::Grid1D grid{0.0, 1.0, 16};
        const std::vector<pidc::Species> species{
            {-1.0, 1.0, "electron"},
            {2.0, 1.0, "ion"}};
        const std::uint32_t seed = 20260509u;

        const std::vector<pidc::Particle> particles = seeded_particles(32, seed);
        const std::vector<pidc::Particle> repeated_particles = seeded_particles(32, seed);
        const std::vector<double> charges =
            pidc::pic::deposit_charge_cic_1d(grid, particles, species);
        const std::vector<double> repeated_charges =
            pidc::pic::deposit_charge_cic_1d(grid, repeated_particles, species);
        const std::vector<double> expected = manual_cic_reference(grid, particles, species);

        require_vector_close(charges, expected, 1.0e-12, "fixed-seed reference deposition mismatch");
        require_vector_close(charges, repeated_charges, 1.0e-12, "fixed-seed deposition must be reproducible");
        require_close(
            sum(charges),
            total_particle_charge(particles, species),
            1.0e-12,
            "fixed-seed total charge should be conserved");
    }

    std::cout << "cic_deposition_reference_1d test passed\n";
    return 0;
}
