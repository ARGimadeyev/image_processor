#pragma once

#include <algorithm>
#include <cstddef>
#include <concepts>
#include <cstdint>
#include <vector>
#include "color.h"

class Image {
public:
    Image(size_t height, size_t width) : height_(height), width_(width), picture_(width * height, Color()) {
    }
    size_t Height() const;

    size_t Width() const;

    Color& At(size_t x, size_t y);

    const Color& At(size_t x, size_t y) const;

    const Color GetMatrix(const std::vector<std::vector<int32_t>>& matrix, size_t x, size_t y) const;

    const Color AverageMatrix(int64_t x0, int64_t y0, int64_t mul) const;

private:
    size_t height_;
    size_t width_;
    std::vector<Color> picture_;
};

template <class T = std::size_t>
    requires std::integral<T>
T Clamp(size_t coordinate, int64_t offset, size_t length) {
    const int64_t cord = static_cast<int64_t>(coordinate) + offset;
    const int64_t len = static_cast<int64_t>(length);
    const int64_t answer = std::clamp<int64_t>(cord, 0, len - 1);
    return static_cast<T>(answer);
}
