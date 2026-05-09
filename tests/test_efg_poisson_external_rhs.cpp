#include <Eigen/Dense>

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/efg/EFGPoissonSolver.hpp"
#include "pidc/efg/GaussCell2D.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "test_utils.hpp"

namespace {

double constant_dirichlet(pidc::Vec2)
{
    return 2.0;
}

bool is_boundary(pidc::Vec2 point)
{
    return point.x == 0.0 || point.x == 1.0 || point.y == 0.0 || point.y == 1.0;
}

} // namespace

int main()
{
    using pidc::test::require;

    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
    const std::size_t node_count = 5;
    const pidc::NodeCloud nodes = pidc::create_regular_node_cloud(domain, node_count, node_count);
    const std::vector<pidc::GaussCell2D> cells =
        pidc::gauss2x2_cells(domain, node_count - 1, node_count - 1);
    const pidc::MLSConfig mls{1.8 / static_cast<double>(node_count - 1)};

    pidc::EFGPoissonSolver solver;
    solver.assemble_stiffness_only(nodes, domain, cells, mls, constant_dirichlet);

    const Eigen::VectorXd zero_rhs = Eigen::VectorXd::Zero(static_cast<Eigen::Index>(nodes.size()));
    const Eigen::VectorXd constant_solution = solver.solve(zero_rhs);
    require(constant_solution.allFinite(), "external-rhs solve must be finite");

    for (std::size_t i = 0; i < nodes.size(); ++i) {
        if (is_boundary(nodes[i].position)) {
            require(std::abs(constant_solution(static_cast<Eigen::Index>(i)) - 2.0) < 1.0e-9,
                    "external-rhs solve must preserve non-homogeneous Dirichlet penalty rhs");
        }
    }

    Eigen::VectorXd impulse_rhs = zero_rhs;
    impulse_rhs(12) = 1.0;
    const Eigen::VectorXd impulse_solution = solver.solve(impulse_rhs);
    require((impulse_solution - constant_solution).norm() > 1.0e-6,
            "different external rhs values must change the solution without refactorizing");

    bool wrong_size_threw = false;
    try {
        (void)solver.solve(Eigen::VectorXd::Zero(3));
    } catch (const std::invalid_argument&) {
        wrong_size_threw = true;
    }
    require(wrong_size_threw, "wrong-size external rhs must throw invalid_argument");

    Eigen::VectorXd bad_rhs = zero_rhs;
    bad_rhs(0) = std::numeric_limits<double>::quiet_NaN();
    bool nonfinite_threw = false;
    try {
        (void)solver.solve(bad_rhs);
    } catch (const std::invalid_argument&) {
        nonfinite_threw = true;
    }
    require(nonfinite_threw, "non-finite external rhs must throw invalid_argument");

    std::cout << "efg_poisson_external_rhs test passed\n";
    return 0;
}
