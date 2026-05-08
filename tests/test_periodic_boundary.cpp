#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "pidc/Domain2D.hpp"
#include "pidc/Vec2.hpp"

namespace {

bool approx_equal(double lhs, double rhs)
{
    return std::abs(lhs - rhs) < 1.0e-12;
}

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

} // namespace

int main()
{
    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 2.0}};

    const pidc::Vec2 right_top = domain.wrapPeriodic({1.25, 2.75});
    require(approx_equal(right_top.x, 0.25), "point leaving right returns on left");
    require(approx_equal(right_top.y, 0.75), "point leaving top returns at bottom");
    require(domain.contains(right_top), "wrapped right/top point is inside domain");

    const pidc::Vec2 left_bottom = domain.wrapPeriodic({-0.25, -0.50});
    require(approx_equal(left_bottom.x, 0.75), "point leaving left returns on right");
    require(approx_equal(left_bottom.y, 1.50), "point leaving bottom returns at top");
    require(domain.contains(left_bottom), "wrapped left/bottom point is inside domain");

    const pidc::Vec2 exactly_upper = domain.wrapPeriodic({1.0, 2.0});
    require(approx_equal(exactly_upper.x, 0.0), "upper x boundary wraps to lower x");
    require(approx_equal(exactly_upper.y, 0.0), "upper y boundary wraps to lower y");
    require(domain.contains(exactly_upper), "point on upper boundary is wrapped inside");

    std::cout << "periodic boundary test passed\n";
    return 0;
}
