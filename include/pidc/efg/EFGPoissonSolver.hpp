#pragma once

#include <Eigen/Dense>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/efg/GaussCell2D.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/mls/MLSShapeFunction.hpp"

namespace pidc {

using ScalarField2D = std::function<double(Vec2)>;

class EFGPoissonSolver {
public:
    void assemble(
        const NodeCloud& nodes,
        const Domain2D& domain,
        const std::vector<GaussCell2D>& cells,
        const MLSConfig& mls,
        ScalarField2D source,
        ScalarField2D dirichlet)
    {
        if (nodes.size() == 0) {
            throw std::invalid_argument{"EFGPoissonSolver requires at least one node"};
        }
        if (cells.empty()) {
            throw std::invalid_argument{"EFGPoissonSolver requires at least one integration cell"};
        }
        if (!source || !dirichlet) {
            throw std::invalid_argument{"EFGPoissonSolver requires source and Dirichlet fields"};
        }

        validate_mls_config(mls);

        const auto n = static_cast<Eigen::Index>(nodes.size());
        stiffness_ = Eigen::MatrixXd::Zero(n, n);
        rhs_ = Eigen::VectorXd::Zero(n);

        for (const GaussCell2D& cell : cells) {
            for (const GaussPoint2D& quadrature : cell.points) {
                const ShapeFunctionData shape = mls_evaluate(quadrature.point, nodes, mls);
                const double f = source(quadrature.point);
                require_finite(f, "EFGPoissonSolver source returned a non-finite value");

                for (std::size_t a = 0; a < shape.neighbor_ids.size(); ++a) {
                    const auto row = static_cast<Eigen::Index>(shape.neighbor_ids[a]);
                    rhs_(row) += quadrature.weight * shape.phi[a] * f;

                    for (std::size_t b = 0; b < shape.neighbor_ids.size(); ++b) {
                        const auto col = static_cast<Eigen::Index>(shape.neighbor_ids[b]);
                        const double grad_dot =
                            shape.grad_phi[a].x * shape.grad_phi[b].x +
                            shape.grad_phi[a].y * shape.grad_phi[b].y;
                        stiffness_(row, col) += quadrature.weight * grad_dot;
                    }
                }
            }
        }

        impose_dirichlet_penalty(nodes, domain, dirichlet);

        if (!stiffness_.allFinite() || !rhs_.allFinite()) {
            throw std::runtime_error{"EFGPoissonSolver assembled non-finite matrix or rhs"};
        }

        assembled_ = true;
    }

    Eigen::VectorXd solve() const
    {
        if (!assembled_) {
            throw std::runtime_error{"EFGPoissonSolver solve called before assemble"};
        }

        Eigen::FullPivLU<Eigen::MatrixXd> lu(stiffness_);
        if (!lu.isInvertible()) {
            throw std::runtime_error{"EFGPoissonSolver stiffness matrix is singular"};
        }

        Eigen::VectorXd solution = lu.solve(rhs_);
        if (!solution.allFinite()) {
            throw std::runtime_error{"EFGPoissonSolver produced a non-finite solution"};
        }

        return solution;
    }

    const Eigen::MatrixXd& stiffness_matrix() const noexcept
    {
        return stiffness_;
    }

    const Eigen::VectorXd& rhs() const noexcept
    {
        return rhs_;
    }

private:
    Eigen::MatrixXd stiffness_{};
    Eigen::VectorXd rhs_{};
    bool assembled_{false};

    static void require_finite(double value, const char* message)
    {
        if (!std::isfinite(value)) {
            throw std::runtime_error{message};
        }
    }

    static bool is_boundary_node(Vec2 point, const Domain2D& domain) noexcept
    {
        const double scale = std::max(1.0, std::max(domain.width(), domain.height()));
        const double tolerance = 1.0e-12 * scale;

        return std::abs(point.x - domain.lower().x) <= tolerance ||
               std::abs(point.x - domain.upper().x) <= tolerance ||
               std::abs(point.y - domain.lower().y) <= tolerance ||
               std::abs(point.y - domain.upper().y) <= tolerance;
    }

    // DEC-0024: penalty method for Dirichlet BCs — preserves symmetry and
    // handles homogeneous/non-homogeneous cases identically. For non-homogeneous
    // Dirichlet with multiple boundary nodes, this is also correct (unlike the
    // row/column elimination approach, which has order-dependence when g_i != 0).
    // Future sparse migration (DEC-0025): only the diagonal is touched, so
    // Eigen::SparseMatrix + SimplicialLDLT can be used without structural changes.
    void impose_dirichlet_penalty(
        const NodeCloud& nodes,
        const Domain2D& domain,
        const ScalarField2D& dirichlet)
    {
        const double max_diag = stiffness_.diagonal().cwiseAbs().maxCoeff();
        const double penalty  = 1.0e12 * std::max(max_diag, 1.0);

        for (std::size_t i = 0; i < nodes.size(); ++i) {
            const Vec2 position = nodes[i].position;
            if (!is_boundary_node(position, domain)) {
                continue;
            }

            const double value = dirichlet(position);
            require_finite(value, "EFGPoissonSolver Dirichlet field returned a non-finite value");

            const auto index = static_cast<Eigen::Index>(i);
            stiffness_(index, index) += penalty;
            rhs_(index)              += penalty * value;
        }
    }
};

} // namespace pidc
