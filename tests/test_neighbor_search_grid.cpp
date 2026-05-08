#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Node.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/geometry/NeighborSearchGrid.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "test_utils.hpp"

namespace {

std::vector<std::size_t> brute_force_query(
    const pidc::NodeCloud& cloud,
    pidc::Vec2 point,
    double radius)
{
    std::vector<std::size_t> result;
    const double radius_squared = radius * radius;

    for (std::size_t i = 0; i < cloud.size(); ++i) {
        const pidc::Vec2 node_position = cloud[i].position;
        const double dx = point.x - node_position.x;
        const double dy = point.y - node_position.y;
        if (dx * dx + dy * dy <= radius_squared + 1.0e-14) {
            result.push_back(i);
        }
    }

    std::sort(result.begin(), result.end());
    return result;
}

void require_equal_ids(
    const std::vector<std::size_t>& lhs,
    const std::vector<std::size_t>& rhs,
    const char* message)
{
    pidc::test::require(lhs == rhs, message);
}

void require_invalid_cell_size_throws(
    const pidc::NodeCloud& cloud,
    const pidc::Domain2D& domain,
    double cell_size,
    const char* message)
{
    bool threw = false;
    try {
        (void)pidc::NeighborSearchGrid{cloud, domain, cell_size};
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    pidc::test::require(threw, message);
}

void require_invalid_radius_throws(
    const pidc::NeighborSearchGrid& grid,
    double radius,
    const char* message)
{
    bool threw = false;
    try {
        (void)grid.query_radius(pidc::Vec2{0.5, 0.5}, radius);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    pidc::test::require(threw, message);
}

} // namespace

int main()
{
    using pidc::test::require;

    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
    const pidc::NodeCloud cloud = pidc::create_regular_node_cloud(domain, 5, 5);
    const pidc::NeighborSearchGrid grid{cloud, domain, 0.25};

    const std::vector<pidc::Vec2> points{
        {0.5, 0.5},
        {0.0, 0.0},
        {1.0, 1.0},
        {0.37, 0.61},
        {-0.1, 0.5},
        {1.1, 0.5},
    };

    const std::vector<double> radii{0.0, 0.10, 0.26, 0.40};

    for (const pidc::Vec2 point : points) {
        for (const double radius : radii) {
            require_equal_ids(
                grid.query_radius(point, radius),
                brute_force_query(cloud, point, radius),
                "grid query must match brute-force query");
        }
    }

    const std::vector<std::size_t> center_expected{7, 11, 12, 13, 17};
    require_equal_ids(
        grid.query_radius(pidc::Vec2{0.5, 0.5}, 0.26),
        center_expected,
        "center query should return center plus cardinal neighbours");

    const std::vector<std::size_t> corner_expected{0, 1, 5};
    require_equal_ids(
        grid.query_radius(pidc::Vec2{0.0, 0.0}, 0.26),
        corner_expected,
        "corner query should include upper/right adjacent nodes");

    const std::vector<std::size_t> upper_corner_expected{19, 23, 24};
    require_equal_ids(
        grid.query_radius(pidc::Vec2{1.0, 1.0}, 0.26),
        upper_corner_expected,
        "upper boundary nodes should be indexed in the last grid cells");

    require_invalid_cell_size_throws(cloud, domain, 0.0, "zero cell size must throw");
    require_invalid_cell_size_throws(cloud, domain, -1.0, "negative cell size must throw");
    require_invalid_cell_size_throws(
        cloud,
        domain,
        std::numeric_limits<double>::infinity(),
        "infinite cell size must throw");

    require_invalid_radius_throws(grid, -1.0, "negative radius must throw");
    require_invalid_radius_throws(
        grid,
        std::numeric_limits<double>::quiet_NaN(),
        "NaN radius must throw");

    const pidc::NodeCloud outside_cloud{{pidc::Node{0, pidc::Vec2{1.1, 0.5}}}};
    bool outside_node_threw = false;
    try {
        (void)pidc::NeighborSearchGrid{outside_cloud, domain, 0.25};
    } catch (const std::invalid_argument&) {
        outside_node_threw = true;
    }
    require(outside_node_threw, "nodes outside the domain must throw during grid construction");

    std::cout << "neighbor_search_grid test passed\n";
    return 0;
}
