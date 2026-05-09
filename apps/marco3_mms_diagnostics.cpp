// marco3_mms_diagnostics.cpp — Diagnóstico de convergência MMS EFG Poisson 2D.
//
// Executa o MMS com n ∈ {5, 7, 9, 13, 17}, exporta CSV de convergência e
// verifica critério de aceite do Marco 3 (ordem de φ > 1.7 no par 13→17).
//
// Deve ser executado da raiz do repositório:
//   ./build/pidc_marco3_mms_diagnostics
// Saída: data/output/efg_mms_convergence.csv
//
// NOTA: exact_solution, source, exact_field e compute_error_metrics são cópias
// de tests/test_efg_poisson_mms.cpp — restrição "sem headers novos" do projeto.

#include <Eigen/Dense>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/ShapeFunctionData.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/efg/EFGPoissonSolver.hpp"
#include "pidc/efg/GaussCell2D.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/mls/MLSShapeFunction.hpp"

namespace {

constexpr double kPi = 3.141592653589793238462643383279502884;

double exact_solution(pidc::Vec2 point)
{
    return std::sin(kPi * point.x) * std::sin(kPi * point.y);
}

double source(pidc::Vec2 point)
{
    return 2.0 * kPi * kPi * exact_solution(point);
}

pidc::Vec2 exact_field(pidc::Vec2 point)
{
    return {
        -kPi * std::cos(kPi * point.x) * std::sin(kPi * point.y),
        -kPi * std::sin(kPi * point.x) * std::cos(kPi * point.y),
    };
}

double homogeneous_dirichlet(pidc::Vec2) { return 0.0; }

struct ErrorMetrics {
    double potential_l2{0.0};
    double potential_linf{0.0};
    double field_l2{0.0};
    double field_linf{0.0};
};

ErrorMetrics compute_error_metrics(
    const std::vector<pidc::GaussCell2D>& cells,
    const pidc::NodeCloud& nodes,
    const pidc::MLSConfig& config,
    const Eigen::VectorXd& nodal_values)
{
    double potential_error_squared = 0.0;
    double field_error_squared = 0.0;
    double potential_linf = 0.0;
    double field_linf = 0.0;

    for (const pidc::GaussCell2D& cell : cells) {
        for (const pidc::GaussPoint2D& quadrature : cell.points) {
            const pidc::ShapeFunctionData shape =
                pidc::mls_evaluate(quadrature.point, nodes, config);

            double numerical_potential = 0.0;
            pidc::Vec2 numerical_grad{0.0, 0.0};
            for (std::size_t i = 0; i < shape.neighbor_ids.size(); ++i) {
                const double value =
                    nodal_values(static_cast<Eigen::Index>(shape.neighbor_ids[i]));
                numerical_potential += shape.phi[i] * value;
                numerical_grad.x += shape.grad_phi[i].x * value;
                numerical_grad.y += shape.grad_phi[i].y * value;
            }

            const double potential_error = numerical_potential - exact_solution(quadrature.point);
            potential_error_squared += quadrature.weight * potential_error * potential_error;
            potential_linf = std::max(potential_linf, std::abs(potential_error));

            const pidc::Vec2 numerical_field{-numerical_grad.x, -numerical_grad.y};
            const pidc::Vec2 analytical_field = exact_field(quadrature.point);
            const double field_error_x = numerical_field.x - analytical_field.x;
            const double field_error_y = numerical_field.y - analytical_field.y;
            const double field_error_norm =
                std::sqrt(field_error_x * field_error_x + field_error_y * field_error_y);
            field_error_squared += quadrature.weight * field_error_norm * field_error_norm;
            field_linf = std::max(field_linf, field_error_norm);
        }
    }

    return {
        std::sqrt(potential_error_squared),
        potential_linf,
        std::sqrt(field_error_squared),
        field_linf,
    };
}

struct MmsRow {
    std::size_t n;
    double h;
    double pot_l2;
    double pot_linf;
    double fld_l2;
    double fld_linf;
    double order_pot_l2{0.0};
    double order_fld_l2{0.0};
};

std::vector<MmsRow> run_convergence_study()
{
    const std::vector<std::size_t> resolutions = {5, 7, 9, 13, 17};
    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};

    std::vector<MmsRow> rows;
    rows.reserve(resolutions.size());

