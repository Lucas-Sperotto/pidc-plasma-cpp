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

namespace pidc {

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

    const auto n = static_cast<Eigen::Index>(cloud.size());
    Eigen::VectorXd charges = Eigen::VectorXd::Zero(n);

    for (const Particle& particle : particles) {
        if (particle.species_id >= species_list.size()) {
            throw std::out_of_range{"deposit_charge: particle species_id out of range"};
        }
        const double q = species_list[particle.species_id].charge;
        const ShapeFunctionData shape = mls_evaluate(particle.position, cloud, mls_config);

        for (std::size_t i = 0; i < shape.neighbor_ids.size(); ++i) {
            const auto idx = static_cast<Eigen::Index>(shape.neighbor_ids[i]);
            charges(idx) += q * shape.phi[i];
        }
    }

    return charges;
}

} // namespace pidc
