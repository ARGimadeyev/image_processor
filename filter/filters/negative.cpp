#include <cstddef>
#include "../filters.h"

void Negative::Apply(Image& image) const {
    for (size_t y = 0; y < image.Height(); ++y) {
        for (size_t x = 0; x < image.Width(); ++x) {
            Color color = image.At(x, y);
            color.B = 1.0 - color.B;
            color.G = 1.0 - color.G;
            color.R = 1.0 - color.R;
            image.At(x, y) = color;
        }
    }
}
