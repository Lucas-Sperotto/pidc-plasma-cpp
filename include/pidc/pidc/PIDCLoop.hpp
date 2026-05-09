#pragma once

#include <Eigen/Dense>

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/Particle.hpp"
#include "pidc/Species.hpp"
#include "pidc/Vec2.hpp"
#include "pidc/efg/EFGPoissonSolver.hpp"
#include "pidc/geometry/NodeCloud.hpp"
#include "pidc/mls/MLSConfig.hpp"
#include "pidc/pidc/ChargeDeposition.hpp"
#include "pidc/pidc/DiffuseCell.hpp"
#include "pidc/pidc/PIDCFieldInterpolator.hpp"

namespace pidc {

struct PIDCStepDiagnostics {
    double total_charge{0.0};
    double max_field_magnitude{0.0};
    bool all_finite{true};
    std::size_t particle_count{0};
};

inline Eigen::VectorXd pidc_rhs_from_charge(
    const Eigen::VectorXd& nodal_charge,
    double epsilon0)
{
    if (!(epsilon0 > 0.0) || !std::isfinite(epsilon0)) {
        throw std::invalid_argument{"pidc_rhs_from_charge requires epsilon0 > 0"};
    }
    if (!nodal_charge.allFinite()) {
        throw std::invalid_argument{"pidc_rhs_from_charge requires finite nodal charges"};
    }
    return nodal_charge / epsilon0;
}

inline void advance_particle_leapfrog_2d(
    Particle& particle,
    const Species& species,
    Vec2 electric_field,
    double dt)
{
    if (!(species.mass > 0.0) || !std::isfinite(species.mass) ||
        !std::isfinite(species.charge)) {
        throw std::invalid_argument{"advance_particle_leapfrog_2d requires finite species data"};
    }
    if (!std::isfinite(electric_field.x) || !std::isfinite(electric_field.y)) {
        throw std::invalid_argument{"advance_particle_leapfrog_2d requires finite field"};
    }
    if (!(dt >= 0.0) || !std::isfinite(dt)) {
        throw std::invalid_argument{"advance_particle_leapfrog_2d requires finite dt >= 0"};
    }

    const double q_over_m = species.charge / species.mass;
    particle.velocity.x += q_over_m * electric_field.x * dt;
    particle.velocity.y += q_over_m * electric_field.y * dt;
    particle.position.x += particle.velocity.x * dt;
    particle.position.y += particle.velocity.y * dt;
}

inline PIDCStepDiagnostics pidc_advance_one_step(
    std::vector<Particle>& particles,
    const std::vector<Species>& species_list,
    const NodeCloud& cloud,
    const EFGPoissonSolver& solver,
    const MLSConfig& mls_config,
    double epsilon0,
    double dt)
{
    const std::vector<DiffuseCell> cells = evaluate_diffuse_cells(cloud, particles, mls_config);
    const Eigen::VectorXd nodal_charge =
        deposit_charge_from_cells(cloud, particles, species_list, cells);
    const Eigen::VectorXd rhs = pidc_rhs_from_charge(nodal_charge, epsilon0);
    const Eigen::VectorXd potential = solver.solve(rhs);
    const std::vector<Vec2> fields = interpolate_field_pidc(cells, potential);

    PIDCStepDiagnostics diagnostics;
    diagnostics.total_charge = nodal_charge.sum();
    diagnostics.particle_count = particles.size();

    for (std::size_t i = 0; i < particles.size(); ++i) {
        if (particles[i].species_id >= species_list.size()) {
            throw std::out_of_range{"pidc_advance_one_step: particle species_id out of range"};
        }

        const double field_norm = std::sqrt(fields[i].x * fields[i].x + fields[i].y * fields[i].y);
        diagnostics.max_field_magnitude = std::max(diagnostics.max_field_magnitude, field_norm);

        advance_particle_leapfrog_2d(particles[i], species_list[particles[i].species_id], fields[i], dt);

        diagnostics.all_finite = diagnostics.all_finite &&
            std::isfinite(particles[i].position.x) &&
            std::isfinite(particles[i].position.y) &&
            std::isfinite(particles[i].velocity.x) &&
            std::isfinite(particles[i].velocity.y);
    }

    diagnostics.all_finite = diagnostics.all_finite &&
        std::isfinite(diagnostics.total_charge) &&
        std::isfinite(diagnostics.max_field_magnitude);

    if (!diagnostics.all_finite) {
        throw std::runtime_error{"pidc_advance_one_step produced non-finite diagnostics"};
    }

    return diagnostics;
}

} // namespace pidc
