#include <iostream>
#include <stdexcept>
#include <vector>

#include "pidc/ShapeFunctionData.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/mls/MLSShapeFunction.hpp"
#include "test_utils.hpp"

namespace {

// Regular 5x5 node grid in [0,1]^2 with spacing 0.25
pidc::NodeCloud make_regular_cloud()
{
    std::vector<pidc::Node> nodes;
    nodes.reserve(25);
    std::size_t id = 0;
    for (int j = 0; j < 5; ++j)
        for (int i = 0; i < 5; ++i)
            nodes.push_back({id++, pidc::Vec2{i * 0.25, j * 0.25}});
    return pidc::NodeCloud{std::move(nodes)};
}

} // namespace

int main()
{
    using pidc::test::approx_equal;
    using pidc::test::require;

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

    require(approx_equal(sum_phi,     1.0, 1.0e-10),  "partition of unity: sum(phi) == 1");
    require(approx_equal(sum_phi_x,   x.x, 1.0e-10),  "linear reproduction: sum(phi * xi.x) == x.x");
    require(approx_equal(sum_phi_y,   x.y, 1.0e-10),  "linear reproduction: sum(phi * xi.y) == x.y");
    require(approx_equal(sum_dphi_dx, 0.0, 1.0e-10),  "gradient PU x: sum(dphi/dx) == 0");
    require(approx_equal(sum_dphi_dy, 0.0, 1.0e-10),  "gradient PU y: sum(dphi/dy) == 0");
    require(approx_equal(sum_dx_x,    1.0, 1.0e-10),  "gradient LR: sum(dphi/dx * xi.x) == 1");
    require(approx_equal(sum_dx_y,    0.0, 1.0e-10),  "gradient LR: sum(dphi/dx * xi.y) == 0");
    require(approx_equal(sum_dy_x,    0.0, 1.0e-10),  "gradient LR: sum(dphi/dy * xi.x) == 0");
    require(approx_equal(sum_dy_y,    1.0, 1.0e-10),  "gradient LR: sum(dphi/dy * xi.y) == 1");

    // --- asymmetric query points: gradient correctness (R-013 verification) ---
    // R-013 (Gemini T-020) claimed an extra (∇p)ᵀ c wᵢ term in the gradient.
    // Mathematical analysis shows the formula is correct: dc_dx.dot(pki) already
    // contains (∂p/∂x)ᵀ A⁻¹ pᵢ as its first component; Gemini confused A⁻¹pᵢ
    // with c = A⁻¹p(x). These tests are asymmetric so symmetry cannot mask
    // any potential error — they would fail if the formula were wrong.

    auto check_all = [&](const pidc::ShapeFunctionData& s,
                         pidc::Vec2 qp,
                         const char* tag) {
        double su = 0.0, sx = 0.0, sy = 0.0;
        double sdx = 0.0, sdy = 0.0;
        double sdx_x = 0.0, sdx_y = 0.0;
        double sdy_x = 0.0, sdy_y = 0.0;
        for (std::size_t k = 0; k < s.phi.size(); ++k) {
            const pidc::Vec2 xi = cloud[s.neighbor_ids[k]].position;
            su    += s.phi[k];
            sx    += s.phi[k] * xi.x;
            sy    += s.phi[k] * xi.y;
            sdx   += s.grad_phi[k].x;
            sdy   += s.grad_phi[k].y;
            sdx_x += s.grad_phi[k].x * xi.x;
            sdx_y += s.grad_phi[k].x * xi.y;
            sdy_x += s.grad_phi[k].y * xi.x;
            sdy_y += s.grad_phi[k].y * xi.y;
        }
        (void)tag;
        require(approx_equal(su,    1.0,  1.0e-10), "PU");
        require(approx_equal(sx,    qp.x, 1.0e-10), "LR x");
        require(approx_equal(sy,    qp.y, 1.0e-10), "LR y");
        require(approx_equal(sdx,   0.0,  1.0e-10), "grad PU x");
        require(approx_equal(sdy,   0.0,  1.0e-10), "grad PU y");
        require(approx_equal(sdx_x, 1.0,  1.0e-10), "grad LR xx");
        require(approx_equal(sdx_y, 0.0,  1.0e-10), "grad LR xy");
        require(approx_equal(sdy_x, 0.0,  1.0e-10), "grad LR yx");
        require(approx_equal(sdy_y, 1.0,  1.0e-10), "grad LR yy");
    };

    // second query point (off-centre, T-017 original)
    const pidc::Vec2 x2{0.3, 0.7};
    const pidc::ShapeFunctionData sfd2 = pidc::mls_evaluate(x2, cloud, h);
    require(pidc::is_valid(sfd2), "second query: valid");
    check_all(sfd2, x2, "x2");

    // third query point — explicitly suggested in R-013
    const pidc::Vec2 x3{0.6, 0.4};
    const pidc::ShapeFunctionData sfd3 = pidc::mls_evaluate(x3, cloud, h);
    require(pidc::is_valid(sfd3), "third query: valid");
    check_all(sfd3, x3, "x3");

    // fourth query point — irrational-looking to minimise accidental symmetry
    const pidc::Vec2 x4{0.37, 0.61};
    const pidc::ShapeFunctionData sfd4 = pidc::mls_evaluate(x4, cloud, h);
    require(pidc::is_valid(sfd4), "fourth query: valid");
    check_all(sfd4, x4, "x4");

    // --- exception with too few neighbours ---
    const pidc::NodeCloud tiny{{pidc::Node{0, pidc::Vec2{0.5, 0.5}}}};
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
