#include "data_stream.h"
#include "../errors.h"
#include "../image/color.h"
#include "../image/image.h"
#include <fstream>
#include <ios>
#include <limits>
#include <string_view>
#include <bit>

namespace {
constexpr uint16_t KSignature = 0x4D42;  // "BM"
constexpr uint32_t KFileHeaderSize = 14;
constexpr uint32_t KInfoHeaderSize = 40;
constexpr uint16_t KBitsPerPixel = 24;
constexpr uint64_t KBytesPerPixel = 3;
constexpr uint64_t KRowAlignment = 4;

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

template <class T>
void WriteTo(std::ostream& out, T value, std::string_view filename) {
    constexpr uint8_t CharSize = 8;
    unsigned char bytes[sizeof(T)];
    for (size_t i = 0; i < sizeof(T); ++i) {
        bytes[i] = static_cast<unsigned char>(value >> (i * CharSize));
    }
    if (!out.write(reinterpret_cast<const char*>(bytes), sizeof(bytes))) {
        throw FileWritingError(filename);
    }
}

const std::string GetFileName(std::string_view path) {
    const auto slash = path.find_last_of('/');
    if (slash == std::string_view::npos) {
        return ToString(path);
    }
    return ToString(path.substr(slash + 1));
}

}  // namespace

BmpHeader::BmpHeader(std::istream& in, std::string_view filename) {
    if (ReadTo<uint16_t>(in, filename) != KSignature) {
        throw FileInvalidSignature(filename);
    }

    ReadTo<uint32_t>(in, filename);  // Размер файла из заголовка
    const auto reserved1 = ReadTo<uint16_t>(in, filename);
    const auto reserved2 = ReadTo<uint16_t>(in, filename);
    pixel_offset_ = ReadTo<uint32_t>(in, filename);

    const auto dib_size = ReadTo<uint32_t>(in, filename);
    const auto width = std::bit_cast<int32_t>(ReadTo<uint32_t>(in, filename));
    const auto height = std::bit_cast<int32_t>(ReadTo<uint32_t>(in, filename));
    const auto planes = ReadTo<uint16_t>(in, filename);
    const auto bits_per_pixel = ReadTo<uint16_t>(in, filename);
    const auto compression = ReadTo<uint32_t>(in, filename);

    ReadTo<uint32_t>(in, filename);  // Размер изображения: для BI_RGB может быть 0
    ReadTo<uint32_t>(in, filename);  // Разрешение по X
    ReadTo<uint32_t>(in, filename);  // Разрешение по Y
    const auto colors_used = ReadTo<uint32_t>(in, filename);
    ReadTo<uint32_t>(in, filename);  // Число важных цветов

    if (reserved1 != 0 || reserved2 != 0 || dib_size != KInfoHeaderSize ||
        pixel_offset_ < KFileHeaderSize + KInfoHeaderSize || width <= 0 || height == 0 || planes != 1 ||
        bits_per_pixel != KBitsPerPixel || compression != 0 || colors_used != 0) {
        throw FileError(filename, "Неподдерживаемый формат BMP");
    }

    width_ = static_cast<size_t>(width);
    top_down_ = height < 0;
    height_ = static_cast<size_t>(top_down_ ? -static_cast<int64_t>(height) : height);
}

