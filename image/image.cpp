#include "image.h"
#include <cstddef>
#include <cstdint>
#include <vector>

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

const Color Image::GetMatrix(const std::vector<std::vector<int32_t>>& matrix, size_t x, size_t y) const {
    Color sum;
    for (int64_t i = 0; i < 3; ++i) {
        const size_t new_y = Clamp<size_t>(y, i - 1, Height());
        for (int64_t j = 0; j < 3; ++j) {
            const size_t new_x = Clamp<size_t>(x, j - 1, Width());
            sum += At(new_x, new_y) * matrix[i][j];
        }
    }
    return sum;
}

const Color Image::AverageMatrix(int64_t x0, int64_t y0, int64_t mul) const {
    Color average;
    for (int64_t dx = -mul; dx <= mul; ++dx) {
        for (int64_t dy = -mul; dy <= mul; ++dy) {
            const int64_t x = x0 + dx;
            const int64_t y = y0 + dy;
            if (x < 0 || x >= static_cast<int64_t>(Width()) || y < 0 || y >= static_cast<int64_t>(Height())) {
                average += Color(1.0, 1.0, 1.0);
            } else {
                average += At(static_cast<size_t>(x), static_cast<size_t>(y));
            }
        }
    }
    average = average * (1.0 / static_cast<double>((2 * mul + 1) * (2 * mul + 1)));
    return average;
}
