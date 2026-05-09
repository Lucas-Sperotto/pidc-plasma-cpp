#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "pidc/pic/Grid1D.hpp"
#include "pidc/pic/ManufacturedField1D.hpp"
#include "test_utils.hpp"

namespace {

constexpr double pi()
{
    return 3.141592653589793238462643383279502884;
}

void require_close(double actual, double expected, const char* message)
{
    pidc::test::require(pidc::test::approx_equal(actual, expected, 1.0e-12), message);
}

} // namespace

int main()
{
    const double length = 1.0;
    const double amplitude = 2.0;
    const double epsilon0 = 3.0;
    const double k = 2.0 * pi() / length;

    const pidc::pic::Grid1D grid{0.0, length, 8};
    const pidc::pic::SineManufacturedField1D field{amplitude, length, epsilon0};

    // Test 1: analytic values at known quarter-period points.
    {
        require_close(field.potential(0.0), 0.0, "phi(0) mismatch");
        require_close(field.electric_field(0.0), -amplitude * k, "E(0) mismatch");
        require_close(field.charge_density(0.0), 0.0, "rho(0) mismatch");

        require_close(field.potential(0.25 * length), amplitude, "phi(L/4) mismatch");
        require_close(field.electric_field(0.25 * length), 0.0, "E(L/4) mismatch");
        require_close(
            field.charge_density(0.25 * length),
            epsilon0 * k * k * amplitude,
            "rho(L/4) mismatch");

        require_close(field.potential(0.5 * length), 0.0, "phi(L/2) mismatch");
        require_close(field.electric_field(0.5 * length), amplitude * k, "E(L/2) mismatch");
        require_close(field.charge_density(0.5 * length), 0.0, "rho(L/2) mismatch");

        require_close(field.potential(0.75 * length), -amplitude, "phi(3L/4) mismatch");
        require_close(field.electric_field(0.75 * length), 0.0, "E(3L/4) mismatch");
        require_close(
            field.charge_density(0.75 * length),
            -epsilon0 * k * k * amplitude,
            "rho(3L/4) mismatch");
    }

    // Tests 2-4: sampling returns one value per periodic grid node.
    {
        const std::vector<double> potential = pidc::pic::sample_potential(grid, field);
        const std::vector<double> electric_field = pidc::pic::sample_electric_field(grid, field);
        const std::vector<double> charge_density = pidc::pic::sample_charge_density(grid, field);

        pidc::test::require(potential.size() == grid.size(), "sample_potential size mismatch");
        pidc::test::require(electric_field.size() == grid.size(), "sample_electric_field size mismatch");
        pidc::test::require(charge_density.size() == grid.size(), "sample_charge_density size mismatch");
    }

    // Test 5: analytic functions are periodic with period L.
    {
        const double x = 0.37 * length;
        require_close(field.potential(x + length), field.potential(x), "periodic phi mismatch at x+L");
        require_close(field.electric_field(x + length), field.electric_field(x), "periodic E mismatch at x+L");
        require_close(field.charge_density(x + length), field.charge_density(x), "periodic rho mismatch at x+L");

        require_close(field.potential(x - length), field.potential(x), "periodic phi mismatch at x-L");
        require_close(field.electric_field(x - length), field.electric_field(x), "periodic E mismatch at x-L");
        require_close(field.charge_density(x - length), field.charge_density(x), "periodic rho mismatch at x-L");
    }

    // Test 6: sampled periodic charge density has approximately zero mean.
    {
        const std::vector<double> charge_density = pidc::pic::sample_charge_density(grid, field);
        double total = 0.0;
        for (const double value : charge_density) {
            total += value;
        }

        require_close(total, 0.0, "sampled charge density should have zero mean");
    }

    // Test 7: invalid field parameters fail explicitly.
    {
        bool threw_bad_length = false;
        try {
            const pidc::pic::SineManufacturedField1D bad_field{1.0, 0.0, 1.0};
            (void)bad_field.potential(0.0);
        } catch (const std::invalid_argument&) {
            threw_bad_length = true;
        }
        pidc::test::require(threw_bad_length, "non-positive length must throw invalid_argument");

        bool threw_bad_epsilon0 = false;
        try {
            const pidc::pic::SineManufacturedField1D bad_field{1.0, 1.0, 0.0};
            (void)bad_field.charge_density(0.0);
        } catch (const std::invalid_argument&) {
            threw_bad_epsilon0 = true;
        }
        pidc::test::require(threw_bad_epsilon0, "non-positive epsilon0 must throw invalid_argument");
    }

    std::cout << "manufactured_field_1d test passed\n";
    return 0;
}
