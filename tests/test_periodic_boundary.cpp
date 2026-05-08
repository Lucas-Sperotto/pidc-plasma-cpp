#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "pidc/Domain2D.hpp"
#include "pidc/Particle.hpp"

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

    pidc::Particle right_top{{1.25, 2.75}, {}, -1.0, 1.0};
    domain.applyPeriodic(right_top);
    require(approx_equal(right_top.position.x, 0.25), "particle leaving right returns on left");
    require(approx_equal(right_top.position.y, 0.75), "particle leaving top returns at bottom");
    require(domain.contains(right_top.position), "wrapped right/top particle is inside domain");

    pidc::Particle left_bottom{{-0.25, -0.50}, {}, -1.0, 1.0};
    domain.applyPeriodic(left_bottom);
    require(approx_equal(left_bottom.position.x, 0.75), "particle leaving left returns on right");
    require(approx_equal(left_bottom.position.y, 1.50), "particle leaving bottom returns at top");
    require(domain.contains(left_bottom.position), "wrapped left/bottom particle is inside domain");

    pidc::Particle exactly_upper{{1.0, 2.0}, {}, -1.0, 1.0};
    domain.applyPeriodic(exactly_upper);
    require(approx_equal(exactly_upper.position.x, 0.0), "upper x boundary wraps to lower x");
    require(approx_equal(exactly_upper.position.y, 0.0), "upper y boundary wraps to lower y");
    require(domain.contains(exactly_upper.position), "particle on upper boundary is wrapped inside");

    std::cout << "periodic boundary test passed\n";
    return 0;
}
