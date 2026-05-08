#include <iostream>
#include <utility>
#include <vector>

#include "pidc/geometry/NodeCloud.hpp"
#include "test_utils.hpp"

int main()
{
    using pidc::test::approx_equal;
    using pidc::test::require;

    const pidc::NodeCloud empty{std::vector<pidc::Node>{}};
    require(empty.size() == 0, "empty NodeCloud must have size zero");
    require(empty.nodes().empty(), "empty NodeCloud must expose an empty node list");

    std::vector<pidc::Node> nodes{
        {10, {0.0, 0.0}},
        {20, {1.0, 0.0}},
        {30, {0.0, 1.0}},
    };

    const pidc::NodeCloud cloud{std::move(nodes)};

    require(cloud.size() == 3, "NodeCloud must report the number of owned nodes");
    require(cloud.nodes().size() == cloud.size(), "nodes() size must match size()");

    require(cloud[0].id == 10, "NodeCloud must preserve node order at index 0");
    require(cloud[1].id == 20, "NodeCloud must preserve node order at index 1");
    require(cloud[2].id == 30, "NodeCloud must preserve node order at index 2");

    require(approx_equal(cloud[1].position.x, 1.0), "operator[] must expose node position x");
    require(approx_equal(cloud[2].position.y, 1.0), "operator[] must expose node position y");

    std::cout << "node_cloud test passed\n";
    return 0;
}
