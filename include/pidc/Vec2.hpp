#pragma once

namespace pidc {

struct Vec2 {
    double x{0.0};
    double y{0.0};

    constexpr Vec2() = default;
    constexpr Vec2(double x_value, double y_value) : x{x_value}, y{y_value} {}

    constexpr Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vec2& operator*=(double scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }
};

constexpr Vec2 operator+(Vec2 lhs, const Vec2& rhs)
{
    lhs += rhs;
    return lhs;
}

constexpr Vec2 operator-(Vec2 lhs, const Vec2& rhs)
{
    lhs -= rhs;
    return lhs;
}

constexpr Vec2 operator*(Vec2 value, double scalar)
{
    value *= scalar;
    return value;
}

constexpr Vec2 operator*(double scalar, Vec2 value)
{
    value *= scalar;
    return value;
}

} // namespace pidc
