#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>

#include "pidc/pic/Grid1D.hpp"
#include "test_utils.hpp"

namespace {

void require_close(double actual, double expected, const char* message)
{
    pidc::test::require(pidc::test::approx_equal(actual, expected, 1.0e-12), message);
}

} // namespace

int main()
{
    const pidc::pic::Grid1D grid{0.0, 1.0, 4};

    pidc::test::require(grid.size() == 4, "grid size should equal nx");
    require_close(grid.dx, 0.25, "dx should be domain length divided by nx");

    require_close(grid.coordinate(0), 0.0, "node 0 coordinate mismatch");
    require_close(grid.coordinate(1), 0.25, "node 1 coordinate mismatch");
    require_close(grid.coordinate(2), 0.5, "node 2 coordinate mismatch");
    require_close(grid.coordinate(3), 0.75, "node 3 coordinate mismatch");

    require_close(grid.wrap_position(-0.25), 0.75, "negative position should wrap to the right");
    require_close(grid.wrap_position(-1.125), 0.875, "multiple negative crossings should wrap");
    require_close(grid.wrap_position(1.25), 0.25, "position beyond xmax should wrap");
    require_close(grid.wrap_position(2.5), 0.5, "multiple positive crossings should wrap");

    pidc::test::require(grid.cell_index(0.0) == 0, "xmin should belong to cell 0");
    pidc::test::require(grid.cell_index(0.10) == 0, "internal point in first cell mismatch");
    pidc::test::require(grid.cell_index(0.25) == 1, "node at second cell start mismatch");
    pidc::test::require(grid.cell_index(0.63) == 2, "internal point in third cell mismatch");
    pidc::test::require(grid.cell_index(0.88) == 3, "internal point in last cell mismatch");

    const double near_right = std::nextafter(1.0, 0.0);
    pidc::test::require(grid.cell_index(near_right) == 3, "point just below xmax should stay in last cell");
    pidc::test::require(grid.cell_index(1.0) == 0, "xmax should wrap to xmin and cell 0");
    require_close(grid.wrap_position(1.0), 0.0, "xmax should wrap to xmin");

    pidc::test::require(grid.left_node_index(0.875) == 3, "left node near right edge mismatch");
    pidc::test::require(grid.right_node_index(0.875) == 0, "right node near right edge should be periodic");
    pidc::test::require(grid.left_node_index(-0.125) == 3, "left node after negative wrap mismatch");
    pidc::test::require(grid.right_node_index(-0.125) == 0, "right node after negative wrap should be periodic");

    require_close(grid.fraction_in_cell(0.125), 0.5, "midpoint fraction in first cell mismatch");
    require_close(grid.fraction_in_cell(0.375), 0.5, "midpoint fraction in second cell mismatch");
    require_close(grid.fraction_in_cell(-0.125), 0.5, "fraction after negative wrap mismatch");
    pidc::test::require(
        std::abs(grid.fraction_in_cell(near_right) - 1.0) < 1.0e-12,
        "fraction just below xmax should approach one");
    require_close(grid.fraction_in_cell(1.0), 0.0, "fraction at xmax should wrap to zero");

    bool threw_bad_nx = false;
    try {
        (void)pidc::pic::Grid1D{0.0, 1.0, 0};
    } catch (const std::invalid_argument&) {
        threw_bad_nx = true;
    }
    pidc::test::require(threw_bad_nx, "zero nx must throw invalid_argument");

    bool threw_bad_position = false;
    try {
        (void)grid.wrap_position(std::numeric_limits<double>::quiet_NaN());
    } catch (const std::invalid_argument&) {
        threw_bad_position = true;
    }
    pidc::test::require(threw_bad_position, "non-finite positions must throw invalid_argument");

    std::cout << "pic_grid1d test passed\n";
    return 0;
}
