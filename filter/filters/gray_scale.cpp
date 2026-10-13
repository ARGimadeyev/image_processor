#include <cmath>
#include "../filters.h"

constexpr double RColor = 0.299;
constexpr double GColor = 0.587;
constexpr double BColor = 0.114;
void GrayScale::Apply(Image& image) const {
    for (size_t y = 0; y < image.Height(); ++y) {
        for (size_t x = 0; x < image.Width(); ++x) {
            auto [b, g, r] = image.At(x, y);
            double color = BColor * b + GColor * g + RColor * r;
            image.At(x, y) = Color(color, color, color);
        }
    }
}
