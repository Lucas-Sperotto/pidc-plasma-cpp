#pragma once

#include <cstddef>
#include <vector>

#include "pidc/Vec2.hpp"

namespace pidc {

struct ShapeFunctionData {
    std::vector<std::size_t> neighbor_ids;
    std::vector<double>      phi;
    std::vector<Vec2>        grad_phi;
};

inline bool is_valid(const ShapeFunctionData& d) noexcept
{
    return d.phi.size() == d.neighbor_ids.size()
        && d.phi.size() == d.grad_phi.size();
}

} // namespace pidc
