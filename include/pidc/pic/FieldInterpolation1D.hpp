#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/pic/Grid1D.hpp"

namespace pidc::pic {
namespace detail {

inline void validate_field_nodes_1d(const Grid1D& grid, const std::vector<double>& field_nodes)
{
    if (field_nodes.size() != grid.size()) {
        throw std::invalid_argument{"field_nodes size must match Grid1D size"};
    }

    for (const double value : field_nodes) {
        if (!std::isfinite(value)) {
            throw std::invalid_argument{"field_nodes values must be finite"};
        }
    }
}

inline double interpolate_field_cic_1d_unchecked(
    const Grid1D& grid,
    const std::vector<double>& field_nodes,
    double x)
{
    const std::size_t left = grid.left_node_index(x);
    const std::size_t right = grid.right_node_index(x);
    const double f = grid.fraction_in_cell(x);

    return field_nodes[left] * (1.0 - f) + field_nodes[right] * f;
}

} // namespace detail

inline double interpolate_field_cic_1d(
    const Grid1D& grid,
    const std::vector<double>& field_nodes,
    double x)
{
    detail::validate_field_nodes_1d(grid, field_nodes);
    return detail::interpolate_field_cic_1d_unchecked(grid, field_nodes, x);
}

inline std::vector<double> interpolate_field_cic_1d(
    const Grid1D& grid,
    const std::vector<double>& field_nodes,
    const std::vector<pidc::Particle>& particles)
{
    detail::validate_field_nodes_1d(grid, field_nodes);

    std::vector<double> values;
    values.reserve(particles.size());
    for (const auto& particle : particles) {
        values.push_back(detail::interpolate_field_cic_1d_unchecked(
            grid,
            field_nodes,
            particle.position.x));
    }

    return values;
}

} // namespace pidc::pic
