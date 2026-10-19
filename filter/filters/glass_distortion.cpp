#include <sys/types.h>
#include "../filter.h"

#include <algorithm>
#include <cstdint>
#include <random>
#include <utility>

namespace {
// constexpr double Blur = 6;
constexpr double BlurBack = 2;
constexpr int64_t Radius = 12;
constexpr int64_t Mul = 1;
constexpr int64_t HorizontalStep = 2 * Radius + 4;
constexpr int64_t VerticalStep = static_cast<int64_t>(HorizontalStep * 0.8660254037844386 + 0.5);

void InvertCircle(int64_t x0, int64_t y0, Image& new_image, const Image& image) {
    size_t x_begin = Clamp(x0, -Radius, image.Width());
    size_t x_end = Clamp(x0, Radius, image.Width());
    for (size_t x = x_begin; x <= x_end; ++x) {

        size_t y_begin = Clamp(y0, -Radius, image.Height());
        size_t y_end = Clamp(y0, Radius, image.Height());
        for (size_t y = y_begin; y <= y_end; ++y) {
            if (Radius * Radius < (x - x0) * (x - x0) + (y - y0) * (y - y0)) {
                continue;
            }
            const int64_t diff_x = (x0 - static_cast<int64_t>(x)) * (2 * Mul + 1);
            const int64_t diff_y = (y0 - static_cast<int64_t>(y)) * (2 * Mul + 1);
            new_image.At(x, y) = image.AverageMatrix(x0 + diff_x, y0 + diff_y, Mul);
        }
    }
}
}  // namespace

void GlassDistortion::Apply(Image& image) const {
    if (image.Width() == 0 || image.Height() == 0) {
        return;
    }
    // GaussianBlur{Blur}.Apply(image);
    Image result = image;
    // GaussianBlur{Blur + BlurBack}.Apply(image);

    std::mt19937 generator(1);
    std::uniform_int_distribution<int64_t> jitter(-4, 4);
    for (int64_t row = 0; row * VerticalStep < static_cast<int64_t>(image.Height()) + Radius; ++row) {
        const int64_t shift = row % 2 == 0 ? 0 : HorizontalStep / 2;
        for (int64_t column = 0; column * HorizontalStep + shift < static_cast<int64_t>(image.Width()) + Radius;
             ++column) {
            const int64_t center_x = std::max<int64_t>(0, column * HorizontalStep + shift + jitter(generator));
            const int64_t center_y = std::max<int64_t>(0, row * VerticalStep + jitter(generator));
            InvertCircle(center_x, center_y, result, image);
        }
    }
    image = std::move(result);
    GaussianBlur{BlurBack}.Apply(image);
}
