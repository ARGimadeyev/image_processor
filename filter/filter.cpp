#include "filter.h"
#include <cstddef>
#include <utility>

void PixelwiseFilter::Apply(Image& image) const {
    for (size_t y = 0; y < image.Height(); ++y) {
        for (size_t x = 0; x < image.Width(); ++x) {
            image.At(x, y) = TransformPixel(image.At(x, y));
        }
    }
}

void ConvolutionFilter::Apply(Image& image) const {
    Image new_image(image.Height(), image.Width());
    for (size_t y = 0; y < image.Height(); ++y) {
        for (size_t x = 0; x < image.Width(); ++x) {
            new_image.At(x, y) = ComputePixel(image, x, y).Norm();
        }
    }
    image = std::move(new_image);
}
