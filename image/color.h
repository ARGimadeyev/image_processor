#pragma once

#include <cstdint>
#include <array>

struct Color {
    double B, G, R;
    Color() : B(0), G(0), R(0) {
    }
    Color(uint8_t r, uint8_t g, uint8_t b);

    std::array<uint8_t, 3> Out() const;
};
