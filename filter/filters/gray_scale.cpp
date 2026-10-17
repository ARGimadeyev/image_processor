#include "../filter.h"
#include "../../image/color.h"

constexpr double RColor = 0.299;
constexpr double GColor = 0.587;
constexpr double BColor = 0.114;

Color GrayScale::TransformPixel(const Color &pixel) const {
    auto [b, g, r] = pixel;
    double color = BColor * b + GColor * g + RColor * r;
    return {color, color, color};
}
