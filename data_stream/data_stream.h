#pragma once

#include <string_view>
#include "../image/image.h"

class BitmapInfoHeader {
public:
    static BitmapInfoHeader Read(std::istream& in, std::string_view filename);

    uint64_t Width() const {
        return width_;
    }

    uint64_t Height() const {
        return height_;
    }

    bool TopDown() const {
        return top_down_;
    }

    uint64_t RowStride() const {
        return row_stride_;
    }

    uint64_t Padding() const {
        return padding_;
    }

private:
    BitmapInfoHeader(uint64_t width, uint64_t height, bool top_down, uint64_t row_stride, uint64_t padding)
        : width_(width), height_(height), top_down_(top_down), row_stride_(row_stride), padding_(padding) {
    }

    uint64_t width_;
    uint64_t height_;
    bool top_down_;
    uint64_t row_stride_;
    uint64_t padding_;
};
Image Read(std::string_view path);
