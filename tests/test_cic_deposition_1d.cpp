#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/pic/ChargeDeposition1D.hpp"
#include "pidc/pic/Grid1D.hpp"
#include "test_utils.hpp"

// Grid: [0, 1), nx=4, dx=0.25. Nodes at x = 0.0, 0.25, 0.50, 0.75.

static pidc::Particle make_particle(double x, std::size_t species_id = 0)
{
    pidc::Particle p;
    p.position.x = x;
    p.species_id = species_id;
    return p;
}

int main()
{
    const pidc::pic::Grid1D grid{0.0, 1.0, 4};
    const pidc::Species electron{-1.0, 1.0, "e"};
    const std::vector<pidc::Species> species{electron};

    // Test 1: particle at cell midpoint → 50% on each neighbor
    {
        // Midpoint of cell 0: x = 0.125, left=0, right=1, f=0.5
        const std::vector<pidc::Particle> pts{make_particle(0.125)};
        const auto Q = pidc::pic::deposit_charge_cic_1d(grid, pts, species);

        pidc::test::require(Q.size() == 4, "T1: output size must equal nx");
        pidc::test::require(
            pidc::test::approx_equal(Q[0], -0.5, 1e-12),
            "T1: left node should receive 50% of charge");
        pidc::test::require(
            pidc::test::approx_equal(Q[1], -0.5, 1e-12),
            "T1: right node should receive 50% of charge");
        pidc::test::require(
            pidc::test::approx_equal(Q[2], 0.0, 1e-12) &&
            pidc::test::approx_equal(Q[3], 0.0, 1e-12),
            "T1: far nodes should be zero");
    }

    // Test 2: particle exactly on node i (f=0) → 100% on that node
    {
        // x = 0.25 → node 1 exactly, f = 0
        const std::vector<pidc::Particle> pts{make_particle(0.25)};
        const auto Q = pidc::pic::deposit_charge_cic_1d(grid, pts, species);

        pidc::test::require(
            pidc::test::approx_equal(Q[1], -1.0, 1e-12),
            "T2: charge should be 100% on node 1");
        pidc::test::require(
            pidc::test::approx_equal(Q[0], 0.0, 1e-12) &&
            pidc::test::approx_equal(Q[2], 0.0, 1e-12) &&
            pidc::test::approx_equal(Q[3], 0.0, 1e-12),
            "T2: all other nodes should be zero");
    }

    // Test 3: particle near xmax → periodic: nodes nx-1 and 0
    {
        // x = 0.875 (midpoint of last cell [0.75, 1.0)), f=0.5, left=3, right=0
        const std::vector<pidc::Particle> pts{make_particle(0.875)};
        const auto Q = pidc::pic::deposit_charge_cic_1d(grid, pts, species);

        pidc::test::require(
            pidc::test::approx_equal(Q[3], -0.5, 1e-12),
            "T3: left periodic node (nx-1) should receive 50%");
        pidc::test::require(
            pidc::test::approx_equal(Q[0], -0.5, 1e-12),
            "T3: right periodic node (0) should receive 50%");
        pidc::test::require(
            pidc::test::approx_equal(Q[1], 0.0, 1e-12) &&
            pidc::test::approx_equal(Q[2], 0.0, 1e-12),
            "T3: interior nodes should be zero");
    }

    // Test 4: global conservation |sum(Q_i) - sum(q_p)| < 1e-12
    {
        // 10 particles at varied positions, mixed charges
        const pidc::Species ion{+2.0, 1.0, "ion"};
        const std::vector<pidc::Species> mixed_species{electron, ion};

        std::vector<pidc::Particle> pts;
        double expected_total = 0.0;

        const double positions[10] = {0.05, 0.13, 0.27, 0.38, 0.41,
                                       0.56, 0.62, 0.74, 0.82, 0.93};
        for (int i = 0; i < 10; ++i) {
            pidc::Particle p;
            p.position.x = positions[i];
            p.species_id = static_cast<std::size_t>(i % 2); // alternates e and ion
            pts.push_back(p);
            expected_total += mixed_species[p.species_id].charge;
        }

        const auto Q = pidc::pic::deposit_charge_cic_1d(grid, pts, mixed_species);
        double actual_total = 0.0;
        for (const double q : Q) {
            actual_total += q;
        }

        pidc::test::require(
            pidc::test::approx_equal(actual_total, expected_total, 1e-12),
            "T4: global charge must be conserved to 1e-12");
    }

    // Test 5: output vector has size grid.size()
    {
        const std::vector<pidc::Particle> empty;
        const auto Q = pidc::pic::deposit_charge_cic_1d(grid, empty, species);
        pidc::test::require(Q.size() == grid.size(),
            "T5: empty input must return vector of size nx");

        const pidc::pic::Grid1D big_grid{0.0, 1.0, 32};
        const auto Q2 = pidc::pic::deposit_charge_cic_1d(big_grid, empty, species);
        pidc::test::require(Q2.size() == 32,
            "T5: output size must match grid nx for nx=32");
    }

    // Test 6: invalid species_id throws
    {
        pidc::Particle bad;
        bad.position.x = 0.5;
        bad.species_id = 99;
        bool threw = false;
        try {
            pidc::pic::deposit_charge_cic_1d(grid, {bad}, species);
        } catch (const std::out_of_range&) {
            threw = true;
        }
        pidc::test::require(threw, "T6: invalid species_id must throw out_of_range");
    }

    std::cout << "cic_deposition_1d test passed\n";
    return 0;
}
