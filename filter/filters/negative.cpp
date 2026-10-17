#include "../filter.h"
#include "../../image/color.h"

Color Negative::TransformPixel(const Color& pixel) const {
    return {1.0 - pixel.B, 1.0 - pixel.G, 1.0 - pixel.R};
}
