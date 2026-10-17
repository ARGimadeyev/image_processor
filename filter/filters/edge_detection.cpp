#include "../filter.h"
#include "../../image/color.h"
#include <cstddef>

const std::vector<std::vector<int32_t>> MATRIX{
    {0, -1, 0},
    {-1, 4, -1},
    {0, -1, 0},
};

Color EdgeDetection::ComputePixel(const Image& image, size_t x, size_t y) const {
    const double value = image.GetMatrix(MATRIX, x, y).B;
    const double result = value > threshold_ ? 1.0 : 0.0;
    return {result, result, result};
}

void EdgeDetection::Apply(Image& image) const {
    GrayScale::Apply(image);
    ConvolutionFilter::Apply(image);
}
