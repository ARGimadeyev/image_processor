#pragma once

#include <cstddef>
#include <vector>
#include "color.h"

class Image {
public:
    size_t Height() const;
    size_t Width() const;
    Color& At(size_t x, size_t y);
    const Color& At(size_t x, size_t y) const;

private:
    size_t height_;
    size_t width_;
    std::vector<Color> picture_;
};
