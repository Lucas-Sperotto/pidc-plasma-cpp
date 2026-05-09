#include <iostream>
#include <limits>
#include <stdexcept>

#include "pidc/Vec2.hpp"
#include "pidc/mls/InfluenceDomain.hpp"
#include "test_utils.hpp"

namespace {

void require_invalid_circle(double radius, const char* message)
{
    bool threw = false;
    try {
        (void)pidc::CircularInfluenceDomain{radius};
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    pidc::test::require(threw, message);
}

void require_invalid_rectangle(double hx, double hy, const char* message)
{
    bool threw = false;
    try {
        (void)pidc::RectangularInfluenceDomain{hx, hy};
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    pidc::test::require(threw, message);
}

} // namespace

int main()
{
    using pidc::test::approx_equal;
    using pidc::test::require;

    const pidc::Vec2 center{1.0, 2.0};

    const pidc::CircularInfluenceDomain circle{0.5};
    require(circle.contains(center, {1.0, 2.0}), "circle must contain its center");
    require(circle.contains(center, {1.3, 2.4}), "circle must include points inside radius");
    require(circle.contains(center, {1.5, 2.0}), "circle must include boundary points");
    require(!circle.contains(center, {1.5001, 2.0}), "circle must reject outside points");
    require(approx_equal(circle.normalized_distance(center, {1.3, 2.4}), 1.0, 1.0e-12),
            "circle normalized distance must be Euclidean/radius");

    const pidc::RectangularInfluenceDomain rectangle{0.25, 0.75};
    require(rectangle.contains(center, {1.0, 2.0}), "rectangle must contain its center");
    require(rectangle.contains(center, {1.25, 2.75}), "rectangle must include its corner");
    require(rectangle.contains(center, {0.75, 1.25}), "rectangle must include opposite corner");
    require(!rectangle.contains(center, {1.26, 2.0}), "rectangle must reject outside x");
    require(!rectangle.contains(center, {1.0, 2.76}), "rectangle must reject outside y");
    require(approx_equal(rectangle.normalized_distance(center, {1.125, 2.75}), 1.0, 1.0e-12),
            "rectangle normalized distance must be max component ratio");

    require_invalid_circle(0.0, "zero circle radius must throw");
    require_invalid_circle(-1.0, "negative circle radius must throw");
    require_invalid_circle(std::numeric_limits<double>::infinity(),
                           "infinite circle radius must throw");
    require_invalid_rectangle(0.0, 1.0, "zero rectangle half width must throw");
    require_invalid_rectangle(1.0, -1.0, "negative rectangle half height must throw");
    require_invalid_rectangle(std::numeric_limits<double>::quiet_NaN(), 1.0,
                              "NaN rectangle half width must throw");

    std::cout << "influence_domain test passed\n";
    return 0;
}
