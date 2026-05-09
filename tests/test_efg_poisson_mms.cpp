#include <Eigen/Dense>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
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
#include "test_utils.hpp"

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

double homogeneous_dirichlet(pidc::Vec2)
{
    return 0.0;
}

struct ErrorMetrics {
    double potential_l2{0.0};
    double potential_linf{0.0};
    double field_l2{0.0};
    double field_linf{0.0};
};

bool is_finite(ErrorMetrics metrics)
{
    return std::isfinite(metrics.potential_l2) &&
           std::isfinite(metrics.potential_linf) &&
           std::isfinite(metrics.field_l2) &&
           std::isfinite(metrics.field_linf);
}

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
            const pidc::ShapeFunctionData shape = pidc::mls_evaluate(quadrature.point, nodes, config);

            double numerical_potential = 0.0;
            pidc::Vec2 numerical_grad{0.0, 0.0};
            for (std::size_t i = 0; i < shape.neighbor_ids.size(); ++i) {
                const double value = nodal_values(static_cast<Eigen::Index>(shape.neighbor_ids[i]));
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
            const double field_error_norm = std::sqrt(
                field_error_x * field_error_x + field_error_y * field_error_y);
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

struct MmsResult {
    ErrorMetrics error{};
    double max_symmetry_error{0.0};
};

MmsResult run_mms(std::size_t node_count)
{
    using pidc::test::require;

    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
    const pidc::NodeCloud nodes = pidc::create_regular_node_cloud(domain, node_count, node_count);
    const std::size_t cell_count = node_count - 1;
    const std::vector<pidc::GaussCell2D> cells = pidc::gauss2x2_cells(domain, cell_count, cell_count);
    const double grid_spacing = 1.0 / static_cast<double>(node_count - 1);
    const pidc::MLSConfig mls{1.8 * grid_spacing};

    pidc::EFGPoissonSolver solver;
    solver.assemble(nodes, domain, cells, mls, source, homogeneous_dirichlet);
    const Eigen::VectorXd solution = solver.solve();
    const Eigen::MatrixXd k = solver.stiffness_matrix();

    require(k.allFinite(), "stiffness matrix must be finite");
    require(solver.rhs().allFinite(), "rhs must be finite");
    require(solution.allFinite(), "solution must be finite");

    double max_symmetry_error = 0.0;
    for (Eigen::Index row = 0; row < k.rows(); ++row) {
        for (Eigen::Index col = 0; col < k.cols(); ++col) {
            max_symmetry_error = std::max(max_symmetry_error, std::abs(k(row, col) - k(col, row)));
        }
    }
    require(max_symmetry_error < 1.0e-10, "stiffness matrix must remain symmetric after Dirichlet");

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        const pidc::Vec2 position = nodes[i].position;
        if (position.x == 0.0 || position.x == 1.0 || position.y == 0.0 || position.y == 1.0) {
            require(std::abs(solution(static_cast<Eigen::Index>(i))) < 1.0e-10,
                    "homogeneous Dirichlet boundary nodes must be approximately zero (penalty method)");
        }
    }

    return MmsResult{compute_error_metrics(cells, nodes, mls, solution), max_symmetry_error};
}

} // namespace

int main()
{
    pidc::EFGPoissonSolver unassembled;
    bool solve_before_assemble_threw = false;
    try {
        (void)unassembled.solve();
    } catch (const std::runtime_error&) {
        solve_before_assemble_threw = true;
    }
    pidc::test::require(solve_before_assemble_threw, "solve before assemble must throw");

    const MmsResult coarse = run_mms(5);
    const MmsResult refined = run_mms(9);

    pidc::test::require(is_finite(coarse.error), "5x5 MMS error metrics must be finite");
    pidc::test::require(is_finite(refined.error), "9x9 MMS error metrics must be finite");

    pidc::test::require(coarse.error.potential_l2 < 1.0e-2,
                        "5x5 MMS potential L2 error must be below tolerance");
    pidc::test::require(coarse.error.potential_linf < 1.0e-2,
                        "5x5 MMS potential Linf error must be below tolerance");
    pidc::test::require(coarse.error.field_l2 < 5.0e-2,
                        "5x5 MMS field L2 error must be below tolerance");
    pidc::test::require(coarse.error.field_linf < 1.2e-1,
                        "5x5 MMS field Linf error must be below tolerance");

    pidc::test::require(refined.error.potential_l2 < coarse.error.potential_l2,
                        "9x9 MMS potential L2 error must improve over 5x5");
    pidc::test::require(refined.error.potential_linf < coarse.error.potential_linf,
                        "9x9 MMS potential Linf error must improve over 5x5");
    pidc::test::require(refined.error.field_l2 < coarse.error.field_l2,
                        "9x9 MMS field L2 error must improve over 5x5");
    pidc::test::require(refined.error.field_linf < coarse.error.field_linf,
                        "9x9 MMS field Linf error must improve over 5x5");

    std::cout << "efg_poisson_mms test passed\n";
    std::cout.precision(17);
    std::cout << "potential L2 5x5: " << coarse.error.potential_l2 << '\n';
    std::cout << "potential L2 9x9: " << refined.error.potential_l2 << '\n';
    std::cout << "potential Linf 5x5: " << coarse.error.potential_linf << '\n';
    std::cout << "potential Linf 9x9: " << refined.error.potential_linf << '\n';
    std::cout << "field L2 5x5: " << coarse.error.field_l2 << '\n';
    std::cout << "field L2 9x9: " << refined.error.field_l2 << '\n';
    std::cout << "field Linf 5x5: " << coarse.error.field_linf << '\n';
    std::cout << "field Linf 9x9: " << refined.error.field_linf << '\n';
    return 0;
}
