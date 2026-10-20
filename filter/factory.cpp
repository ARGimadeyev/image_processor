#include "factory.h"
#include "../errors.h"
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <string_view>
#include <system_error>
#include <unordered_map>

namespace {

using Creator = std::unique_ptr<Filter> (*)(const FilterSpec&);
constexpr int64_t MaxGlassRadius = 3037000499;  // floor(sqrt(INT64_MAX))

template <typename T>
std::unique_ptr<Filter> MakeWithoutParams(const FilterSpec& spec) {
    if (!spec.params.empty()) {
        throw ParseError(ToString(spec.name) + " не принимает никаких параметров.");
    }
    return std::make_unique<T>();
}

template <typename T>
bool ParseUnsigned(std::string_view text, T& value) {
    if (text.empty() || text[0] < '0' || text[0] > '9') {
        return false;
    }
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    return error == std::errc{} && end == text.data() + text.size();
}

bool ParseDouble(std::string_view text, double& value) {
    if (text.empty()) {
        return false;
    }
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), value);
    return error == std::errc{} && end == text.data() + text.size() && std::isfinite(value);
}

std::unique_ptr<Filter> MakeCrop(const FilterSpec& spec) {
    size_t width = 0;
    size_t height = 0;
    if (spec.params.size() != 2 || !ParseUnsigned(spec.params[0], width) ||
        !ParseUnsigned(spec.params[1], height) || width == 0 || height == 0) {
        throw ParseError("Crop принимает два натуральных числа");
    }
    return std::make_unique<Crop>(width, height);
}

std::unique_ptr<Filter> MakeEdgeDetection(const FilterSpec& spec) {
    double threshold = 0;
    if (spec.params.size() != 1 || !ParseDouble(spec.params[0], threshold)) {
        throw ParseError("Edge Detection принимает 1 аргумент double");
    }
    return std::make_unique<EdgeDetection>(threshold);
}

std::unique_ptr<Filter> MakeGaussianBlur(const FilterSpec& spec) {
    double sigma = 0;
    if (spec.params.size() != 1 || !ParseDouble(spec.params[0], sigma) || sigma < 0) {
        throw ParseError("Gaussian Blur принимает одно неотрицательное значение double");
    }
    if (sigma > static_cast<double>(std::numeric_limits<int64_t>::max() / 8)) {
        throw ParseError("Gaussian Blur: значение слишком велико");
    }
    return std::make_unique<GaussianBlur>(sigma);
}
std::unique_ptr<Filter> MakeGlassDistortion(const FilterSpec& spec) {
    int64_t radius = 0;
    double blur = 0;
    if (spec.params.size() != 2 || !ParseUnsigned(spec.params[0], radius) ||
        !ParseDouble(spec.params[1], blur) || blur < 0) {
        throw ParseError("Glass Distortion принимает радиус и блюр");
    }
    if (radius > MaxGlassRadius || blur > static_cast<double>(std::numeric_limits<int64_t>::max() / 16)) {
        throw ParseError("Glass Distortion: радиус или блюр слишком велики");
    }
    return std::make_unique<GlassDistortion>(radius, blur);
}
const std::unordered_map<std::string_view, Creator> FILTER_CREATOR = {
    {"crop", MakeCrop},
    {"gs", MakeWithoutParams<GrayScale>},
    {"neg", MakeWithoutParams<Negative>},
    {"sharp", MakeWithoutParams<Sharpening>},
    {"edge", MakeEdgeDetection},
    {"blur", MakeGaussianBlur},
    {"glass", MakeGlassDistortion},
};

}  // namespace

std::unique_ptr<Filter> CreateFilter(const FilterSpec& spec) {
    const auto it = FILTER_CREATOR.find(spec.name);
    if (it == FILTER_CREATOR.end()) {
        throw UnkdownFilter(spec.name);
    }
    return it->second(spec);
}
