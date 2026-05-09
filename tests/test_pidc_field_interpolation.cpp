#include <Eigen/Dense>

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "pidc/Domain2D.hpp"
#include "pidc/Particle.hpp"
#include "pidc/ShapeFunctionData.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/geometry/RegularNodeCloud2D.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/pidc/DiffuseCell.hpp"
#include "pidc/pidc/PIDCFieldInterpolator.hpp"
#include "test_utils.hpp"

namespace {

double linear_potential(pidc::Vec2 point)
{
    return 1.25 + 2.0 * point.x - 3.0 * point.y;
}

} // namespace

int main()
{
    using pidc::test::require;

    const pidc::Domain2D domain{{0.0, 0.0}, {1.0, 1.0}};
    const pidc::NodeCloud cloud = pidc::create_regular_node_cloud(domain, 5, 5);
    const pidc::MLSConfig mls{1.8 / 4.0};
    const std::vector<pidc::Particle> particles{
        pidc::Particle{{0.37, 0.61}, {}, 0},
        pidc::Particle{{0.62, 0.42}, {}, 0},
    };
    const std::vector<pidc::DiffuseCell> cells =
        pidc::evaluate_diffuse_cells(cloud, particles, mls);

    Eigen::VectorXd nodal_potential(static_cast<Eigen::Index>(cloud.size()));
    for (std::size_t i = 0; i < cloud.size(); ++i) {
        nodal_potential(static_cast<Eigen::Index>(i)) = linear_potential(cloud[i].position);
    }

    const pidc::Vec2 expected_field{-2.0, 3.0};
    for (const pidc::DiffuseCell& cell : cells) {
        const pidc::Vec2 field = pidc::interpolate_field_pidc(cell.shape, nodal_potential);
        require(std::abs(field.x - expected_field.x) < 1.0e-10,
                "PIDC interpolation must reproduce linear-potential Ex");
        require(std::abs(field.y - expected_field.y) < 1.0e-10,
                "PIDC interpolation must reproduce linear-potential Ey");
    }

    const std::vector<pidc::Vec2> vector_fields =
        pidc::interpolate_field_pidc(cells, nodal_potential);
    require(vector_fields.size() == particles.size(),
            "vector PIDC interpolation must return one field per particle");
    for (const pidc::Vec2 field : vector_fields) {
        require(std::abs(field.x - expected_field.x) < 1.0e-10,
                "vector PIDC interpolation must reproduce Ex");
        require(std::abs(field.y - expected_field.y) < 1.0e-10,
                "vector PIDC interpolation must reproduce Ey");
    }

    pidc::ShapeFunctionData invalid;
    invalid.neighbor_ids = {0};
    invalid.phi = {};
    invalid.grad_phi = {pidc::Vec2{0.0, 0.0}};
    bool invalid_shape_threw = false;
    try {
        (void)pidc::interpolate_field_pidc(invalid, nodal_potential);
    } catch (const std::invalid_argument&) {
        invalid_shape_threw = true;
    }
    require(invalid_shape_threw, "invalid ShapeFunctionData must throw");

    std::cout << "pidc_field_interpolation test passed\n";
    return 0;
}
