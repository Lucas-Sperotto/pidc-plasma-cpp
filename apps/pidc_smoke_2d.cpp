#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/efg/EFGPoissonSolver.hpp"
#include "pidc/efg/GaussCell2D.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/pidc/PIDCLoop.hpp"

namespace {

double zero_dirichlet(pidc::Vec2)
{
    return 0.0;
}

} // namespace

int main()
{
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

    std::filesystem::create_directories("data/output");
    std::ofstream csv{"data/output/pidc_smoke_2d.csv"};
    csv << "step,particle_count,total_charge,max_field_magnitude,finite\n";

    constexpr double epsilon0 = 1.0;
    constexpr double dt = 1.0e-3;
    constexpr int steps = 3;

    for (int step = 0; step < steps; ++step) {
        const pidc::PIDCStepDiagnostics diagnostics =
            pidc::pidc_advance_one_step(particles, species, nodes, solver, mls, epsilon0, dt);

        csv << step << ','
            << diagnostics.particle_count << ','
            << diagnostics.total_charge << ','
            << diagnostics.max_field_magnitude << ','
            << (diagnostics.all_finite ? 1 : 0) << '\n';

        if (!diagnostics.all_finite || diagnostics.particle_count != particles.size()) {
            std::cerr << "PIDC smoke diagnostics failed\n";
            return 1;
        }

        for (const pidc::Particle& particle : particles) {
            if (!domain.contains(particle.position)) {
                std::cerr << "PIDC smoke particle left the Dirichlet smoke domain\n";
                return 1;
            }
        }
    }

    std::cout << "pidc_smoke_2d wrote data/output/pidc_smoke_2d.csv\n";
    return 0;
}
