#include <algorithm>
#include <utility>
#include "../filter.h"

void Crop::Apply(Image& image) const {
    const size_t height = std::min(height_, image.Height());
    const size_t width = std::min(width_, image.Width());

    Image new_image(height, width);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            new_image.At(x, y) = image.At(x, y);
        }
    }
    image = std::move(new_image);
}
