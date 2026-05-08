#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Node.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/geometry/NodeCloud.hpp"

namespace pidc {

inline NodeCloud create_regular_node_cloud(const Domain2D& domain, std::size_t nx, std::size_t ny)
{
    if (nx == 0 || ny == 0) {
        throw std::invalid_argument{"RegularNodeCloud2D requires at least one node per dimension"};
    }

    std::vector<Node> nodes;
    nodes.reserve(nx * ny);

    const Vec2 lower = domain.lower();
    const double dx = (nx > 1) ? domain.width() / static_cast<double>(nx - 1) : 0.0;
    const double dy = (ny > 1) ? domain.height() / static_cast<double>(ny - 1) : 0.0;

    for (std::size_t j = 0; j < ny; ++j) {
        for (std::size_t i = 0; i < nx; ++i) {
            const std::size_t id = j * nx + i;
            nodes.push_back(Node{
                id,
                Vec2{lower.x + static_cast<double>(i) * dx, lower.y + static_cast<double>(j) * dy},
                0.0,
            });
        }
    }

    return NodeCloud{std::move(nodes)};
}

} // namespace pidc
