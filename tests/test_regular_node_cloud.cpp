#include <cstddef>
#include <iostream>
#include <stdexcept>

#include "pidc/Domain2D.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "test_utils.hpp"

namespace {

void require_position(const pidc::NodeCloud& cloud, std::size_t index, pidc::Vec2 expected)
{
    const pidc::Vec2 actual = cloud[index].position;
    pidc::test::require(pidc::test::approx_equal(actual.x, expected.x), "node x position mismatch");
    pidc::test::require(pidc::test::approx_equal(actual.y, expected.y), "node y position mismatch");
}

} // namespace

int main()
{
    // Test 1: Basic 3x3 grid on unit square.
    {
        const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
        const pidc::NodeCloud cloud = pidc::create_regular_node_cloud(domain, 3, 3);

        pidc::test::require(cloud.size() == 9, "3x3 cloud should have 9 nodes");
        require_position(cloud, 0, {0.0, 0.0});
        require_position(cloud, 2, {1.0, 0.0});
        require_position(cloud, 4, {0.5, 0.5});
        require_position(cloud, 6, {0.0, 1.0});
        require_position(cloud, 8, {1.0, 1.0});
    }

    // Test 2: Rectangular domain 5x3.
    {
        const pidc::Domain2D domain{{0.0, 0.0}, {2.0, 1.0}};
        const pidc::NodeCloud cloud = pidc::create_regular_node_cloud(domain, 5, 3);

        pidc::test::require(cloud.size() == 15, "5x3 cloud should have 15 nodes");
        require_position(cloud, 0, {0.0, 0.0});
        require_position(cloud, 4, {2.0, 0.0});
        require_position(cloud, 7, {1.0, 0.5});
        require_position(cloud, 10, {0.0, 1.0});
        require_position(cloud, 14, {2.0, 1.0});
    }

    // Test 3: Single row and single column.
    {
        const pidc::Domain2D domain{{-1.0, -1.0}, {1.0, 1.0}};

        const pidc::NodeCloud row_cloud = pidc::create_regular_node_cloud(domain, 5, 1);
        pidc::test::require(row_cloud.size() == 5, "row cloud should have 5 nodes");
        require_position(row_cloud, 0, {-1.0, -1.0});
        require_position(row_cloud, 2, {0.0, -1.0});
        require_position(row_cloud, 4, {1.0, -1.0});

        const pidc::NodeCloud col_cloud = pidc::create_regular_node_cloud(domain, 1, 5);
        pidc::test::require(col_cloud.size() == 5, "column cloud should have 5 nodes");
        require_position(col_cloud, 0, {-1.0, -1.0});
        require_position(col_cloud, 2, {-1.0, 0.0});
        require_position(col_cloud, 4, {-1.0, 1.0});
    }

    // Test 4: Invalid dimensions fail before creating nodes.
    {
        const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};

        bool threw_zero_x = false;
        try {
            (void)pidc::create_regular_node_cloud(domain, 0, 2);
        } catch (const std::invalid_argument&) {
            threw_zero_x = true;
        }
        pidc::test::require(threw_zero_x, "zero nx must throw invalid_argument");

        bool threw_zero_y = false;
        try {
            (void)pidc::create_regular_node_cloud(domain, 2, 0);
        } catch (const std::invalid_argument&) {
            threw_zero_y = true;
        }
        pidc::test::require(threw_zero_y, "zero ny must throw invalid_argument");
    }

    std::cout << "regular_node_cloud test passed\n";
    return 0;
}
