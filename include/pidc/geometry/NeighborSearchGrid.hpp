#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/geometry/NodeCloud.hpp"

namespace pidc {

class NeighborSearchGrid {
public:
    NeighborSearchGrid(const NodeCloud& cloud, const Domain2D& domain, double cell_size)
        : cloud_{&cloud},
          domain_{domain},
          cell_size_{cell_size}
    {
        if (!(cell_size_ > 0.0) || !std::isfinite(cell_size_)) {
            throw std::invalid_argument{"NeighborSearchGrid requires a positive finite cell size"};
        }

        nx_ = std::max<std::size_t>(
            1,
            static_cast<std::size_t>(std::ceil(domain_.width() / cell_size_)));
        ny_ = std::max<std::size_t>(
            1,
            static_cast<std::size_t>(std::ceil(domain_.height() / cell_size_)));
        cells_.resize(nx_ * ny_);

        for (std::size_t i = 0; i < cloud.size(); ++i) {
            const Vec2 position = cloud[i].position;
            if (!point_is_finite(position) || !inside_inclusive_domain(position)) {
                throw std::invalid_argument{"NeighborSearchGrid nodes must lie within domain bounds"};
            }
            cells_[flat_index(cell_x(position.x), cell_y(position.y))].push_back(i);
        }
    }

    std::vector<std::size_t> query_radius(Vec2 point, double radius) const
    {
        if (!(radius >= 0.0) || !std::isfinite(radius)) {
            throw std::invalid_argument{"NeighborSearchGrid requires a non-negative finite radius"};
        }

        std::vector<std::size_t> result;
        const double radius_squared = radius * radius;

        const std::size_t min_x = cell_x(point.x - radius);
        const std::size_t max_x = cell_x(point.x + radius);
        const std::size_t min_y = cell_y(point.y - radius);
        const std::size_t max_y = cell_y(point.y + radius);

        for (std::size_t iy = min_y; iy <= max_y; ++iy) {
            for (std::size_t ix = min_x; ix <= max_x; ++ix) {
                for (const std::size_t node_index : cells_[flat_index(ix, iy)]) {
                    const Vec2 node_position = (*cloud_)[node_index].position;
                    const double dx = point.x - node_position.x;
                    const double dy = point.y - node_position.y;
                    if (dx * dx + dy * dy <= radius_squared + 1.0e-14) {
                        result.push_back(node_index);
                    }
                }
            }
        }

        std::sort(result.begin(), result.end());
        return result;
    }

private:
    const NodeCloud* cloud_{nullptr};
    Domain2D domain_;
    double cell_size_{1.0};
    std::size_t nx_{1};
    std::size_t ny_{1};
    std::vector<std::vector<std::size_t>> cells_;

    static bool point_is_finite(Vec2 point) noexcept
    {
        return std::isfinite(point.x) && std::isfinite(point.y);
    }

    bool inside_inclusive_domain(Vec2 point) const noexcept
    {
        return point.x >= domain_.lower().x && point.x <= domain_.upper().x &&
               point.y >= domain_.lower().y && point.y <= domain_.upper().y;
    }

    std::size_t cell_x(double x) const noexcept
    {
        return coordinate_cell(x, domain_.lower().x, nx_);
    }

    std::size_t cell_y(double y) const noexcept
    {
        return coordinate_cell(y, domain_.lower().y, ny_);
    }

    std::size_t coordinate_cell(double value, double lower, std::size_t count) const noexcept
    {
        const double raw = std::floor((value - lower) / cell_size_);
        if (raw <= 0.0) {
            return 0;
        }

        const auto index = static_cast<std::size_t>(raw);
        return std::min(index, count - 1);
    }

    std::size_t flat_index(std::size_t ix, std::size_t iy) const noexcept
    {
        return iy * nx_ + ix;
    }
};

} // namespace pidc
