#include "../filter.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

void GaussianBlur::Apply(Image& image) const {
    if (sigma_ == 0) {
        return;
    }
    const auto radius = static_cast<int64_t>(std::ceil(3.0 * sigma_));
    std::vector<double> weights(static_cast<size_t>(2 * radius + 1));
    double weight_sum = 0;
    for (int64_t offset = -radius; offset <= radius; ++offset) {
        const double distance = static_cast<double>(offset) / sigma_;
        const double weight = std::exp(-0.5 * distance * distance);
        const size_t indx = static_cast<size_t>(offset + radius);
        weights[indx] = weight;
        weight_sum += weight;
    }

    for (double& weight : weights) {
        weight /= weight_sum;
    }

    Image intermediate_value(image.Height(), image.Width());
    for (size_t y = 0; y < image.Height(); ++y) {
        for (size_t x = 0; x < image.Width(); ++x) {
            Color sum;
            for (int64_t offset = -radius; offset <= radius; ++offset) {
                const size_t neighbor_x = Clamp(x, offset, image.Width());
                const size_t indx = static_cast<size_t>(offset + radius);
                sum += image.At(neighbor_x, y) * weights[indx];
            }
            intermediate_value.At(x, y) = sum;
        }
    }

    for (size_t y = 0; y < image.Height(); ++y) {
        for (size_t x = 0; x < image.Width(); ++x) {
            Color sum;
            for (int64_t offset = -radius; offset <= radius; ++offset) {
                const size_t neighbor_y = Clamp(y, offset, image.Height());
                const size_t indx = static_cast<size_t>(offset + radius);
                sum += intermediate_value.At(x, neighbor_y) * weights[indx];
            }
            image.At(x, y) = sum;
        }
    }
}
