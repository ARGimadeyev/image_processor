#include "../filter.h"
#include "../../image/image.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <random>
#include <utility>

namespace {
constexpr int64_t Mul = 2;

void InvertCircle(int64_t x0, int64_t y0, Image& new_image, const Image& image, int64_t radius) {
    auto x_begin = Clamp<int64_t>(x0, -radius, image.Width());
    auto x_end = Clamp<int64_t>(x0, radius, image.Width());
    for (auto x = x_begin; x <= x_end; ++x) {

        auto y_begin = Clamp<int64_t>(y0, -radius, image.Height());
        auto y_end = Clamp<int64_t>(y0, radius, image.Height());
        for (auto y = y_begin; y <= y_end; ++y) {
            if (radius * radius < (x - x0) * (x - x0) + (y - y0) * (y - y0)) {
                continue;
            }
            const auto diff_x = (x0 - x) * (2 * Mul + 1);
            const auto diff_y = (y0 - y) * (2 * Mul + 1);
            new_image.At(x, y) = image.AverageMatrix(x0 + diff_x, y0 + diff_y, Mul);
        }
    }
}
}  // namespace

void GlassDistortion::Apply(Image& image) const {
    const int64_t hstep = 2 * radius_ + 2;
    const int64_t vstep = static_cast<int64_t>(static_cast<double>(hstep) * 0.8660254037844386 + 0.5);
    // GaussianBlur{Blur}.Apply(image);
    Image result = image;
    GaussianBlur{blur_ * 2}.Apply(result);

    std::mt19937 generator(1);
    std::uniform_int_distribution<int64_t> jitter(-4, 4);
    for (int64_t row = 0; row * vstep < (static_cast<int64_t>(image.Height()) + radius_); ++row) {
        const int64_t shift = row % 2 == 0 ? 0 : hstep / 2;
        for (int64_t column = 0; column * hstep + shift < static_cast<int64_t>(image.Width()) + radius_; ++column) {
            const auto center_x = std::max<int64_t>(0, column * hstep + shift + jitter(generator));
            const auto center_y = std::max<int64_t>(0, row * vstep + jitter(generator));
            InvertCircle(center_x, center_y, result, image, radius_);
        }
    }
    image = std::move(result);
    GaussianBlur{blur_}.Apply(image);
}
