#pragma once

#include <Eigen/Dense>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/mls/MLSShapeFunction.hpp"
#include "pidc/pidc/DiffuseCell.hpp"

namespace pidc {

inline Eigen::VectorXd deposit_charge_from_cells(
    const NodeCloud& cloud,
    const std::vector<Particle>& particles,
    const std::vector<Species>& species_list,
    const std::vector<DiffuseCell>& cells)
{
    const auto n = static_cast<Eigen::Index>(cloud.size());
    Eigen::VectorXd charges = Eigen::VectorXd::Zero(n);

    if (cells.size() != particles.size()) {
        throw std::invalid_argument{
            "deposit_charge_from_cells requires one diffuse cell per particle"};
    }

    for (const DiffuseCell& cell : cells) {
        if (cell.particle_index >= particles.size()) {
            throw std::out_of_range{"deposit_charge_from_cells: particle_index out of range"};
        }
        if (!is_valid(cell.shape)) {
            throw std::invalid_argument{"deposit_charge_from_cells: invalid ShapeFunctionData"};
        }

        const Particle& particle = particles[cell.particle_index];
        if (particle.species_id >= species_list.size()) {
            throw std::out_of_range{"deposit_charge_from_cells: particle species_id out of range"};
        }

        const double q = species_list[particle.species_id].charge;
        for (std::size_t i = 0; i < cell.shape.neighbor_ids.size(); ++i) {
            const auto idx = static_cast<Eigen::Index>(cell.shape.neighbor_ids[i]);
            if (idx < 0 || idx >= n) {
                throw std::out_of_range{"deposit_charge_from_cells: neighbor_id out of range"};
            }
            charges(idx) += q * cell.shape.phi[i];
        }
    }

    return charges;
}

// DEC-0026: deposit_charge accumulates q_p * phi_i(x_p) for every particle p
// into nodal charge vector Q. Global conservation follows directly from the
// partition-of-unity property: sum_i phi_i(x) = 1 for any interior x.
inline Eigen::VectorXd deposit_charge(
    const NodeCloud& cloud,
    const std::vector<Particle>& particles,
    const std::vector<Species>& species_list,
    const MLSConfig& mls_config)
{
    validate_mls_config(mls_config);
    const std::vector<DiffuseCell> cells = evaluate_diffuse_cells(cloud, particles, mls_config);
    return deposit_charge_from_cells(cloud, particles, species_list, cells);
}

} // namespace pidc
