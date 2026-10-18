#include "../filter.h"

#include <cstdint>
#include <random>
#include <utility>

void GlassDistortion::Apply(Image& image) const {
    if (image.Width() == 0 || image.Height() == 0) {
        return;
    }

    constexpr int64_t Radius = 4;
    std::mt19937 generator(1);  // Один и тот же результат при каждом запуске.
    std::uniform_int_distribution<int64_t> shift(-Radius, Radius);

    Image result(image.Height(), image.Width());

    for (size_t y = 0; y < image.Height(); ++y) {
        for (size_t x = 0; x < image.Width(); ++x) {
            const size_t source_x = Clamp(x, shift(generator), image.Width());
            const size_t source_y = Clamp(y, shift(generator), image.Height());

            result.At(x, y) = image.At(source_x, source_y);
        }
    }

    image = std::move(result);
}