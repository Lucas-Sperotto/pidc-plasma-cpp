#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/ShapeFunctionData.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/mls/MLSShapeFunction.hpp"

namespace pidc {

struct DiffuseCell {
    std::size_t particle_index{0};
    ShapeFunctionData shape{};
};

inline std::vector<DiffuseCell> evaluate_diffuse_cells(
    const NodeCloud& cloud,
    const std::vector<Particle>& particles,
    const MLSConfig& mls_config)
{
    validate_mls_config(mls_config);

    std::vector<DiffuseCell> cells;
    cells.reserve(particles.size());

    for (std::size_t i = 0; i < particles.size(); ++i) {
        DiffuseCell cell;
        cell.particle_index = i;
        cell.shape = mls_evaluate(particles[i].position, cloud, mls_config);
        if (!is_valid(cell.shape)) {
            throw std::runtime_error{"evaluate_diffuse_cells produced inconsistent shape data"};
        }
        cells.push_back(std::move(cell));
    }

    return cells;
}

} // namespace pidc
