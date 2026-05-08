#include <cstdlib>
#include <iostream>
#include <string>

#include <Eigen/Dense>

#include "pidc/Vec2.hpp"

namespace {

void require(bool condition, const std::string& message)
{
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

bool approx_equal(double lhs, double rhs)
{
    const double tolerance = 1.0e-12;
    const double diff = lhs - rhs;
    return diff < tolerance && diff > -tolerance;
}

} // namespace

int main()
{
    const Eigen::Matrix2d matrix = (Eigen::Matrix2d{} << 2.0, 0.0, 0.0, 4.0).finished();
    const Eigen::Vector2d rhs{6.0, 8.0};
    const Eigen::Vector2d solution = matrix.ldlt().solve(rhs);

    require(approx_equal(solution.x(), 3.0), "Eigen solve must compute x component");
    require(approx_equal(solution.y(), 2.0), "Eigen solve must compute y component");

    const pidc::Vec2 point{solution.x(), solution.y()};
    require(approx_equal(point.x, 3.0), "Eigen consumer must still see pidc headers");
    require(approx_equal(point.y, 2.0), "Eigen consumer must still use pidc headers");

    std::cout << "eigen_dependency test passed\n";
    return 0;
}
