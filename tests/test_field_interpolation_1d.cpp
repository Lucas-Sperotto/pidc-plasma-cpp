#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/pic/FieldInterpolation1D.hpp"
#include "pidc/pic/Grid1D.hpp"
#include "pidc/pic/ManufacturedField1D.hpp"
#include "test_utils.hpp"

namespace {

pidc::Particle make_particle(double x)
{
    pidc::Particle particle;
    particle.position.x = x;
    return particle;
}

void require_close(double actual, double expected, double tolerance, const char* message)
{
    pidc::test::require(pidc::test::approx_equal(actual, expected, tolerance), message);
}

} // namespace

int main()
{
    const pidc::pic::Grid1D grid{0.0, 1.0, 4};
    const std::vector<double> field_nodes{0.0, 4.0, 8.0, 12.0};

    // Test 1: exact CIC values inside cells and on nodes.
    {
        require_close(
            pidc::pic::interpolate_field_cic_1d(grid, field_nodes, 0.125),
            2.0,
            1.0e-12,
            "midpoint of first cell should average nodes 0 and 1");
        require_close(
            pidc::pic::interpolate_field_cic_1d(grid, field_nodes, 0.25),
            4.0,
            1.0e-12,
            "point exactly on node 1 should return node value");
        require_close(
            pidc::pic::interpolate_field_cic_1d(grid, field_nodes, 0.375),
            6.0,
            1.0e-12,
            "midpoint of second cell should average nodes 1 and 2");
    }

    // Test 2: periodic interpolation across the right boundary.
    {
        require_close(
            pidc::pic::interpolate_field_cic_1d(grid, field_nodes, 0.875),
            6.0,
            1.0e-12,
            "last-cell midpoint should average nodes nx-1 and 0");
        require_close(
            pidc::pic::interpolate_field_cic_1d(grid, field_nodes, -0.125),
            6.0,
            1.0e-12,
            "negative position should wrap to last-cell midpoint");
        require_close(
            pidc::pic::interpolate_field_cic_1d(grid, field_nodes, 1.125),
            2.0,
            1.0e-12,
            "position greater than xmax should wrap to first-cell midpoint");
    }

    // Test 3: vector overload preserves particle order and size.
    {
        const std::vector<pidc::Particle> particles{
            make_particle(0.125),
            make_particle(0.25),
            make_particle(0.875)};
        const std::vector<double> values =
            pidc::pic::interpolate_field_cic_1d(grid, field_nodes, particles);

        pidc::test::require(values.size() == particles.size(), "vector overload size mismatch");
        require_close(values[0], 2.0, 1.0e-12, "vector overload particle 0 mismatch");
        require_close(values[1], 4.0, 1.0e-12, "vector overload particle 1 mismatch");
        require_close(values[2], 6.0, 1.0e-12, "vector overload particle 2 mismatch");
    }

    // Test 4: uniform nodal field remains uniform at arbitrary wrapped positions.
    {
        const std::vector<double> uniform_nodes(grid.size(), 3.5);
        const double positions[] = {-1.25, -0.10, 0.0, 0.62, 1.0, 2.37};
        for (const double x : positions) {
            require_close(
                pidc::pic::interpolate_field_cic_1d(grid, uniform_nodes, x),
                3.5,
                1.0e-12,
                "uniform field should interpolate to itself");
        }
    }

    // Test 5: sampled manufactured electric field is recovered to CIC accuracy.
    {
        const pidc::pic::Grid1D fine_grid{0.0, 1.0, 128};
        const pidc::pic::SineManufacturedField1D sine_field{1.0, 1.0, 1.0};
        const std::vector<double> sampled_field =
            pidc::pic::sample_electric_field(fine_grid, sine_field);

        const double positions[] = {0.031, 0.147, 0.333, 0.618, 0.891};
        for (const double x : positions) {
            const double interpolated =
                pidc::pic::interpolate_field_cic_1d(fine_grid, sampled_field, x);
            const double exact = sine_field.electric_field(x);
            pidc::test::require(
                std::abs(interpolated - exact) < 3.0e-3,
                "manufactured electric field interpolation error should be below 3e-3");
            require_close(
                pidc::pic::interpolate_field_cic_1d(fine_grid, sampled_field, x + 1.0),
                interpolated,
                1.0e-12,
                "manufactured interpolation should be periodic");
        }
    }

    // Test 6: invalid nodal data fail explicitly.
    {
        bool threw_bad_size = false;
        try {
            (void)pidc::pic::interpolate_field_cic_1d(grid, {1.0, 2.0}, 0.25);
        } catch (const std::invalid_argument&) {
            threw_bad_size = true;
        }
        pidc::test::require(threw_bad_size, "wrong field size must throw invalid_argument");

        std::vector<double> bad_values = field_nodes;
        bad_values[2] = std::numeric_limits<double>::infinity();
        bool threw_non_finite = false;
        try {
            (void)pidc::pic::interpolate_field_cic_1d(grid, bad_values, 0.25);
        } catch (const std::invalid_argument&) {
            threw_non_finite = true;
        }
        pidc::test::require(threw_non_finite, "non-finite field value must throw invalid_argument");
    }

    std::cout << "field_interpolation_1d test passed\n";
    return 0;
}
