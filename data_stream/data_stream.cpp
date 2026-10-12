#include "data_stream.h"
#include "../errors.h"
#include <fstream>
#include <ios>
#include <string_view>
#include <limits>

namespace {
constexpr uint16_t KSignature = 0x4D42;  // "BM"

template <class T>
T ReadTo(std::istream& in, std::string_view filename) {
    constexpr uint8_t CharSize = 8;
    unsigned char bytes[sizeof(T)];

    if (!in.read(reinterpret_cast<char*>(bytes), sizeof(T))) {
        throw FileReadingError(filename);
    }

    uint64_t value = 0;
    for (size_t i = 0; i < sizeof(T); ++i) {
        value |= static_cast<uint64_t>(bytes[i]) << (i * CharSize);
    }
    return static_cast<T>(value);
}

const std::string GetFileName(std::string_view path) {
    const auto slash = path.find_last_of('/');
    if (slash == std::string_view::npos) {
        return ToString(path);
    }
    return ToString(path.substr(slash + 1));
}

}  // namespace

Image Read(std::string_view path) {
    const auto filename = GetFileName(path);
    std::ifstream in(ToString(path), std::ios::binary);

    if (!in) {
        throw FileOpeningError(filename);
    }

    if (ReadTo<uint16_t>(in, filename) != KSignature) {
        throw FileInvalidSignature(filename);
    }

    ReadTo<uint32_t>(in, filename);  // размер файла в заголовке
    ReadTo<uint16_t>(in, filename);  // reserved 1
    ReadTo<uint16_t>(in, filename);  // reserved 2

    const uint32_t pixel_offset = ReadTo<uint32_t>(in, filename);

    const uint32_t dib_size = ReadTo<uint32_t>(in, filename);
    const uint32_t raw_width = ReadTo<uint32_t>(in, filename);
    const uint32_t raw_height = ReadTo<uint32_t>(in, filename);
    const uint16_t planes = ReadTo<uint16_t>(in, filename);
    const uint16_t bits_per_pixel = ReadTo<uint16_t>(in, filename);
    const uint32_t compression = ReadTo<uint32_t>(in, filename);

    if (dib_size != 40 || pixel_offset < 54 || raw_width == 0 || raw_width > INT32_MAX || raw_height == 0 ||
        planes != 1 || bits_per_pixel != 24 || compression != 0) {
        throw FileReadingError(filename);
    }

    // Отрицательная высота BMP означает строки сверху вниз.
    const bool top_down = raw_height > INT32_MAX;
    const uint64_t height = top_down ? (uint64_t{1} << 32) - raw_height : raw_height;
    const uint64_t width = raw_width;

    // Каждая строка BMP дополнена нулевыми байтами до кратности 4.
    const uint64_t row_bytes = width * 3;
    const uint64_t padding = (4 - row_bytes % 4) % 4;
    const uint64_t stride = row_bytes + padding;

    in.seekg(0, std::ios::end);
    const auto end = in.tellg();
    if (end < std::streampos{0}) {
        throw FileReadingError(filename);
    }
    const auto file_size = static_cast<uint64_t>(static_cast<std::streamoff>(end));

    if (pixel_offset > file_size || stride * height > file_size - pixel_offset ||
        width * height > std::numeric_limits<size_t>::max()) {
        throw FileReadingError(filename);
    }

    Image image(static_cast<size_t>(width), static_cast<size_t>(height));

    in.seekg(pixel_offset, std::ios::beg);
    if (!in) {
        throw FileReadingError(filename);
    }

    for (size_t row = 0; row < height; ++row) {
        const size_t y = top_down ? row : height - 1 - row;

        for (size_t x = 0; x < width; ++x) {
            const uint8_t b = ReadTo<uint8_t>(in, filename);
            const uint8_t g = ReadTo<uint8_t>(in, filename);
            const uint8_t r = ReadTo<uint8_t>(in, filename);
            image.At(x, y) = Color(r, g, b);
        }

        char skipped[3];
        if (padding != 0 && !in.read(skipped, static_cast<std::streamsize>(padding))) {
            throw FileReadingError(filename);
        }
    }

    return image;
}
