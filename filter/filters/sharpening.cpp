#include <cstddef>
#include "../filter.h"

namespace {

const std::vector<std::vector<int32_t>> MATRIX{
    {0, -1, 0},
    {-1, 5, -1},
    {0, -1, 0},
};
}

Color Sharpening::ComputePixel(const Image& image, size_t x, size_t y) const {
    return image.GetMatrix(MATRIX, x, y);
}
