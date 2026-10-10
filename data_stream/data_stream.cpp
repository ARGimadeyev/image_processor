#include "data_stream.h"
#include "../errors.h"
#include <fstream>
#include <ios>
#include <string_view>

namespace {
constexpr uint16_t KSignature = 0x4D42;  // "BM"
}

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

Image Read(std::string_view path) {
    const auto filename = GetFileName(path);
    std::ifstream in(ToString(path), std::ios::binary);

    if (!in) {
        throw FileOpeningError(filename);
    }

    if (ReadTo<uint16_t>(in, filename) != KSignature) {
        throw FileInvalidSignature(filename);
    }
    //
    //
    //
    //

    return {};
}
