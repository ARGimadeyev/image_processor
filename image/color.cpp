#include "color.h"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace {
constexpr double MaxC = 255.0;

inline uint8_t Convert(double color) {
    color = std::max(0.0, std::min(color, 1.0));
    return static_cast<uint8_t>(std::round(color * MaxC));
}
}  // namespace

Color::Color(uint8_t b, uint8_t g, uint8_t r) : B(b / MaxC), G(g / MaxC), R(r / MaxC) {
}

std::array<uint8_t, 3> Color::Out() const {
    uint8_t r = Convert(R);
    uint8_t g = Convert(G);
    uint8_t b = Convert(B);
    return {b, g, r};
}

Color Color::Norm() const {
    double b = std::max(0.0, std::min(B, 1.0));
    double g = std::max(0.0, std::min(G, 1.0));
    double r = std::max(0.0, std::min(R, 1.0));
    return {b, g, r};
}
