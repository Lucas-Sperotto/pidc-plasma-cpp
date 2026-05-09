#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

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
    // Assembles K and b from Gauss integration, applies Dirichlet penalty, and
    // caches the LDLT factorization. For MMS tests and any single-solve use case.
    void assemble(
        const NodeCloud& nodes,
        const Domain2D& domain,
        const std::vector<GaussCell2D>& cells,
        const MLSConfig& mls,
        ScalarField2D source,
        ScalarField2D dirichlet)
    {
        assembled_ = false;
        factorized_ = false;

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
        std::vector<Eigen::Triplet<double>> stiffness_entries;
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
                        stiffness_entries.emplace_back(row, col, quadrature.weight * grad_dot);
                    }
                }
            }
        }

        rebuild_stiffness(n, stiffness_entries);
        impose_dirichlet_penalty(nodes, domain, dirichlet, stiffness_entries);

        if (!sparse_matrix_all_finite(stiffness_) || !rhs_.allFinite()) {
            throw std::runtime_error{"EFGPoissonSolver assembled non-finite matrix or rhs"};
        }

        cache_factorization();
        assembled_ = true;
    }

    // DEC-0031: Assembles K only (no source integral), applies Dirichlet penalty to K,
    // and caches the LDLT factorization. For the PIDC loop where b changes every step
    // (b comes from deposit_charge) while K stays constant.
    // After this call, use solve(rhs) — not solve() — to provide the external RHS.
    void assemble_stiffness_only(
        const NodeCloud& nodes,
        const Domain2D& domain,
        const std::vector<GaussCell2D>& cells,
        const MLSConfig& mls,
        const ScalarField2D& dirichlet)
    {
        assembled_ = false;
        factorized_ = false;

        if (nodes.size() == 0) {
            throw std::invalid_argument{"EFGPoissonSolver requires at least one node"};
        }
        if (cells.empty()) {
            throw std::invalid_argument{"EFGPoissonSolver requires at least one integration cell"};
        }
        if (!dirichlet) {
            throw std::invalid_argument{"EFGPoissonSolver requires a Dirichlet field"};
        }

        validate_mls_config(mls);

        const auto n = static_cast<Eigen::Index>(nodes.size());
        std::vector<Eigen::Triplet<double>> stiffness_entries;
        rhs_ = Eigen::VectorXd::Zero(n);

        for (const GaussCell2D& cell : cells) {
            for (const GaussPoint2D& quadrature : cell.points) {
                const ShapeFunctionData shape = mls_evaluate(quadrature.point, nodes, mls);

                for (std::size_t a = 0; a < shape.neighbor_ids.size(); ++a) {
                    for (std::size_t b = 0; b < shape.neighbor_ids.size(); ++b) {
                        const auto row = static_cast<Eigen::Index>(shape.neighbor_ids[a]);
                        const auto col = static_cast<Eigen::Index>(shape.neighbor_ids[b]);
                        const double grad_dot =
                            shape.grad_phi[a].x * shape.grad_phi[b].x +
                            shape.grad_phi[a].y * shape.grad_phi[b].y;
                        stiffness_entries.emplace_back(row, col, quadrature.weight * grad_dot);
                    }
                }
            }
        }

        rebuild_stiffness(n, stiffness_entries);
        impose_dirichlet_penalty(nodes, domain, dirichlet, stiffness_entries);

        if (!sparse_matrix_all_finite(stiffness_)) {
            throw std::runtime_error{"EFGPoissonSolver assembled non-finite stiffness matrix"};
        }

        cache_factorization();
        // assembled_ stays false: rhs_ was not built from a source integral
    }

    // Solves K u = b using the cached RHS and factorization (after assemble()).
    Eigen::VectorXd solve() const
    {
        if (!assembled_) {
            throw std::runtime_error{"EFGPoissonSolver solve called before assemble"};
        }
        return solve_with_cached_factorization(rhs_);
    }

    // DEC-0031: Solves K u = rhs using the cached factorization and a caller-provided RHS.
    // Intended for the PIDC time loop: rhs = Q_nodal / epsilon0 (from deposit_charge).
    // Requires assemble() or assemble_stiffness_only() to have been called first.
    Eigen::VectorXd solve(const Eigen::VectorXd& rhs) const
    {
        if (!factorized_) {
            throw std::runtime_error{
                "EFGPoissonSolver solve(rhs) called before assemble or assemble_stiffness_only"};
        }
        return solve_with_cached_factorization(rhs);
    }

    Eigen::MatrixXd stiffness_matrix() const
    {
        return Eigen::MatrixXd{stiffness_};
    }

    const Eigen::VectorXd& rhs() const noexcept
    {
        return rhs_;
    }

private:
    Eigen::SparseMatrix<double> stiffness_{};
    Eigen::VectorXd rhs_{};
    // DEC-0031: cached factorization — computed once per assemble*() call,
    // reused across all solve(rhs) calls in the PIDC time loop.
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> ldlt_{};
    bool assembled_{false};
    bool factorized_{false};

    void cache_factorization()
    {
        ldlt_.compute(stiffness_);
        if (ldlt_.info() != Eigen::Success) {
            throw std::runtime_error{"EFGPoissonSolver stiffness matrix factorization failed"};
        }
        factorized_ = true;
    }

    Eigen::VectorXd solve_with_cached_factorization(const Eigen::VectorXd& rhs) const
    {
        Eigen::VectorXd solution = ldlt_.solve(rhs);
        if (ldlt_.info() != Eigen::Success) {
            throw std::runtime_error{"EFGPoissonSolver sparse solve failed"};
        }
        if (!solution.allFinite()) {
            throw std::runtime_error{"EFGPoissonSolver produced a non-finite solution"};
        }
        return solution;
    }

    static void require_finite(double value, const char* message)
    {
        if (!std::isfinite(value)) {
            throw std::runtime_error{message};
        }
    }

    static bool sparse_matrix_all_finite(const Eigen::SparseMatrix<double>& matrix) noexcept
    {
        for (Eigen::Index outer = 0; outer < matrix.outerSize(); ++outer) {
            for (Eigen::SparseMatrix<double>::InnerIterator entry(matrix, outer); entry; ++entry) {
                if (!std::isfinite(entry.value())) {
                    return false;
                }
            }
        }
        return true;
    }

    void rebuild_stiffness(
        Eigen::Index node_count,
        const std::vector<Eigen::Triplet<double>>& stiffness_entries)
    {
        stiffness_.resize(node_count, node_count);
        stiffness_.setZero();
        stiffness_.setFromTriplets(stiffness_entries.begin(), stiffness_entries.end());
        stiffness_.makeCompressed();
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
        const ScalarField2D& dirichlet,
        std::vector<Eigen::Triplet<double>>& stiffness_entries)
    {
        double max_diag = 0.0;
        for (Eigen::Index i = 0; i < stiffness_.rows(); ++i) {
            max_diag = std::max(max_diag, std::abs(stiffness_.coeff(i, i)));
        }
        const double penalty  = 1.0e12 * std::max(max_diag, 1.0);

        for (std::size_t i = 0; i < nodes.size(); ++i) {
            const Vec2 position = nodes[i].position;
            if (!is_boundary_node(position, domain)) {
                continue;
            }

            const double value = dirichlet(position);
            require_finite(value, "EFGPoissonSolver Dirichlet field returned a non-finite value");

            const auto index = static_cast<Eigen::Index>(i);
            stiffness_entries.emplace_back(index, index, penalty);
            rhs_(index) += penalty * value;
        }

        rebuild_stiffness(static_cast<Eigen::Index>(nodes.size()), stiffness_entries);
    }
};

} // namespace pidc
