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

    std::array<uint8_t, 3> Out() const;
};
