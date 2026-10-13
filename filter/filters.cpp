#include "filter.h"
#include "filters.h"
#include "../parser/parser.h"
#include "../errors.h"
#include <cctype>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

namespace {

using Creator = std::unique_ptr<Filter> (*)(const FilterSpec&);

template <typename T>
std::unique_ptr<Filter> MakeWithoutParams(const FilterSpec& spec) {
    if (!spec.params.empty()) {
        throw ParseError(ToString(spec.name) + " не принимает никаких параметров.");
    }
    return std::make_unique<T>();
}

bool IsDouble(std::string_view num) {
    if (num.empty() || (!isdigit(num[0]) && num[0] != '-' && num[0] != '.')) {
        return false;
    }
    size_t dot = 0;
    for (size_t i = 1; i < num.size(); ++i) {
        if (num[i] == '.') {
            dot++;
            continue;
        }
        if (!std::isdigit(num[i])) {
            return false;
        }
    }
    return dot <= 1;
}

bool IsUnsigned(std::string_view num) {
    if (num.empty()) {
        return false;
    }
    for (size_t i = 0; i < num.size(); ++i) {
        if (!std::isdigit(num[i])) {
            return false;
        }
    }
    return true;
}

std::unique_ptr<Filter> MakeCrop(const FilterSpec& spec) {
    if (spec.params.size() != 2) {
        throw ParseError("Crop");
    }
    if (!IsUnsigned(spec.params[0]) || !IsUnsigned(spec.params[1])) {
        throw ParseError("Crop принимает два натуральных числа");
    }
    size_t width = std::stoul(ToString(spec.params[0]));
    size_t height = std::stoul(ToString(spec.params[1]));
    if (width == 0 || height == 0) {
        throw ParseError("Crop принимает два натуральных числа");
    }
    return std::make_unique<Crop>(width, height);
}

std::unique_ptr<Filter> MakeEdgeDetection(const FilterSpec& spec) {
    if (spec.params.size() != 1 || !IsDouble(spec.params[0])) {
        throw ParseError("Edge Detection принимает 1 аргумент double");
    }
    return std::make_unique<EdgeDetection>(std::stod(ToString(spec.params[0])));
}

std::unique_ptr<Filter> MakeGaussianBlur(const FilterSpec& spec) {
    if (spec.params.size() != 1 || !IsDouble(spec.params[0]) || spec.params[0][0] == '-') {
        throw ParseError("Gaussian Detection принимает одно неотрицательное значение double");
    }
    return std::make_unique<GaussianBlur>(std::stod(ToString(spec.params[0])));
}

const std::unordered_map<std::string_view, Creator> FILTER_CREATOR = {
    {"crop", &MakeCrop},
    {"gs", &MakeWithoutParams<GrayScale>},
    {"neg", &MakeWithoutParams<Negative>},
    {"sharp", &MakeWithoutParams<Sharpening>},
    {"edge", &MakeEdgeDetection},
    {"blur", &MakeGaussianBlur},
};

}  // namespace

std::unique_ptr<Filter> CreateFilter(const FilterSpec& spec) {
    const auto it = FILTER_CREATOR.find(spec.name);
    if (it == FILTER_CREATOR.end()) {
        throw UnkdownFilter(spec.name);
    }
    return it->second(spec);
}
