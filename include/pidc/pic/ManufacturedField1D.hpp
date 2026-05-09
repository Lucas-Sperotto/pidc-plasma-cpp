#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

#include "pidc/pic/Grid1D.hpp"

namespace pidc::pic {

struct SineManufacturedField1D {
    double amplitude{1.0};
    double length{1.0};
    double epsilon0{1.0};

    double potential(double x) const
    {
        validate(x);
        return amplitude * std::sin(wavenumber() * periodic_coordinate(x));
    }

    double electric_field(double x) const
    {
        validate(x);
        const double k = wavenumber();
        return -amplitude * k * std::cos(k * periodic_coordinate(x));
    }

    double charge_density(double x) const
    {
        validate(x);
        const double k = wavenumber();
        return epsilon0 * k * k * amplitude * std::sin(k * periodic_coordinate(x));
    }

private:
    static constexpr double pi() noexcept
    {
        return 3.141592653589793238462643383279502884;
    }

    void validate(double x) const
    {
        if (!std::isfinite(amplitude) || !std::isfinite(length) || !std::isfinite(epsilon0) ||
            !std::isfinite(x)) {
            throw std::invalid_argument{"SineManufacturedField1D requires finite inputs"};
        }

        if (!(length > 0.0)) {
            throw std::invalid_argument{"SineManufacturedField1D requires length > 0"};
        }

        if (!(epsilon0 > 0.0)) {
            throw std::invalid_argument{"SineManufacturedField1D requires epsilon0 > 0"};
        }
    }

    double wavenumber() const noexcept
    {
        return 2.0 * pi() / length;
    }

    double periodic_coordinate(double x) const noexcept
    {
        const double wrapped = x - length * std::floor(x / length);
        return wrapped >= length ? 0.0 : wrapped;
    }
};

inline std::vector<double> sample_potential(
    const Grid1D& grid,
    const SineManufacturedField1D& field)
{
    std::vector<double> values;
    values.reserve(grid.size());

    for (std::size_t i = 0; i < grid.size(); ++i) {
        values.push_back(field.potential(grid.coordinate(i)));
    }

    return values;
}

inline std::vector<double> sample_electric_field(
    const Grid1D& grid,
    const SineManufacturedField1D& field)
{
    std::vector<double> values;
    values.reserve(grid.size());

    for (std::size_t i = 0; i < grid.size(); ++i) {
        values.push_back(field.electric_field(grid.coordinate(i)));
    }

    return values;
}

inline std::vector<double> sample_charge_density(
    const Grid1D& grid,
    const SineManufacturedField1D& field)
{
    std::vector<double> values;
    values.reserve(grid.size());

    for (std::size_t i = 0; i < grid.size(); ++i) {
        values.push_back(field.charge_density(grid.coordinate(i)));
    }

    return values;
}

} // namespace pidc::pic
