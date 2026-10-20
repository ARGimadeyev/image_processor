#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <string_view>
#include "../image/image.h"

class BmpHeader {
public:
    BmpHeader(std::istream& in, std::string_view filename);
    static void Write(std::ostream& out, uint32_t width, uint32_t height, uint32_t image_size,
                      std::string_view filename);

    size_t Width() const {
        return width_;
    }
    size_t Height() const {
        return height_;
    }
    size_t PixelOffset() const {
        return pixel_offset_;
    }
    bool TopDown() const {
        return top_down_;
    }

private:
    size_t width_;
    size_t height_;
    size_t pixel_offset_;
    bool top_down_;
};

Image Read(std::string_view path);

void Write(std::string_view path, const Image& image);
