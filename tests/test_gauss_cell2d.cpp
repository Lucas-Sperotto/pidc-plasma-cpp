#include <cmath>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/efg/GaussCell2D.hpp"
#include "test_utils.hpp"

namespace {

template <typename F>
double integrate(const std::vector<pidc::GaussCell2D>& cells, F function)
{
    double value = 0.0;
    for (const pidc::GaussCell2D& cell : cells) {
        for (const pidc::GaussPoint2D& point : cell.points) {
            value += point.weight * function(point.point);
        }
    }
    return value;
}

void require_close(double actual, double expected, const char* message)
{
    pidc::test::require(pidc::test::approx_equal(actual, expected, 1.0e-12), message);
}

void require_invalid_counts_throw(const pidc::Domain2D& domain, std::size_t nx, std::size_t ny)
{
    bool threw = false;
    try {
        (void)pidc::gauss2x2_cells(domain, nx, ny);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    pidc::test::require(threw, "zero cell count must throw invalid_argument");
}

} // namespace

int main()
{
    const pidc::Domain2D unit{{0.0, 0.0}, {1.0, 1.0}};
    const std::vector<pidc::GaussCell2D> cells = pidc::gauss2x2_cells(unit, 4, 3);

    pidc::test::require(cells.size() == 12, "4x3 integration grid must have 12 cells");
    for (const pidc::GaussCell2D& cell : cells) {
        pidc::test::require(cell.points.size() == 4, "each cell must have four Gauss points");
    }

    require_close(integrate(cells, [](pidc::Vec2) { return 1.0; }), 1.0, "integral of 1");
    require_close(integrate(cells, [](pidc::Vec2 p) { return p.x; }), 0.5, "integral of x");
    require_close(integrate(cells, [](pidc::Vec2 p) { return p.y; }), 0.5, "integral of y");
    require_close(integrate(cells, [](pidc::Vec2 p) { return p.x * p.x; }), 1.0 / 3.0, "integral of x^2");
    require_close(integrate(cells, [](pidc::Vec2 p) { return p.x * p.y; }), 0.25, "integral of xy");

    const pidc::Domain2D rectangle{{1.0, 2.0}, {3.0, 5.0}};
    const std::vector<pidc::GaussCell2D> rectangle_cells = pidc::gauss2x2_cells(rectangle, 2, 2);
    require_close(integrate(rectangle_cells, [](pidc::Vec2) { return 1.0; }), 6.0, "rectangle area");

    require_invalid_counts_throw(unit, 0, 1);
    require_invalid_counts_throw(unit, 1, 0);

    std::cout << "gauss_cell2d test passed\n";
    return 0;
}
