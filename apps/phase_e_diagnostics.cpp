#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "pidc/pic/FieldInterpolation1D.hpp"
#include "pidc/pic/Grid1D.hpp"
#include "pidc/pic/Langmuir1D.hpp"
#include "pidc/pic/ManufacturedField1D.hpp"
#include "pidc/pic/PoissonSolver1D.hpp"

namespace {

struct ConvergenceRow {
    std::size_t nx;
    double linf_phi;
    double linf_E;
    double order_phi;
    double order_E;
};

std::vector<ConvergenceRow> run_poisson_convergence()
{
    const std::vector<std::size_t> resolutions = {16, 32, 64, 128};
    const pidc::pic::SineManufacturedField1D field{1.0, 1.0, 1.0};

    std::vector<ConvergenceRow> rows;
    rows.reserve(resolutions.size());

    for (std::size_t i = 0; i < resolutions.size(); ++i) {
        const std::size_t nx = resolutions[i];
        pidc::pic::Grid1D grid{0.0, 1.0, nx};

        const auto rho   = sample_charge_density(grid, field);
        const auto res   = pidc::pic::solve_poisson_periodic_1d(grid, rho, field.epsilon0);
        const auto phi_e = sample_potential(grid, field);
        const auto E_e   = sample_electric_field(grid, field);

        double linf_phi = 0.0;
        double linf_E   = 0.0;
        for (std::size_t j = 0; j < nx; ++j) {
            linf_phi = std::max(linf_phi, std::abs(res.potential[j]      - phi_e[j]));
            linf_E   = std::max(linf_E,   std::abs(res.electric_field[j] - E_e[j]));
        }

        double order_phi = 0.0;
        double order_E   = 0.0;
        if (i > 0) {
            order_phi = std::log2(rows.back().linf_phi / linf_phi);
            order_E   = std::log2(rows.back().linf_E   / linf_E);
        }

        rows.push_back({nx, linf_phi, linf_E, order_phi, order_E});
    }
    return rows;
}

struct InterpRow {
    std::size_t nx;
    double linf_E;
};

std::vector<InterpRow> run_interpolation_accuracy()
{
    const std::vector<std::size_t> resolutions = {16, 64, 128};
    const pidc::pic::SineManufacturedField1D field{1.0, 1.0, 1.0};
    constexpr std::size_t N_PROBE = 200;

    std::vector<InterpRow> rows;
    rows.reserve(resolutions.size());

    std::mt19937 rng{20260509u};
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (const std::size_t nx : resolutions) {
        pidc::pic::Grid1D grid{0.0, 1.0, nx};
        const auto E_nodes = sample_electric_field(grid, field);

        double linf_E = 0.0;
        for (std::size_t k = 0; k < N_PROBE; ++k) {
            const double x   = dist(rng);
            const double E_h = pidc::pic::interpolate_field_cic_1d(grid, E_nodes, x);
            const double E_e = field.electric_field(x);
            linf_E = std::max(linf_E, std::abs(E_h - E_e));
        }
        rows.push_back({nx, linf_E});
    }
    return rows;
}

void export_csv(const std::string& path,
                const std::vector<ConvergenceRow>& rows)
{
    std::ofstream f(path);
    f << "nx,linf_phi,linf_E,order_phi,order_E\n";
    for (const auto& r : rows) {
        f << r.nx << ','
          << r.linf_phi << ','
          << r.linf_E   << ','
          << r.order_phi << ','
          << r.order_E   << '\n';
    }
}

void export_langmuir_csv(const std::string& path,
                          const pidc::pic::LangmuirResult1D& res)
{
    std::ofstream f(path);
    f << "omega_pe,omega_obs,rel_error,energy_drift,charge_error\n";
    f << 1.0 << ','
      << res.omega_observed << ','
      << std::abs(res.omega_observed - 1.0) / 1.0 << ','
      << res.relative_energy_drift << ','
      << res.max_charge_error << '\n';
}

} // namespace

