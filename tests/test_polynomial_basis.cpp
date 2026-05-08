#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "pidc/mls/PolynomialBasis.hpp"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

bool approx_equal(double lhs, double rhs, double tol = 1.0e-15)
{
    return std::abs(lhs - rhs) < tol;
}

} // namespace

int main()
{
    // Caso 1: origem
    {
        const auto p = pidc::linear_basis({0.0, 0.0});
        require(approx_equal(p[0], 1.0), "linear_basis(origin)[0] must be 1.0");
        require(approx_equal(p[1], 0.0), "linear_basis(origin)[1] must be 0.0");
        require(approx_equal(p[2], 0.0), "linear_basis(origin)[2] must be 0.0");
    }

    // Caso 2: ponto geral
    {
        const auto p = pidc::linear_basis({1.5, -2.3});
        require(approx_equal(p[0], 1.0),  "linear_basis[0] must always be 1.0");
        require(approx_equal(p[1], 1.5),  "linear_basis[1] must equal x");
        require(approx_equal(p[2], -2.3), "linear_basis[2] must equal y");
    }

    // Caso 3: termo constante para múltiplos pontos
    for (const pidc::Vec2 pt : {pidc::Vec2{0.0, 0.0}, pidc::Vec2{3.7, -1.1}, pidc::Vec2{-0.5, 100.0}}) {
        require(approx_equal(pidc::linear_basis(pt)[0], 1.0),
                "linear_basis[0] must be 1.0 for any point");
    }

    // Caso 4: derivada em x
    {
        const auto dx = pidc::linear_basis_dx();
        require(dx.size() == 3,           "linear_basis_dx size must be 3");
        require(approx_equal(dx[0], 0.0), "linear_basis_dx[0] must be 0.0");
        require(approx_equal(dx[1], 1.0), "linear_basis_dx[1] must be 1.0");
        require(approx_equal(dx[2], 0.0), "linear_basis_dx[2] must be 0.0");
    }

    // Caso 5: derivada em y
    {
        const auto dy = pidc::linear_basis_dy();
        require(dy.size() == 3,           "linear_basis_dy size must be 3");
        require(approx_equal(dy[0], 0.0), "linear_basis_dy[0] must be 0.0");
        require(approx_equal(dy[1], 0.0), "linear_basis_dy[1] must be 0.0");
        require(approx_equal(dy[2], 1.0), "linear_basis_dy[2] must be 1.0");
    }

    // Caso 6: tamanho invariante de linear_basis
    {
        require(pidc::linear_basis({1.0, 2.0}).size() == 3,
                "linear_basis must always return array of size 3");
    }

    std::cout << "polynomial_basis test passed\n";
    return 0;
}
