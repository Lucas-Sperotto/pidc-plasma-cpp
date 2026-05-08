#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "pidc/geometry/NodeCloud.hpp"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

bool approx_equal(double lhs, double rhs)
{
    const double tolerance = 1.0e-12;
    const double diff = lhs - rhs;
    return diff < tolerance && diff > -tolerance;
}

} // namespace

int main()
{
    const pidc::NodeCloud empty{std::vector<pidc::Node>{}};
    require(empty.size() == 0, "empty NodeCloud must have size zero");
    require(empty.nodes().empty(), "empty NodeCloud must expose an empty node list");

    std::vector<pidc::Node> nodes{
        {10, {0.0, 0.0}, 0.25},
        {20, {1.0, 0.0}, 0.25},
        {30, {0.0, 1.0}, 0.50},
    };

    const pidc::NodeCloud cloud{std::move(nodes)};

    require(cloud.size() == 3, "NodeCloud must report the number of owned nodes");
    require(cloud.nodes().size() == cloud.size(), "nodes() size must match size()");

    require(cloud[0].id == 10, "NodeCloud must preserve node order at index 0");
    require(cloud[1].id == 20, "NodeCloud must preserve node order at index 1");
    require(cloud[2].id == 30, "NodeCloud must preserve node order at index 2");

    require(approx_equal(cloud[1].position.x, 1.0), "operator[] must expose node position x");
    require(approx_equal(cloud[2].position.y, 1.0), "operator[] must expose node position y");
    require(approx_equal(cloud.nodes()[2].volume, 0.50), "nodes() must expose node volume");

    std::cout << "node_cloud test passed\n";
    return 0;
}