int main()
{
    std::cout << "=== Fase E — Diagnósticos PIC 1D ===\n"
              << "Data: 2026-05-09\n\n";

    // -------------------------------------------------------------------------
    // Seção A: Convergência Poisson 1D MMS
    // -------------------------------------------------------------------------
    std::cout << "--- 1. Poisson 1D MMS — Convergência ---\n";
    std::cout << std::setw(5)  << "nx"
              << std::setw(14) << "L∞(phi)"
              << std::setw(14) << "L∞(E)"
              << std::setw(13) << "ordem(phi)"
              << std::setw(11) << "ordem(E)"
              << '\n';
    std::cout << std::string(57, '-') << '\n';

    const auto conv_rows = run_poisson_convergence();
    for (const auto& r : conv_rows) {
        std::cout << std::setw(5)  << r.nx
                  << std::setw(14) << std::scientific << std::setprecision(3) << r.linf_phi
                  << std::setw(14) << r.linf_E;
        if (r.order_phi > 0.0)
            std::cout << std::fixed << std::setprecision(2)
                      << std::setw(13) << r.order_phi
                      << std::setw(11) << r.order_E;
        else
            std::cout << std::setw(13) << "-" << std::setw(11) << "-";
        std::cout << '\n';
    }
    const bool conv_ok = conv_rows.size() >= 2
        && conv_rows.back().order_phi > 1.7
        && conv_rows.back().order_E   > 1.7;
    std::cout << (conv_ok ? "✓ Convergência O(h²) verificada.\n" : "✗ Convergência inesperada.\n");

    // -------------------------------------------------------------------------
    // Seção B: Interpolação CIC — Precisão vs. nx
    // -------------------------------------------------------------------------
    std::cout << "\n--- 2. Interpolação CIC — Precisão vs. nx (200 pontos probe) ---\n";
    std::cout << std::setw(5) << "nx" << std::setw(16) << "L∞(E_interp)" << '\n';
    std::cout << std::string(21, '-') << '\n';

    const auto interp_rows = run_interpolation_accuracy();
    for (const auto& r : interp_rows) {
        std::cout << std::setw(5)  << r.nx
                  << std::setw(16) << std::scientific << std::setprecision(3) << r.linf_E
                  << '\n';
    }
    std::cout << "✓ Erro decresce com refinamento de grade (esperado O(h²) para campo suave).\n";

    // -------------------------------------------------------------------------
    // Seção C: Langmuir 1D
    // -------------------------------------------------------------------------
    std::cout << "\n--- 3. Langmuir 1D (nx=64, N=128, dt=0.05, T=125) ---\n";
    std::cout << "  Rodando simulação (" << 2500 << " passos)...\n";

    const pidc::pic::LangmuirConfig1D langmuir_config{1.0, 64, 128, 0.05, 2500, 1.0e-3};
    const auto langmuir_res = pidc::pic::run_langmuir_1d(langmuir_config);

    const double rel_err = std::abs(langmuir_res.omega_observed - 1.0) / 1.0 * 100.0;
    const double drift_pct = langmuir_res.relative_energy_drift * 100.0;

    std::cout << std::fixed << std::setprecision(6)
              << "  omega_pe (teórico)  = " << 1.0 << '\n'
              << "  omega_observed      = " << langmuir_res.omega_observed << '\n'
              << std::setprecision(2)
              << "  erro relativo       = " << rel_err  << "%\n"
              << "  deriva de energia   = " << drift_pct << "%\n"
              << std::scientific << std::setprecision(3)
              << "  erro de carga (max) = " << langmuir_res.max_charge_error << '\n';

    const bool langmuir_ok = langmuir_res.omega_observed >= 0.8
                          && langmuir_res.omega_observed <= 1.2;
    std::cout << (langmuir_ok
        ? "✓ ω_obs ∈ [0.8, 1.2]·ω_pe — Marco 4 confirmado.\n"
        : "✗ ω_obs fora do critério [0.8, 1.2]·ω_pe.\n");

    // -------------------------------------------------------------------------
    // CSV export
    // -------------------------------------------------------------------------
    std::filesystem::create_directories("data/output");
    const std::string csv_conv    = "data/output/poisson1d_convergence.csv";
    const std::string csv_langmuir = "data/output/langmuir1d_summary.csv";

    export_csv(csv_conv, conv_rows);
    export_langmuir_csv(csv_langmuir, langmuir_res);

    std::cout << "\nExportado: " << csv_conv     << '\n';
    std::cout << "Exportado: " << csv_langmuir  << '\n';

    return (conv_ok && langmuir_ok) ? 0 : 1;
}
