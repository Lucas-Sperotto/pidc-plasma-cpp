#pragma once

#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/pic/Grid1D.hpp"

namespace pidc::pic {

// DEC-0028: std::vector<double>, sem Eigen. DEC-0027: grade periódica semiaberta.
// Acumula q_p * w_left em left_node e q_p * w_right em right_node (CIC linear).
// Conservação garantida: sum(Q_i) == sum(q_p), pois w_left + w_right == 1.
inline std::vector<double> deposit_charge_cic_1d(
    const Grid1D& grid,
    const std::vector<pidc::Particle>& particles,
    const std::vector<pidc::Species>& species_list)
{
    std::vector<double> charges(grid.size(), 0.0);

    for (const pidc::Particle& p : particles) {
        if (p.species_id >= species_list.size()) {
            throw std::out_of_range{"deposit_charge_cic_1d: species_id out of range"};
        }
        const double q = species_list[p.species_id].charge;
        const double f = grid.fraction_in_cell(p.position.x);
        const std::size_t left  = grid.left_node_index(p.position.x);
        const std::size_t right = grid.right_node_index(p.position.x);
        charges[left]  += q * (1.0 - f);
        charges[right] += q * f;
    }

    return charges;
}

} // namespace pidc::pic
