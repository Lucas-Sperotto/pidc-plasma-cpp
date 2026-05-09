#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "pidc/pic/Grid1D.hpp"
#include "pidc/pic/ManufacturedField1D.hpp"
#include "pidc/pic/PoissonSolver1D.hpp"
#include "test_utils.hpp"

namespace {

double linf_error(const std::vector<double>& actual, const std::vector<double>& expected)
{
    pidc::test::require(actual.size() == expected.size(), "linf_error size mismatch");

    double error = 0.0;
    for (std::size_t i = 0; i < actual.size(); ++i) {
        error = std::max(error, std::abs(actual[i] - expected[i]));
    }
    return error;
}

double mean(const std::vector<double>& values)
{
    double total = 0.0;
    for (const double value : values) {
        total += value;
    }
    return total / static_cast<double>(values.size());
}

void require_close(double actual, double expected, double tolerance, const char* message)
{
    pidc::test::require(pidc::test::approx_equal(actual, expected, tolerance), message);
}

} // namespace

int main()
{
    const pidc::pic::Grid1D grid{0.0, 1.0, 128};
    const pidc::pic::SineManufacturedField1D field{1.25, 1.0, 2.0};

    // Test 1: periodic Poisson recovers the manufactured sine potential.
    {
        const std::vector<double> rho = pidc::pic::sample_charge_density(grid, field);
        const pidc::pic::PoissonResult1D result =
            pidc::pic::solve_poisson_periodic_1d(grid, rho, field.epsilon0);

        std::vector<double> exact_phi;
        std::vector<double> exact_electric_field;
        exact_phi.reserve(grid.size());
        exact_electric_field.reserve(grid.size());
        for (std::size_t i = 0; i < grid.size(); ++i) {
            const double x = grid.coordinate(i);
            exact_phi.push_back(field.potential(x));
            exact_electric_field.push_back(field.electric_field(x));
        }

        pidc::test::require(result.potential.size() == grid.size(), "potential size mismatch");
        pidc::test::require(result.electric_field.size() == grid.size(), "electric field size mismatch");
        pidc::test::require(
            linf_error(result.potential, exact_phi) < 1.0e-3,
            "manufactured potential Linf error should be below 1e-3");
        pidc::test::require(
            linf_error(result.electric_field, exact_electric_field) < 5.0e-3,
            "manufactured electric field Linf error should be below 5e-3");
    }

    // Test 2: gauge fixes the mean potential to zero.
    {
        const std::vector<double> rho = pidc::pic::sample_charge_density(grid, field);
        const pidc::pic::PoissonResult1D result =
            pidc::pic::solve_poisson_periodic_1d(grid, rho, field.epsilon0);
        require_close(mean(result.potential), 0.0, 1.0e-12, "mean potential should be zero");
    }

    // Test 3: zero charge density gives zero potential and zero electric field.
    {
        const std::vector<double> zero_density(grid.size(), 0.0);
        const pidc::pic::PoissonResult1D result =
            pidc::pic::solve_poisson_periodic_1d(grid, zero_density, field.epsilon0);

        for (const double phi : result.potential) {
            require_close(phi, 0.0, 1.0e-12, "zero density potential mismatch");
        }
        for (const double electric_field : result.electric_field) {
            require_close(electric_field, 0.0, 1.0e-12, "zero density electric field mismatch");
        }
    }

    // Test 4: invalid inputs fail explicitly.
    {
        bool threw_bad_size = false;
        try {
            (void)pidc::pic::solve_poisson_periodic_1d(grid, {1.0, 2.0}, field.epsilon0);
        } catch (const std::invalid_argument&) {
            threw_bad_size = true;
        }
        pidc::test::require(threw_bad_size, "wrong charge density size must throw invalid_argument");

        bool threw_bad_epsilon0 = false;
        try {
            const std::vector<double> rho(grid.size(), 0.0);
            (void)pidc::pic::solve_poisson_periodic_1d(grid, rho, 0.0);
        } catch (const std::invalid_argument&) {
            threw_bad_epsilon0 = true;
        }
        pidc::test::require(threw_bad_epsilon0, "non-positive epsilon0 must throw invalid_argument");
    }

    std::cout << "poisson_solver_1d test passed\n";
    return 0;
}
