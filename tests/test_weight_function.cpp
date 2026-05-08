#include <iostream>

#include "pidc/mls/WeightFunction.hpp"
#include "test_utils.hpp"

int main()
{
    using pidc::test::approx_equal;
    using pidc::test::require;

    // Valores na fronteira e fora do suporte
    require(approx_equal(pidc::weight_quartic(0.0), 1.0),
            "weight_quartic(0.0) must be 1.0");
    require(approx_equal(pidc::weight_quartic(1.0), 0.0),
            "weight_quartic(1.0) must be 0.0");
    require(approx_equal(pidc::weight_quartic(1.5), 0.0),
            "weight_quartic(1.5) must be 0.0 (outside support)");
    require(approx_equal(pidc::weight_quartic(-0.1), 1.0),
            "weight_quartic(-0.1) must be 1.0 (negative radius guard)");

    // Valor analítico no ponto médio: 1 - 6(0.25) + 8(0.125) - 3(0.0625) = 0.3125
    require(approx_equal(pidc::weight_quartic(0.5), 0.3125),
            "weight_quartic(0.5) must be 0.3125");

    // Não-negatividade em amostras do interior
    for (int k = 0; k <= 10; ++k) {
        const double r = k * 0.1;
        require(pidc::weight_quartic(r) >= 0.0,
                "weight_quartic must be non-negative on [0, 1]");
    }

    // Derivada nas fronteiras
    require(approx_equal(pidc::weight_quartic_deriv(0.0), 0.0),
            "weight_quartic_deriv(0.0) must be 0.0 (flat at center)");
    require(approx_equal(pidc::weight_quartic_deriv(1.0), 0.0),
            "weight_quartic_deriv(1.0) must be 0.0 (C1 at boundary)");
    require(approx_equal(pidc::weight_quartic_deriv(1.5), 0.0),
            "weight_quartic_deriv(1.5) must be 0.0 (outside support)");

    // Monotonicidade: derivada negativa no interior (0, 1)
    for (int k = 1; k <= 9; ++k) {
        const double r = k * 0.1;
        require(pidc::weight_quartic_deriv(r) <= 0.0,
                "weight_quartic_deriv must be non-positive on (0, 1)");
    }

    std::cout << "weight_function test passed\n";
    return 0;
}
