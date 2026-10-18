#pragma once

#include <cstdint>
#include <array>

struct Color {
    double B, G, R;
    Color() : B(0), G(0), R(0) {
    }
    Color(double b, double g, double r) : B(b), G(g), R(r) {
    }
    Color(uint8_t b, uint8_t g, uint8_t r);

    Color Norm() const;

    std::array<uint8_t, 3> Out() const;

    void operator+=(const Color& c) {
        B += c.B;
        G += c.G;
        R += c.R;
    }
    Color operator*(const int32_t a) const {
        return {B * a, G * a, R * a};
    }
    Color operator*(const double a) const {
        return {B * a, G * a, R * a};
    }
};