void BmpHeader::Write(std::ostream& out, uint32_t width, uint32_t height, uint32_t image_size,
                      std::string_view filename) {
    const uint32_t file_size = KFileHeaderSize + KInfoHeaderSize + image_size;

    WriteTo<uint16_t>(out, KSignature, filename);
    WriteTo<uint32_t>(out, file_size, filename);
    WriteTo<uint16_t>(out, 0, filename);  // Reserved
    WriteTo<uint16_t>(out, 0, filename);  // Reserved
    WriteTo<uint32_t>(out, KFileHeaderSize + KInfoHeaderSize, filename);

    WriteTo<uint32_t>(out, KInfoHeaderSize, filename);
    WriteTo<uint32_t>(out, width, filename);
    WriteTo<uint32_t>(out, height, filename);
    WriteTo<uint16_t>(out, 1, filename);  // Planes
    WriteTo<uint16_t>(out, KBitsPerPixel, filename);
    WriteTo<uint32_t>(out, 0, filename);  // BI_RGB: no compression
    WriteTo<uint32_t>(out, image_size, filename);
    WriteTo<uint32_t>(out, 0, filename);  // Horizontal resolution
    WriteTo<uint32_t>(out, 0, filename);  // Vertical resolution
    WriteTo<uint32_t>(out, 0, filename);  // No color table
    WriteTo<uint32_t>(out, 0, filename);  // Important colors
}

Image Read(std::string_view path) {
    const auto filename = GetFileName(path);
    std::ifstream in(ToString(path), std::ios::binary);
    if (!in) {
        throw FileOpeningError(filename);
    }

    const BmpHeader header(in, filename);

    const size_t row_bytes = header.Width() * KBytesPerPixel;
    const size_t padding = (KRowAlignment - row_bytes % KRowAlignment) % KRowAlignment;

    // После заголовков поток стоит перед данными между ними и пикселями.
    const auto gap = static_cast<std::streamsize>(header.PixelOffset() - KFileHeaderSize - KInfoHeaderSize);
    in.ignore(gap);
    if (in.gcount() != gap || !in) {
        throw FileReadingError(filename);
    }

    Image image(header.Height(), header.Width());

    for (size_t row = 0; row < header.Height(); ++row) {
        const size_t y = header.TopDown() ? row : header.Height() - 1 - row;

        for (size_t x = 0; x < header.Width(); ++x) {
            const uint8_t b = ReadTo<uint8_t>(in, filename);
            const uint8_t g = ReadTo<uint8_t>(in, filename);
            const uint8_t r = ReadTo<uint8_t>(in, filename);
            image.At(x, y) = Color(b, g, r);
        }

        in.ignore(static_cast<std::streamsize>(padding));
        if (in.gcount() != static_cast<std::streamsize>(padding) || !in) {
            throw FileReadingError(filename);
        }
    }
    return image;
}

void Write(std::string_view path, const Image& image) {
    const auto filename = GetFileName(path);
    if (image.Width() == 0 || image.Height() == 0 || image.Width() > std::numeric_limits<int32_t>::max() ||
        image.Height() > std::numeric_limits<int32_t>::max()) {
        throw FileError(filename, "Размеры изображения не подходят для BMP");
    }

    const uint64_t row_bytes = image.Width() * KBytesPerPixel;
    const uint64_t padding = (KRowAlignment - row_bytes % KRowAlignment) % KRowAlignment;
    const uint64_t image_size = (row_bytes + padding) * image.Height();
    const uint64_t file_size = KFileHeaderSize + KInfoHeaderSize + image_size;
    if (file_size > std::numeric_limits<uint32_t>::max()) {
        throw FileError(filename, "Изображение слишком большое для BMP");
    }

    std::ofstream out(ToString(path), std::ios::binary);
    if (!out) {
        throw FileOpeningError(filename);
    }

    BmpHeader::Write(out, static_cast<uint32_t>(image.Width()), static_cast<uint32_t>(image.Height()),
                     static_cast<uint32_t>(image_size), filename);

    const char zeros[KRowAlignment - 1]{};
    for (size_t row = image.Height(); row > 0; --row) {
        for (size_t x = 0; x < image.Width(); ++x) {
            const auto [B, G, R] = image.At(x, row - 1).Out();
            WriteTo<uint8_t>(out, B, filename);
            WriteTo<uint8_t>(out, G, filename);
            WriteTo<uint8_t>(out, R, filename);
        }
        if (!out.write(zeros, static_cast<std::streamsize>(padding))) {
            throw FileWritingError(filename);
        }
    }

    out.close();
    if (!out) {
        throw FileWritingError(filename);
    }
}
