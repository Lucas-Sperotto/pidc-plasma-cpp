#include <Eigen/Dense>

#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/ShapeFunctionData.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/mls/MLSShapeFunction.hpp"
#include "pidc/mls/PolynomialBasis.hpp"
#include "pidc/mls/WeightFunction.hpp"
#include "test_utils.hpp"

namespace {

double moment_condition_number(
    pidc::Vec2 x,
    const pidc::NodeCloud& cloud,
    double support_radius)
{
    Eigen::Matrix3d A = Eigen::Matrix3d::Zero();

    for (std::size_t i = 0; i < cloud.size(); ++i) {
        const pidc::Vec2 xi = cloud[i].position;
        const double dx = x.x - xi.x;
        const double dy = x.y - xi.y;
        const double dist = std::sqrt(dx * dx + dy * dy);
        const double r = dist / support_radius;
        if (r >= 1.0) {
            continue;
        }

        const auto basis = pidc::linear_basis(xi);
        const Eigen::Vector3d p_i(basis[0], basis[1], basis[2]);
        A += pidc::weight_quartic(r) * p_i * p_i.transpose();
    }

    const Eigen::JacobiSVD<Eigen::Matrix3d> svd(A);
    const auto singular_values = svd.singularValues();
    return singular_values(0) / singular_values(2);
}

void require_finite_shape_data(const pidc::ShapeFunctionData& data)
{
    using pidc::test::require;

    require(pidc::is_valid(data), "ShapeFunctionData sizes must be consistent");
    require(!data.phi.empty(), "MLS evaluation must return at least one neighbour");

    for (std::size_t i = 0; i < data.phi.size(); ++i) {
        require(std::isfinite(data.phi[i]), "phi must be finite");
        require(std::isfinite(data.grad_phi[i].x), "grad_phi.x must be finite");
        require(std::isfinite(data.grad_phi[i].y), "grad_phi.y must be finite");
    }
}

void require_invalid_radius_throws(
    const pidc::NodeCloud& cloud,
    double support_radius,
    const char* message)
{
    using pidc::test::require;

    bool threw = false;
    try {
        (void)pidc::mls_evaluate(pidc::Vec2{0.5, 0.5}, cloud, support_radius);
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    require(threw, message);
}

} // namespace

int main()
{
    using pidc::test::require;

    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
    const pidc::NodeCloud cloud = pidc::create_regular_node_cloud(domain, 5, 5);
    const double support_radius = 0.6;
    const double max_condition_number = 1.0e8;

    const std::vector<pidc::Vec2> query_points{
        {0.5, 0.5},
        {0.3, 0.7},
        {0.6, 0.4},
        {0.37, 0.61},
    };

    for (const pidc::Vec2 point : query_points) {
        const pidc::ShapeFunctionData data = pidc::mls_evaluate(point, cloud, support_radius);
        require_finite_shape_data(data);

        const double condition_number = moment_condition_number(point, cloud, support_radius);
        require(std::isfinite(condition_number), "MLS moment condition number must be finite");
        require(condition_number < max_condition_number, "MLS moment condition number must stay below threshold");
    }

    const pidc::ShapeFunctionData by_radius =
        pidc::mls_evaluate(pidc::Vec2{0.37, 0.61}, cloud, support_radius);
    const pidc::ShapeFunctionData by_config =
        pidc::mls_evaluate(pidc::Vec2{0.37, 0.61}, cloud, pidc::MLSConfig{support_radius});
    require(by_radius.neighbor_ids == by_config.neighbor_ids, "MLSConfig overload must preserve neighbours");
    require(by_radius.phi.size() == by_config.phi.size(), "MLSConfig overload must preserve phi size");
    for (std::size_t i = 0; i < by_radius.phi.size(); ++i) {
        require(std::abs(by_radius.phi[i] - by_config.phi[i]) < 1.0e-14,
                "MLSConfig overload must preserve phi values");
    }

    require_invalid_radius_throws(cloud, 0.0, "zero support radius must throw invalid_argument");
    require_invalid_radius_throws(cloud, -1.0, "negative support radius must throw invalid_argument");
    require_invalid_radius_throws(
        cloud,
        std::numeric_limits<double>::infinity(),
        "infinite support radius must throw invalid_argument");
    require_invalid_radius_throws(
        cloud,
        std::numeric_limits<double>::quiet_NaN(),
        "NaN support radius must throw invalid_argument");

    std::cout << "mls_robustness test passed\n";
    return 0;
}
