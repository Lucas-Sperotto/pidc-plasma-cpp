#pragma once

#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace pidc::pic {

struct Grid1D {
    double xmin{0.0};
    double xmax{1.0};
    std::size_t nx{1};
    double dx{1.0};

    Grid1D(double xmin_value, double xmax_value, std::size_t nx_value)
        : xmin{xmin_value},
          xmax{xmax_value},
          nx{nx_value},
          dx{compute_dx(xmin_value, xmax_value, nx_value)}
    {
    }

    std::size_t size() const noexcept
    {
        return nx;
    }

    double coordinate(std::size_t i) const
    {
        if (i >= nx) {
            throw std::out_of_range{"Grid1D coordinate index out of range"};
        }

        return xmin + static_cast<double>(i) * dx;
    }

    double wrap_position(double x) const
    {
        validate_position(x);

        const double length = xmax - xmin;
        const double shifted = x - xmin;
        double wrapped = shifted - length * std::floor(shifted / length);

        if (wrapped >= length) {
            wrapped = 0.0;
        }

        return xmin + wrapped;
    }

    std::size_t cell_index(double x) const
    {
        const double wrapped = wrap_position(x);
        const double raw = std::floor((wrapped - xmin) / dx);

        if (raw < 0.0) {
            return 0;
        }

        const auto index = static_cast<std::size_t>(raw);
        return index < nx ? index : 0;
    }

    std::size_t left_node_index(double x) const
    {
        return cell_index(x);
    }

    std::size_t right_node_index(double x) const
    {
        return (left_node_index(x) + 1) % nx;
    }

    double fraction_in_cell(double x) const
    {
        const double wrapped = wrap_position(x);
        const std::size_t left = left_node_index(wrapped);
        const double fraction = (wrapped - coordinate_unchecked(left)) / dx;

        if (fraction < 0.0) {
            return 0.0;
        }
        if (fraction >= 1.0) {
            return 0.0;
        }

        return fraction;
    }

private:
    static double compute_dx(double xmin_value, double xmax_value, std::size_t nx_value)
    {
        if (!std::isfinite(xmin_value) || !std::isfinite(xmax_value) || !(xmax_value > xmin_value)) {
            throw std::invalid_argument{"Grid1D requires finite bounds with xmax > xmin"};
        }

        if (nx_value == 0) {
            throw std::invalid_argument{"Grid1D requires nx > 0"};
        }

        const double dx_value = (xmax_value - xmin_value) / static_cast<double>(nx_value);
        if (!(dx_value > 0.0) || !std::isfinite(dx_value)) {
            throw std::invalid_argument{"Grid1D requires a positive finite dx"};
        }

        return dx_value;
    }

    static void validate_position(double x)
    {
        if (!std::isfinite(x)) {
            throw std::invalid_argument{"Grid1D position must be finite"};
        }
    }

    double coordinate_unchecked(std::size_t i) const noexcept
    {
        return xmin + static_cast<double>(i) * dx;
    }
};

} // namespace pidc::pic
