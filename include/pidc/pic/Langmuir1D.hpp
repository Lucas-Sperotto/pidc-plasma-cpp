#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/pic/ChargeDeposition1D.hpp"
#include "pidc/pic/FieldInterpolation1D.hpp"
#include "pidc/pic/Grid1D.hpp"
#include "pidc/pic/LeapFrog1D.hpp"
#include "pidc/pic/PoissonSolver1D.hpp"

namespace pidc::pic {

struct LangmuirConfig1D {
    double length{1.0};
    std::size_t nx{64};
    std::size_t particles{128};
    double dt{0.05};
    std::size_t steps{2500};
    double perturbation{1.0e-3};
};

struct LangmuirResult1D {
    double omega_observed{0.0};
    double relative_energy_drift{0.0};
    double max_charge_error{0.0};
};

// Runs a 1D electrostatic PIC Langmuir oscillation in normalized units:
//   epsilon0=1, n0=1, q_electron=-1, m_electron=1 → omega_pe=1.
// Macrocharge: q_mac = -length/particles.
// Ion background: rho_ion = +1 (uniform, immobile).
// Returns observed plasma frequency, energy drift and charge conservation error.
inline LangmuirResult1D run_langmuir_1d(const LangmuirConfig1D& config)
{
    if (config.nx == 0) {
        throw std::invalid_argument{"run_langmuir_1d: nx must be > 0"};
    }
    if (config.particles == 0) {
        throw std::invalid_argument{"run_langmuir_1d: particles must be > 0"};
    }
    if (!(config.dt > 0.0) || !std::isfinite(config.dt)) {
        throw std::invalid_argument{"run_langmuir_1d: dt must be positive and finite"};
    }
    if (config.steps == 0) {
        throw std::invalid_argument{"run_langmuir_1d: steps must be > 0"};
    }

    constexpr double pi = 3.141592653589793238462643383279502884;

    // Macrocharge and macromass (q_mac/m_mac == q/m == -1 in normalized units)
    const double q_mac    = -config.length / static_cast<double>(config.particles);
    const double m_mac    = -q_mac; // L/N
    const double q_over_m = -1.0;

    Grid1D grid{0.0, config.length, config.nx};
    const double dx = grid.dx;

    pidc::Species electron;
    electron.charge = q_mac;
    electron.mass   = m_mac;
    electron.label  = "e";
    const std::vector<pidc::Species> species{electron};

    // Uniform positions with sinusoidal mode-1 displacement
    std::vector<pidc::Particle> parts(config.particles);
    for (std::size_t p = 0; p < config.particles; ++p) {
        const double x0 = (static_cast<double>(p) + 0.5) * config.length
                           / static_cast<double>(config.particles);
        const double x1 = x0 + config.perturbation
                           * std::sin(2.0 * pi * x0 / config.length);
        parts[p].position.x = grid.wrap_position(x1);
        parts[p].velocity.x = 0.0;
        parts[p].species_id = 0;
    }

    // Deposit, add ion background, solve Poisson
    const auto poisson_step = [&]() -> PoissonResult1D {
        const auto Q = deposit_charge_cic_1d(grid, parts, species);
        std::vector<double> rho(config.nx);
        for (std::size_t i = 0; i < config.nx; ++i) {
            rho[i] = Q[i] / dx + 1.0;
        }
        return solve_poisson_periodic_1d(grid, rho, 1.0);
    };

    // Re[ E_hat[k=1] ] = sum_j E_j * cos(-2pi*j/nx) — oscillates at omega_pe
    const auto mode1_signal = [&](const PoissonResult1D& res) -> double {
        double re = 0.0;
        for (std::size_t j = 0; j < config.nx; ++j) {
            const double angle = -2.0 * pi * static_cast<double>(j)
                                  / static_cast<double>(config.nx);
            re += res.electric_field[j] * std::cos(angle);
        }
        return re;
    };

    // Macroparticle kinetic energy: 0.5 * m_mac * sum(v^2)
    const auto kinetic_energy = [&]() -> double {
        double ke = 0.0;
        for (const auto& p : parts) {
            ke += p.velocity.x * p.velocity.x;
        }
        return 0.5 * m_mac * ke;
    };

    // Electrostatic field energy: (epsilon0/2) * sum(E_i^2) * dx
    const auto potential_energy = [&](const PoissonResult1D& res) -> double {
        double pe = 0.0;
        for (const double e : res.electric_field) {
            pe += e * e;
        }
        return 0.5 * pe * dx;
    };

    // Charge conservation error: |sum(Q_i) - sum(q_p)|
    const auto charge_error = [&]() -> double {
        const auto Q = deposit_charge_cic_1d(grid, parts, species);
        double sum_q = 0.0;
        for (const double q : Q) {
            sum_q += q;
        }
        const double total_q = static_cast<double>(config.particles) * q_mac;
        return std::abs(sum_q - total_q);
    };

    // Initialize leapfrog: set v(-dt/2) = v(0) - (q/m)*E*dt/2
    {
        const auto res0 = poisson_step();
        for (auto& p : parts) {
            const double e_at_p = interpolate_field_cic_1d(
                grid, res0.electric_field, p.position.x);
            initialize_leapfrog_velocity_1d(p, e_at_p, q_over_m, config.dt);
        }
    }

    // Main time loop
    std::vector<double> mode1_ts;
    mode1_ts.reserve(config.steps);
    std::vector<double> e_total;
    e_total.reserve(config.steps);
    double max_charge_err = 0.0;

    for (std::size_t step = 0; step < config.steps; ++step) {
        const auto res = poisson_step();

        mode1_ts.push_back(mode1_signal(res));
        e_total.push_back(kinetic_energy() + potential_energy(res));

        if (step % 50 == 0) {
            max_charge_err = std::max(max_charge_err, charge_error());
        }

        for (auto& p : parts) {
            const double e_at_p = interpolate_field_cic_1d(
                grid, res.electric_field, p.position.x);
            leapfrog_advance_1d(p, grid, e_at_p, q_over_m, config.dt);
        }
    }

    // Extract dominant frequency from mode1 time series via O(n^2) DFT
    // (no FFT library; n=steps is small enough for a unit test)
    double omega_observed = 0.0;
    {
        const std::size_t ns   = mode1_ts.size();
        const std::size_t kmax = ns / 2;
        double best_amp_sq = -1.0;
        for (std::size_t k = 1; k <= kmax; ++k) {
            double re = 0.0, im = 0.0;
            for (std::size_t n = 0; n < ns; ++n) {
                const double angle = -2.0 * pi
                                      * static_cast<double>(n)
                                      * static_cast<double>(k)
                                      / static_cast<double>(ns);
                re += mode1_ts[n] * std::cos(angle);
                im += mode1_ts[n] * std::sin(angle);
            }
            const double amp_sq = re * re + im * im;
            if (amp_sq > best_amp_sq) {
                best_amp_sq     = amp_sq;
                omega_observed = 2.0 * pi * static_cast<double>(k)
                                  / (static_cast<double>(ns) * config.dt);
            }
        }
    }

    // Energy drift: max relative deviation from mean (skipping first 10% transient)
    double relative_energy_drift = 0.0;
    {
        const std::size_t skip  = config.steps / 10;
        const std::size_t count = e_total.size() > skip ? e_total.size() - skip : 0;
        if (count > 0) {
            double e_mean = 0.0;
            for (std::size_t i = skip; i < e_total.size(); ++i) {
                e_mean += e_total[i];
            }
            e_mean /= static_cast<double>(count);

            double max_dev = 0.0;
            for (std::size_t i = skip; i < e_total.size(); ++i) {
                max_dev = std::max(max_dev, std::abs(e_total[i] - e_mean));
            }
            if (std::abs(e_mean) > 1.0e-30) {
                relative_energy_drift = max_dev / std::abs(e_mean);
            }
        }
    }

    return LangmuirResult1D{omega_observed, relative_energy_drift, max_charge_err};
}

} // namespace pidc::pic
