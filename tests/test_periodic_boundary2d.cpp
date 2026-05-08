#include <iostream>

#include "pidc/Domain2D.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/geometry/PeriodicBoundary2D.hpp"
#include "test_utils.hpp"

namespace {

void require_vec2_close(pidc::Vec2 value, pidc::Vec2 expected, const char* message)
{
    using pidc::test::approx_equal;
    using pidc::test::require;

    require(approx_equal(value.x, expected.x), message);
    require(approx_equal(value.y, expected.y), message);
}

} // namespace

int main()
{
    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 2.0}};
    const pidc::PeriodicBoundary2D boundary{domain};

    require_vec2_close(
        boundary.wrap(pidc::Vec2{1.25, -0.25}),
        pidc::Vec2{0.25, 1.75},
        "wrap should map points back into the periodic domain");

    require_vec2_close(
        boundary.wrap(pidc::Vec2{-2.25, 4.5}),
        pidc::Vec2{0.75, 0.5},
        "wrap should handle multiple crossings in both directions");

    require_vec2_close(
        boundary.minimum_image(pidc::Vec2{0.9, 0.0}),
        pidc::Vec2{-0.1, 0.0},
        "minimum image should choose the shorter positive x displacement");

    require_vec2_close(
        boundary.minimum_image(pidc::Vec2{-0.9, 0.0}),
        pidc::Vec2{0.1, 0.0},
        "minimum image should choose the shorter negative x displacement");

    require_vec2_close(
        boundary.minimum_image(pidc::Vec2{0.4, 1.2}),
        pidc::Vec2{0.4, -0.8},
        "minimum image should act independently per component");

    require_vec2_close(
        boundary.minimum_image(pidc::Vec2{0.5, -1.0}),
        pidc::Vec2{0.5, -1.0},
        "minimum image should preserve exact half-domain ties");

    require_vec2_close(
        boundary.minimum_image(pidc::Vec2{1.5, -3.0}),
        pidc::Vec2{0.5, -1.0},
        "minimum image should preserve repeated half-domain ties after reduction");

    std::cout << "periodic_boundary2d test passed\n";
    return 0;
}