    for (const std::size_t node_count : resolutions) {
        const std::size_t cell_count = node_count - 1;
        const double grid_spacing = 1.0 / static_cast<double>(node_count - 1);
        const pidc::MLSConfig mls{1.8 * grid_spacing};

        const pidc::NodeCloud nodes =
            pidc::create_regular_node_cloud(domain, node_count, node_count);
        const std::vector<pidc::GaussCell2D> cells =
            pidc::gauss2x2_cells(domain, cell_count, cell_count);

        pidc::EFGPoissonSolver solver;
        solver.assemble(nodes, domain, cells, mls, source, homogeneous_dirichlet);
        const Eigen::VectorXd solution = solver.solve();

        const ErrorMetrics err = compute_error_metrics(cells, nodes, mls, solution);

        MmsRow row;
        row.n = node_count;
        row.h = grid_spacing;
        row.pot_l2 = err.potential_l2;
        row.pot_linf = err.potential_linf;
        row.fld_l2 = err.field_l2;
        row.fld_linf = err.field_linf;

        if (!rows.empty()) {
            const MmsRow& prev = rows.back();
            const double log_h_ratio = std::log(prev.h / row.h);
            row.order_pot_l2 = std::log(prev.pot_l2 / row.pot_l2) / log_h_ratio;
            row.order_fld_l2 = std::log(prev.fld_l2 / row.fld_l2) / log_h_ratio;
        }

        rows.push_back(row);
    }
    return rows;
}

void export_csv(const std::string& path, const std::vector<MmsRow>& rows)
{
    std::ofstream f(path);
    f << "n,h,pot_l2,pot_linf,fld_l2,fld_linf,order_pot_l2,order_fld_l2\n";
    f << std::scientific << std::setprecision(17);
    for (const auto& r : rows) {
        f << r.n << ','
          << r.h << ','
          << r.pot_l2 << ','
          << r.pot_linf << ','
          << r.fld_l2 << ','
          << r.fld_linf << ','
          << r.order_pot_l2 << ','
          << r.order_fld_l2 << '\n';
    }
}

} // namespace

int main()
{
    std::cout << "=== Marco 3 — EFG Poisson MMS Diagnósticos ===\n"
              << "Data: 2026-05-09\n\n";

    const auto rows = run_convergence_study();

    // --- Tabela de console ---
    std::cout << std::setw(4)  << "n"
              << std::setw(10) << "h"
              << std::setw(13) << "L2(phi)"
              << std::setw(13) << "Linf(phi)"
              << std::setw(13) << "L2(E)"
              << std::setw(13) << "Linf(E)"
              << std::setw(10) << "ord(phi)"
              << std::setw(10) << "ord(E)"
              << '\n';
    std::cout << std::string(86, '-') << '\n';

    for (const auto& r : rows) {
        std::cout << std::setw(4)  << r.n
                  << std::setw(10) << std::fixed << std::setprecision(4) << r.h
                  << std::setw(13) << std::scientific << std::setprecision(3) << r.pot_l2
                  << std::setw(13) << r.pot_linf
                  << std::setw(13) << r.fld_l2
                  << std::setw(13) << r.fld_linf;
        if (r.order_pot_l2 > 0.0) {
            std::cout << std::fixed << std::setprecision(2)
                      << std::setw(10) << r.order_pot_l2
                      << std::setw(10) << r.order_fld_l2;
        } else {
            std::cout << std::setw(10) << "-" << std::setw(10) << "-";
        }
        std::cout << '\n';
    }

    // --- Critérios de aceite ---
    bool pot_monotone = true;
    for (std::size_t i = 1; i < rows.size(); ++i) {
        if (rows[i].pot_l2 >= rows[i - 1].pot_l2) pot_monotone = false;
        if (rows[i].fld_l2 >= rows[i - 1].fld_l2) pot_monotone = false;
    }
    const bool order_ok = rows.back().order_pot_l2 > 1.7;
    const bool all_ok = pot_monotone && order_ok;

    std::cout << '\n';
    std::cout << (pot_monotone ? "✓" : "✗")
              << " Erros monotonicamente decrescentes com refinamento\n";
    std::cout << (order_ok ? "✓" : "✗")
              << " Ordem de convergência de φ no par 13→17: "
              << std::fixed << std::setprecision(2) << rows.back().order_pot_l2
              << " (critério: > 1.7)\n";

    // --- CSV export ---
    std::filesystem::create_directories("data/output");
    const std::string csv_path = "data/output/efg_mms_convergence.csv";
    export_csv(csv_path, rows);
    std::cout << "\nExportado: " << csv_path << '\n';

    return all_ok ? 0 : 1;
}
