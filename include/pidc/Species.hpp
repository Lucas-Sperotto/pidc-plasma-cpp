#pragma once

#include <string>

namespace pidc {

struct Species {
    double      charge{0.0};
    double      mass{1.0};
    std::string label{};
};

} // namespace pidc
