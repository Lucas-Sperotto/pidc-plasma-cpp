#pragma once

#include <Eigen/Dense>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "pidc/ShapeFunctionData.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/mls/PolynomialBasis.hpp"
#include "pidc/mls/WeightFunction.hpp"

namespace pidc {

inline ShapeFunctionData mls_evaluate(
    Vec2 x,
    const NodeCloud& cloud,
    const MLSConfig& config)
{
    validate_mls_config(config);
    const double support_radius = config.support_radius;

    // pass 1: collect neighbours inside support
    std::vector<std::size_t> ids;
    std::vector<double>      w, dw_dx, dw_dy;

    for (std::size_t i = 0; i < cloud.size(); ++i) {
        const Vec2   xi   = cloud[i].position;
        const double dx   = x.x - xi.x;
        const double dy   = x.y - xi.y;
        const double dist = std::sqrt(dx*dx + dy*dy);
        const double r    = dist / support_radius;
        if (r >= 1.0) continue;

        ids.push_back(i);
        w.push_back(weight_quartic(r));

        if (dist > 1.0e-14) {
            const double coeff = weight_quartic_deriv(r) / (support_radius * dist);
            dw_dx.push_back(coeff * dx);
            dw_dy.push_back(coeff * dy);
        } else {
            dw_dx.push_back(0.0);
            dw_dy.push_back(0.0);
        }
    }

    const std::size_t n = ids.size();
    if (n < 3)
        throw std::runtime_error("mls_evaluate: fewer than 3 neighbours in support");

    // pass 2: moment matrix A and its spatial derivatives dA/dx, dA/dy
    Eigen::Matrix3d A     = Eigen::Matrix3d::Zero();
    Eigen::Matrix3d dA_dx = Eigen::Matrix3d::Zero();
    Eigen::Matrix3d dA_dy = Eigen::Matrix3d::Zero();

    for (std::size_t k = 0; k < n; ++k) {
        const auto arr = linear_basis(cloud[ids[k]].position);
        const Eigen::Vector3d pk(arr[0], arr[1], arr[2]);
        A     += w[k]     * pk * pk.transpose();
        dA_dx += dw_dx[k] * pk * pk.transpose();
        dA_dy += dw_dy[k] * pk * pk.transpose();
    }

    // p(x) and its constant derivatives (linear basis has constant gradient)
    const auto px_arr  = linear_basis(x);
    const auto dpx_arr = linear_basis_dx();
    const auto dpy_arr = linear_basis_dy();
    const Eigen::Vector3d px (px_arr[0],  px_arr[1],  px_arr[2]);
    const Eigen::Vector3d dpx(dpx_arr[0], dpx_arr[1], dpx_arr[2]);
    const Eigen::Vector3d dpy(dpy_arr[0], dpy_arr[1], dpy_arr[2]);

    // solve A*c = p(x); reuse LU factorisation for gradient RHS
    // A*dc/dxk = dp/dxk - dA/dxk * c  (from differentiating A*c = p(x))
    const Eigen::PartialPivLU<Eigen::Matrix3d> lu(A);
    const Eigen::Vector3d c     = lu.solve(px);
    const Eigen::Vector3d dc_dx = lu.solve(dpx - dA_dx * c);
    const Eigen::Vector3d dc_dy = lu.solve(dpy - dA_dy * c);

    ShapeFunctionData result;
    result.neighbor_ids.resize(n);
    result.phi.resize(n);
    result.grad_phi.resize(n);

    for (std::size_t k = 0; k < n; ++k) {
        const auto arr = linear_basis(cloud[ids[k]].position);
        const Eigen::Vector3d pki(arr[0], arr[1], arr[2]);

        result.neighbor_ids[k] = ids[k];
        result.phi[k]          = c.dot(pki) * w[k];
        result.grad_phi[k]     = Vec2{
            dc_dx.dot(pki) * w[k] + c.dot(pki) * dw_dx[k],
            dc_dy.dot(pki) * w[k] + c.dot(pki) * dw_dy[k]
        };
    }

    return result;
}

inline ShapeFunctionData mls_evaluate(
    Vec2 x,
    const NodeCloud& cloud,
    double support_radius)
{
    return mls_evaluate(x, cloud, MLSConfig{support_radius});
}

} // namespace pidc
