#include <Eigen/Dense>

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

double homogeneous_dirichlet(pidc::Vec2)
{
    return 0.0;
}

double evaluate_solution(
    pidc::Vec2 point,
    const pidc::NodeCloud& nodes,
    const pidc::MLSConfig& config,
    const Eigen::VectorXd& nodal_values)
{
    const pidc::ShapeFunctionData shape = pidc::mls_evaluate(point, nodes, config);

    double value = 0.0;
    for (std::size_t i = 0; i < shape.neighbor_ids.size(); ++i) {
        value += shape.phi[i] * nodal_values(static_cast<Eigen::Index>(shape.neighbor_ids[i]));
    }
    return value;
}

double l2_error(
    const std::vector<pidc::GaussCell2D>& cells,
    const pidc::NodeCloud& nodes,
    const pidc::MLSConfig& config,
    const Eigen::VectorXd& nodal_values)
{
    double error_squared = 0.0;
    for (const pidc::GaussCell2D& cell : cells) {
        for (const pidc::GaussPoint2D& point : cell.points) {
            const double error = evaluate_solution(point.point, nodes, config, nodal_values) -
                                 exact_solution(point.point);
            error_squared += point.weight * error * error;
        }
    }

    return std::sqrt(error_squared);
}

struct MmsResult {
    double l2{0.0};
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

    require(solver.stiffness_matrix().allFinite(), "stiffness matrix must be finite");
    require(solver.rhs().allFinite(), "rhs must be finite");
    require(solution.allFinite(), "solution must be finite");

    double max_symmetry_error = 0.0;
    const Eigen::MatrixXd& k = solver.stiffness_matrix();
    for (Eigen::Index row = 0; row < k.rows(); ++row) {
        for (Eigen::Index col = 0; col < k.cols(); ++col) {
            max_symmetry_error = std::max(max_symmetry_error, std::abs(k(row, col) - k(col, row)));
        }
    }
    require(max_symmetry_error < 1.0e-10, "stiffness matrix must remain symmetric after Dirichlet");

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        const pidc::Vec2 position = nodes[i].position;
        if (position.x == 0.0 || position.x == 1.0 || position.y == 0.0 || position.y == 1.0) {
            require(std::abs(solution(static_cast<Eigen::Index>(i))) < 1.0e-12,
                    "homogeneous Dirichlet boundary nodes must be zero");
        }
    }

    return MmsResult{l2_error(cells, nodes, mls, solution), max_symmetry_error};
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

    pidc::test::require(coarse.l2 < 1.0e-2, "5x5 MMS L2 error must be below tolerance");
    pidc::test::require(refined.l2 < coarse.l2, "9x9 MMS L2 error must improve over 5x5");

    std::cout << "efg_poisson_mms test passed\n";
    std::cout << "L2 5x5: " << coarse.l2 << '\n';
    std::cout << "L2 9x9: " << refined.l2 << '\n';
    return 0;
}
