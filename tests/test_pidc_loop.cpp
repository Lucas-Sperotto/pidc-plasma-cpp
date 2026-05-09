#include <Eigen/Dense>

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/efg/EFGPoissonSolver.hpp"
#include "pidc/efg/GaussCell2D.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/pidc/PIDCLoop.hpp"
#include "test_utils.hpp"

namespace {

double zero_dirichlet(pidc::Vec2)
{
    return 0.0;
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
    solver.assemble_stiffness_only(nodes, domain, cells, mls, zero_dirichlet);

    std::vector<pidc::Species> species{
        pidc::Species{1.0e-6, 1.0, "positive"},
        pidc::Species{-1.0e-6, 1.0, "negative"},
    };
    std::vector<pidc::Particle> particles{
        pidc::Particle{{0.35, 0.35}, {0.0, 0.0}, 0},
        pidc::Particle{{0.65, 0.35}, {0.0, 0.0}, 1},
        pidc::Particle{{0.35, 0.65}, {0.0, 0.0}, 1},
        pidc::Particle{{0.65, 0.65}, {0.0, 0.0}, 0},
    };

    const std::size_t initial_count = particles.size();
    for (int step = 0; step < 2; ++step) {
        const pidc::PIDCStepDiagnostics diagnostics =
            pidc::pidc_advance_one_step(particles, species, nodes, solver, mls, 1.0, 1.0e-3);

        require(diagnostics.all_finite, "PIDC loop diagnostics must stay finite");
        require(diagnostics.particle_count == initial_count,
                "PIDC loop must conserve particle count in the Dirichlet smoke case");
        require(std::abs(diagnostics.total_charge) < 1.0e-18,
                "PIDC loop smoke case must preserve neutral total charge");
        require(std::isfinite(diagnostics.max_field_magnitude),
                "PIDC loop max field must be finite");
        for (const pidc::Particle& particle : particles) {
            require(domain.contains(particle.position),
                    "PIDC loop smoke particles must remain inside the Dirichlet domain");
        }
    }

    bool bad_epsilon_threw = false;
    try {
        (void)pidc::pidc_rhs_from_charge(Eigen::VectorXd::Zero(3), 0.0);
    } catch (const std::invalid_argument&) {
        bad_epsilon_threw = true;
    }
    require(bad_epsilon_threw, "pidc_rhs_from_charge must reject epsilon0 <= 0");

    Eigen::VectorXd bad_charge = Eigen::VectorXd::Zero(3);
    bad_charge(1) = std::numeric_limits<double>::quiet_NaN();
    bool bad_charge_threw = false;
    try {
        (void)pidc::pidc_rhs_from_charge(bad_charge, 1.0);
    } catch (const std::invalid_argument&) {
        bad_charge_threw = true;
    }
    require(bad_charge_threw, "pidc_rhs_from_charge must reject non-finite charges");

    std::cout << "pidc_loop test passed\n";
    return 0;
}
