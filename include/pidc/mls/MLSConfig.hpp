#pragma once

#include <cmath>
#include <stdexcept>

namespace pidc {

struct MLSConfig {
    double support_radius{0.0};
};

inline void validate_mls_config(const MLSConfig& config)
{
    if (!(config.support_radius > 0.0) || !std::isfinite(config.support_radius)) {
        throw std::invalid_argument{"MLSConfig requires a positive finite support radius"};
    }
}

} // namespace pidc
