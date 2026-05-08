#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "pidc/ShapeFunctionData.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/mls/MLSShapeFunction.hpp"

namespace {

void require(bool cond, const char* msg)
{
    if (!cond) {
        std::cerr << "FAIL: " << msg << '\n';
        std::exit(1);
    }
}

bool approx_equal(double lhs, double rhs, double tol = 1.0e-10)
{
    return std::abs(lhs - rhs) < tol;
}

// Regular 5x5 node grid in [0,1]^2 with spacing 0.25
pidc::NodeCloud make_regular_cloud()
{
    std::vector<pidc::Node> nodes;
    nodes.reserve(25);
    std::size_t id = 0;
    for (int j = 0; j < 5; ++j)
        for (int i = 0; i < 5; ++i)
            nodes.push_back({id++, pidc::Vec2{i * 0.25, j * 0.25}, 1.0});
    return pidc::NodeCloud{std::move(nodes)};
}

} // namespace

int main()
{
    const pidc::NodeCloud cloud = make_regular_cloud();
    const double h = 0.6;  // support radius — covers ~12 neighbours from centre

    // --- primary query point (interior, symmetric) ---
    const pidc::Vec2 x{0.5, 0.5};

    pidc::ShapeFunctionData sfd;
    try {
        sfd = pidc::mls_evaluate(x, cloud, h);
    } catch (const std::exception& e) {
        std::cerr << "FAIL: mls_evaluate threw: " << e.what() << '\n';
        return 1;
    }

    require(pidc::is_valid(sfd), "ShapeFunctionData must be valid (sizes consistent)");

    double sum_phi     = 0.0;
    double sum_dphi_dx = 0.0, sum_dphi_dy = 0.0;
    double sum_phi_x   = 0.0, sum_phi_y   = 0.0;
    double sum_dx_x    = 0.0, sum_dx_y    = 0.0;
    double sum_dy_x    = 0.0, sum_dy_y    = 0.0;

    for (std::size_t k = 0; k < sfd.phi.size(); ++k) {
        const pidc::Vec2 xi = cloud[sfd.neighbor_ids[k]].position;
        sum_phi     += sfd.phi[k];
        sum_dphi_dx += sfd.grad_phi[k].x;
        sum_dphi_dy += sfd.grad_phi[k].y;
        sum_phi_x   += sfd.phi[k] * xi.x;
        sum_phi_y   += sfd.phi[k] * xi.y;
        sum_dx_x    += sfd.grad_phi[k].x * xi.x;
        sum_dx_y    += sfd.grad_phi[k].x * xi.y;
        sum_dy_x    += sfd.grad_phi[k].y * xi.x;
        sum_dy_y    += sfd.grad_phi[k].y * xi.y;
    }

    require(approx_equal(sum_phi,     1.0),  "partition of unity: sum(phi) == 1");
    require(approx_equal(sum_phi_x,   x.x),  "linear reproduction: sum(phi * xi.x) == x.x");
    require(approx_equal(sum_phi_y,   x.y),  "linear reproduction: sum(phi * xi.y) == x.y");
    require(approx_equal(sum_dphi_dx, 0.0),  "gradient PU x: sum(dphi/dx) == 0");
    require(approx_equal(sum_dphi_dy, 0.0),  "gradient PU y: sum(dphi/dy) == 0");
    require(approx_equal(sum_dx_x,    1.0),  "gradient LR: sum(dphi/dx * xi.x) == 1");
    require(approx_equal(sum_dx_y,    0.0),  "gradient LR: sum(dphi/dx * xi.y) == 0");
    require(approx_equal(sum_dy_x,    0.0),  "gradient LR: sum(dphi/dy * xi.x) == 0");
    require(approx_equal(sum_dy_y,    1.0),  "gradient LR: sum(dphi/dy * xi.y) == 1");

    // --- second query point (off-centre) ---
    const pidc::Vec2 x2{0.3, 0.7};
    const pidc::ShapeFunctionData sfd2 = pidc::mls_evaluate(x2, cloud, h);
    require(pidc::is_valid(sfd2), "second query: valid");

    double s2 = 0.0, s2x = 0.0, s2y = 0.0;
    for (std::size_t k = 0; k < sfd2.phi.size(); ++k) {
        const pidc::Vec2 xi = cloud[sfd2.neighbor_ids[k]].position;
        s2  += sfd2.phi[k];
        s2x += sfd2.phi[k] * xi.x;
        s2y += sfd2.phi[k] * xi.y;
    }
    require(approx_equal(s2,  1.0),   "second query: partition of unity");
    require(approx_equal(s2x, x2.x),  "second query: linear reproduction x");
    require(approx_equal(s2y, x2.y),  "second query: linear reproduction y");

    // --- exception with too few neighbours ---
    const pidc::NodeCloud tiny{{pidc::Node{0, pidc::Vec2{0.5, 0.5}, 1.0}}};
    bool threw = false;
    try {
        pidc::mls_evaluate({0.5, 0.5}, tiny, 1.0);
    } catch (const std::runtime_error&) {
        threw = true;
    }
    require(threw, "must throw std::runtime_error when fewer than 3 neighbours");

    std::cout << "mls_shape_function test passed\n";
    return 0;
}
