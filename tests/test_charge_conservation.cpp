#include <Eigen/Dense>

#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/pidc/ChargeDeposition.hpp"
#include "test_utils.hpp"

namespace {

// 5x5 cloud on [0,1]^2 with support_radius = 1.8 * h
pidc::NodeCloud make_cloud_5x5()
{
    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
    return pidc::create_regular_node_cloud(domain, 5, 5);
}

constexpr double kH5 = 1.0 / 4.0;          // grid spacing for 5x5
constexpr double kRadius5 = 1.8 * kH5;     // = 0.45

// Verify: sum_i Q_i == sum_p q_p (multiple particles, two species)
void test_global_conservation_multi_particle()
{
    using pidc::test::require;

    const pidc::NodeCloud cloud = make_cloud_5x5();
    const pidc::MLSConfig mls{kRadius5};

    const std::vector<pidc::Species> species{
        pidc::Species{1.0,  1.0, "positive"},
        pidc::Species{-2.0, 1.0, "negative"},
    };

    // Asymmetric positions, alternating species — total charge: 1-2+1-2 = -2
    const std::vector<pidc::Particle> particles{
        pidc::Particle{{0.1, 0.2}, {}, 0},
        pidc::Particle{{0.5, 0.5}, {}, 1},
        pidc::Particle{{0.8, 0.9}, {}, 0},
        pidc::Particle{{0.3, 0.7}, {}, 1},
    };

    double expected = 0.0;
    for (const auto& p : particles) {
        expected += species[p.species_id].charge;
    }

    const Eigen::VectorXd Q = pidc::deposit_charge(cloud, particles, species, mls);

    require(Q.size() == static_cast<Eigen::Index>(cloud.size()),
            "nodal_charges size must equal node count");
    require(Q.allFinite(), "nodal_charges must all be finite");
    require(std::abs(Q.sum() - expected) < 1.0e-12,
            "multi-particle global charge conservation");
}

// Verify conservation for a single particle at an asymmetric interior point
void test_single_particle_conservation()
{
    using pidc::test::require;

    const pidc::NodeCloud cloud = make_cloud_5x5();
    const pidc::MLSConfig mls{kRadius5};

    const std::vector<pidc::Species> species{
        pidc::Species{3.0, 1.0, "test"},
    };
    const std::vector<pidc::Particle> particles{
        pidc::Particle{{0.37, 0.61}, {}, 0},
    };

    const Eigen::VectorXd Q = pidc::deposit_charge(cloud, particles, species, mls);
    require(std::abs(Q.sum() - 3.0) < 1.0e-12, "single particle charge conservation");
}

// Verify that zero particles returns a zero vector of correct size
void test_empty_particles_returns_zero()
{
    using pidc::test::require;

    const pidc::NodeCloud cloud = make_cloud_5x5();
    const pidc::MLSConfig mls{kRadius5};
    const std::vector<pidc::Species> species{pidc::Species{1.0, 1.0, "test"}};

    const Eigen::VectorXd Q = pidc::deposit_charge(cloud, {}, species, mls);
    require(Q.size() == 25, "empty-particle result must have one entry per node");
    require(Q.isZero(), "no particles => all nodal charges are zero");
}

// Verify that an out-of-range species_id throws
void test_invalid_species_throws()
{
    using pidc::test::require;

    const pidc::NodeCloud cloud = make_cloud_5x5();
    const pidc::MLSConfig mls{kRadius5};
    const std::vector<pidc::Species> species{pidc::Species{1.0, 1.0, "only"}};

    const std::vector<pidc::Particle> particles{
        pidc::Particle{{0.5, 0.5}, {}, 99},  // species_id out of range
    };

    bool threw = false;
    try {
        (void)pidc::deposit_charge(cloud, particles, species, mls);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    require(threw, "invalid species_id must throw std::out_of_range");
}

// Conservation test with a 9x9 cloud to confirm independence of grid resolution
void test_conservation_9x9()
{
    using pidc::test::require;

    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
    const pidc::NodeCloud cloud = pidc::create_regular_node_cloud(domain, 9, 9);
    const double h = 1.0 / 8.0;
    const pidc::MLSConfig mls{1.8 * h};

    const std::vector<pidc::Species> species{
        pidc::Species{5.0, 1.0, "a"},
        pidc::Species{-3.0, 1.0, "b"},
    };
    const std::vector<pidc::Particle> particles{
        pidc::Particle{{0.15, 0.73}, {}, 0},
        pidc::Particle{{0.62, 0.41}, {}, 1},
        pidc::Particle{{0.33, 0.88}, {}, 0},
    };

    double expected = 5.0 - 3.0 + 5.0;  // 7.0

    const Eigen::VectorXd Q = pidc::deposit_charge(cloud, particles, species, mls);
    require(std::abs(Q.sum() - expected) < 1.0e-12,
            "9x9 global charge conservation");
}

} // namespace

int main()
{
    test_global_conservation_multi_particle();
    test_single_particle_conservation();
    test_empty_particles_returns_zero();
    test_invalid_species_throws();
    test_conservation_9x9();

    std::cout << "charge_conservation test passed\n";
    return 0;
}
