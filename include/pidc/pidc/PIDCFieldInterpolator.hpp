#pragma once

#include <Eigen/Dense>

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/ShapeFunctionData.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/pidc/DiffuseCell.hpp"

namespace pidc {

inline Vec2 interpolate_field_pidc(
    const ShapeFunctionData& shape,
    const Eigen::VectorXd& nodal_potential)
{
    if (!is_valid(shape)) {
        throw std::invalid_argument{"interpolate_field_pidc requires valid ShapeFunctionData"};
    }
    if (!nodal_potential.allFinite()) {
        throw std::invalid_argument{"interpolate_field_pidc requires finite nodal potential"};
    }

    Vec2 gradient{0.0, 0.0};
    for (std::size_t i = 0; i < shape.neighbor_ids.size(); ++i) {
        const auto node_index = static_cast<Eigen::Index>(shape.neighbor_ids[i]);
        if (node_index < 0 || node_index >= nodal_potential.size()) {
            throw std::out_of_range{"interpolate_field_pidc: neighbor_id out of range"};
        }

        const double value = nodal_potential(node_index);
        gradient.x += value * shape.grad_phi[i].x;
        gradient.y += value * shape.grad_phi[i].y;
    }

    const Vec2 field{-gradient.x, -gradient.y};
    if (!std::isfinite(field.x) || !std::isfinite(field.y)) {
        throw std::runtime_error{"interpolate_field_pidc produced a non-finite field"};
    }
    return field;
}

inline std::vector<Vec2> interpolate_field_pidc(
    const std::vector<DiffuseCell>& cells,
    const Eigen::VectorXd& nodal_potential)
{
    std::vector<Vec2> fields(cells.size());
    std::vector<bool> filled(cells.size(), false);

    for (const DiffuseCell& cell : cells) {
        if (cell.particle_index >= cells.size()) {
            throw std::out_of_range{"interpolate_field_pidc: particle_index out of range"};
        }
        if (filled[cell.particle_index]) {
            throw std::invalid_argument{"interpolate_field_pidc: duplicate particle_index"};
        }
        fields[cell.particle_index] = interpolate_field_pidc(cell.shape, nodal_potential);
        filled[cell.particle_index] = true;
    }

    for (const bool was_filled : filled) {
        if (!was_filled) {
            throw std::invalid_argument{"interpolate_field_pidc: missing particle_index"};
        }
    }

    return fields;
}

} // namespace pidc
