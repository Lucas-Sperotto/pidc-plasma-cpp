#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string_view>

namespace pidc::test {

[[noreturn]] inline void fail(std::string_view message)
{
    std::cerr << "FAIL: " << message << '\n';
    std::exit(1);
}

inline void require(bool condition, std::string_view message)
{
    if (!condition) {
        fail(message);
    }
}

inline bool approx_equal(double lhs, double rhs, double tolerance = 1.0e-12)
{
    return std::abs(lhs - rhs) < tolerance;
}

} // namespace pidc::test
