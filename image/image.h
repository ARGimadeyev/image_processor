#pragma once

#include <cstddef>
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

private:
    size_t height_;
    size_t width_;
    std::vector<Color> picture_;
};
