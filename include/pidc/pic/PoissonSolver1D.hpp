#pragma once

#include <cmath>
#include <complex>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/pic/Grid1D.hpp"

namespace pidc::pic {

struct PoissonResult1D {
    std::vector<double> potential;
    std::vector<double> electric_field;
};

inline PoissonResult1D solve_poisson_periodic_1d(
    const Grid1D& grid,
    const std::vector<double>& charge_density,
    double epsilon0 = 1.0)
{
    const std::size_t n = grid.size();
    if (charge_density.size() != n) {
        throw std::invalid_argument{"solve_poisson_periodic_1d requires charge_density.size() == grid.size()"};
    }

    if (!(epsilon0 > 0.0) || !std::isfinite(epsilon0)) {
        throw std::invalid_argument{"solve_poisson_periodic_1d requires epsilon0 > 0"};
    }

    for (const double rho : charge_density) {
        if (!std::isfinite(rho)) {
            throw std::invalid_argument{"solve_poisson_periodic_1d requires finite charge density values"};
        }
    }

    constexpr double pi = 3.141592653589793238462643383279502884;
    const double dx = grid.dx;
    const double inverse_n = 1.0 / static_cast<double>(n);

    std::vector<std::complex<double>> rho_hat(n, {0.0, 0.0});
    for (std::size_t k = 0; k < n; ++k) {
        std::complex<double> sum{0.0, 0.0};
        for (std::size_t j = 0; j < n; ++j) {
            const double angle =
                -2.0 * pi * static_cast<double>(j) * static_cast<double>(k) / static_cast<double>(n);
            sum += charge_density[j] * std::complex<double>{std::cos(angle), std::sin(angle)};
        }
        rho_hat[k] = sum;
    }

    std::vector<std::complex<double>> phi_hat(n, {0.0, 0.0});
    for (std::size_t k = 1; k < n; ++k) {
        const double sin_term = std::sin(pi * static_cast<double>(k) / static_cast<double>(n));
        const double lambda = 4.0 * sin_term * sin_term / (dx * dx);
        phi_hat[k] = rho_hat[k] / (epsilon0 * lambda);
    }

    std::vector<double> potential(n, 0.0);
    for (std::size_t j = 0; j < n; ++j) {
        std::complex<double> sum{0.0, 0.0};
        for (std::size_t k = 0; k < n; ++k) {
            const double angle =
                2.0 * pi * static_cast<double>(j) * static_cast<double>(k) / static_cast<double>(n);
            sum += phi_hat[k] * std::complex<double>{std::cos(angle), std::sin(angle)};
        }
        potential[j] = (sum * inverse_n).real();
    }

    double mean_potential = 0.0;
    for (const double phi : potential) {
        mean_potential += phi;
    }
    mean_potential *= inverse_n;
    for (double& phi : potential) {
        phi -= mean_potential;
    }

    std::vector<double> electric_field(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        const std::size_t left = (i + n - 1) % n;
        const std::size_t right = (i + 1) % n;
        electric_field[i] = -(potential[right] - potential[left]) / (2.0 * dx);
    }

    return PoissonResult1D{potential, electric_field};
}

} // namespace pidc::pic
