#pragma once

#include <cstdint>

constexpr double MaxC = 255.0;
struct Color {
    double B, G, R;

    Color(uint8_t r, uint8_t g, uint8_t b) : B(b / MaxC), G(g / MaxC), R(r / MaxC) {
    }
};
