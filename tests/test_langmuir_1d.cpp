#include <cmath>
#include <iostream>

#include "pidc/pic/Langmuir1D.hpp"
#include "test_utils.hpp"

int main()
{
    // Parameters from PHASE_E_PIC1D_TECH_PLAN.md T-044.
    // Normalized units: epsilon0=1, omega_pe=1.
    const pidc::pic::LangmuirConfig1D config{
        1.0,     // length
        64,      // nx
        128,     // particles
        0.05,    // dt
        2500,    // steps  (T_sim = 125, ~19.9 Langmuir cycles)
        1.0e-3   // perturbation
    };

    std::cout << "Running Langmuir 1D simulation (" << config.steps
              << " steps)...\n";

    const auto result = pidc::pic::run_langmuir_1d(config);

    std::cout << "  omega_observed     = " << result.omega_observed     << '\n'
              << "  energy_drift       = " << result.relative_energy_drift << '\n'
              << "  max_charge_error   = " << result.max_charge_error    << '\n';

    // T1: observed plasma frequency within 20% of omega_pe = 1
    pidc::test::require(
        result.omega_observed >= 0.8,
        "T1: omega_observed must be >= 0.8 * omega_pe");
    pidc::test::require(
        result.omega_observed <= 1.2,
        "T1: omega_observed must be <= 1.2 * omega_pe");

    // T2: total energy conservation (max relative deviation from mean <= 25%)
    pidc::test::require(
        result.relative_energy_drift <= 0.25,
        "T2: relative_energy_drift must be <= 0.25");

    // T3: CIC charge conservation to machine precision
    pidc::test::require(
        result.max_charge_error <= 1.0e-10,
        "T3: max_charge_error must be <= 1e-10");

    // T4: all diagnostic values finite
    pidc::test::require(
        std::isfinite(result.omega_observed),
        "T4: omega_observed must be finite");
    pidc::test::require(
        std::isfinite(result.relative_energy_drift),
        "T4: relative_energy_drift must be finite");
    pidc::test::require(
        std::isfinite(result.max_charge_error),
        "T4: max_charge_error must be finite");

    std::cout << "langmuir_1d test passed\n";
    return 0;
}
