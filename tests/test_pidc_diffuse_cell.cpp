#include <Eigen/Dense>

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/pidc/ChargeDeposition.hpp"
#include "pidc/pidc/DiffuseCell.hpp"
#include "test_utils.hpp"

namespace {

pidc::NodeCloud make_cloud()
{
    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
    return pidc::create_regular_node_cloud(domain, 5, 5);
}

} // namespace

int main()
{
    using pidc::test::require;

    const pidc::NodeCloud cloud = make_cloud();
    const pidc::MLSConfig mls{1.8 / 4.0};
    const std::vector<pidc::Species> species{
        pidc::Species{2.0, 1.0, "positive"},
        pidc::Species{-3.0, 1.0, "negative"},
    };
    const std::vector<pidc::Particle> particles{
        pidc::Particle{{0.25, 0.25}, {}, 0},
        pidc::Particle{{0.50, 0.50}, {}, 1},
        pidc::Particle{{0.70, 0.30}, {}, 0},
    };

    const std::vector<pidc::DiffuseCell> cells =
        pidc::evaluate_diffuse_cells(cloud, particles, mls);
    require(cells.size() == particles.size(), "one diffuse cell must be evaluated per particle");
    for (std::size_t i = 0; i < cells.size(); ++i) {
        require(cells[i].particle_index == i, "diffuse cells must preserve particle indices");
        require(pidc::is_valid(cells[i].shape), "diffuse cell shape data must be valid");
        double sum_phi = 0.0;
        for (const double phi : cells[i].shape.phi) {
            sum_phi += phi;
        }
        require(std::abs(sum_phi - 1.0) < 1.0e-12,
                "diffuse cell must preserve MLS partition of unity");
    }

    const Eigen::VectorXd cached =
        pidc::deposit_charge_from_cells(cloud, particles, species, cells);
    const Eigen::VectorXd direct =
        pidc::deposit_charge(cloud, particles, species, mls);

    require((cached - direct).norm() < 1.0e-12,
            "cached diffuse-cell deposition must match direct deposition");
    require(std::abs(cached.sum() - 1.0) < 1.0e-12,
            "cached diffuse-cell deposition must conserve total charge");

    bool wrong_cell_count_threw = false;
    try {
        std::vector<pidc::DiffuseCell> missing = cells;
        missing.pop_back();
        (void)pidc::deposit_charge_from_cells(cloud, particles, species, missing);
    } catch (const std::invalid_argument&) {
        wrong_cell_count_threw = true;
    }
    require(wrong_cell_count_threw, "missing diffuse cells must throw invalid_argument");

    std::cout << "pidc_diffuse_cell test passed\n";
    return 0;
}
