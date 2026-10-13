#include "image.h"
#include <cstddef>
#include "color.h"

size_t Image::Height() const {
    return height_;
}

size_t Image::Width() const {
    return width_;
}

Color& Image::At(size_t x, size_t y) {
    return picture_[y * width_ + x];
}

const Color& Image::At(size_t x, size_t y) const {
    return picture_[y * width_ + x];
}
