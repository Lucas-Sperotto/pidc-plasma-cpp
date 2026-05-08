#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Vec2.hpp"

namespace pidc {

struct GaussPoint2D {
    Vec2 point{};
    double weight{0.0};
};

struct GaussCell2D {
    Vec2 lower{};
    Vec2 upper{};
    std::array<GaussPoint2D, 4> points{};
};

inline std::vector<GaussCell2D> gauss2x2_cells(
    const Domain2D& domain,
    std::size_t nx,
    std::size_t ny)
{
    if (nx == 0 || ny == 0) {
        throw std::invalid_argument{"gauss2x2_cells requires positive cell counts"};
    }

    const Vec2 lower = domain.lower();
    const double dx = domain.width() / static_cast<double>(nx);
    const double dy = domain.height() / static_cast<double>(ny);
    const double reference = 1.0 / std::sqrt(3.0);

    std::vector<GaussCell2D> cells;
    cells.reserve(nx * ny);

    for (std::size_t j = 0; j < ny; ++j) {
        for (std::size_t i = 0; i < nx; ++i) {
            const Vec2 cell_lower{
                lower.x + static_cast<double>(i) * dx,
                lower.y + static_cast<double>(j) * dy,
            };
            const Vec2 cell_upper{cell_lower.x + dx, cell_lower.y + dy};
            const Vec2 center{0.5 * (cell_lower.x + cell_upper.x), 0.5 * (cell_lower.y + cell_upper.y)};
            const double weight = 0.25 * dx * dy;

            GaussCell2D cell;
            cell.lower = cell_lower;
            cell.upper = cell_upper;
            cell.points = {{
                GaussPoint2D{{center.x - 0.5 * dx * reference, center.y - 0.5 * dy * reference}, weight},
                GaussPoint2D{{center.x + 0.5 * dx * reference, center.y - 0.5 * dy * reference}, weight},
                GaussPoint2D{{center.x - 0.5 * dx * reference, center.y + 0.5 * dy * reference}, weight},
                GaussPoint2D{{center.x + 0.5 * dx * reference, center.y + 0.5 * dy * reference}, weight},
            }};

            cells.push_back(cell);
        }
    }

    return cells;
}

} // namespace pidc
