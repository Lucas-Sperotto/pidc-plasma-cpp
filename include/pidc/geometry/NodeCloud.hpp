#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "pidc/Node.hpp"

namespace pidc {

class NodeCloud {
public:
    explicit NodeCloud(std::vector<Node> nodes) : nodes_{std::move(nodes)} {}

    std::size_t size() const noexcept
    {
        return nodes_.size();
    }

    const Node& operator[](std::size_t index) const
    {
        return nodes_[index];
    }

    const std::vector<Node>& nodes() const noexcept
    {
        return nodes_;
    }

private:
    std::vector<Node> nodes_{};
};

} // namespace pidc
