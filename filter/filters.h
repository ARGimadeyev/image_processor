#pragma once

#include <cstddef>
#include <memory>

#include "filter.h"
#include "../parser/parser.h"

std::unique_ptr<Filter> CreateFilter(const FilterSpec& spec);

class Crop : public Filter {
public:
    explicit Crop(size_t width, size_t height) : width_(width), height_(height) {
    }
    void Apply(Image& image) const override;

private:
    size_t width_;
    size_t height_;
};

class GrayScale : public Filter {
public:
    void Apply(Image& image) const override;
};

class Negative : public Filter {
public:
    void Apply(Image& image) const override;
};

class Sharpening : public Filter {
public:
    void Apply(Image& image) const override;
};

class EdgeDetection : public Filter {
public:
    explicit EdgeDetection(double threshold) : threshold_(threshold) {
    }
    void Apply(Image& image) const override;

private:
    double threshold_;
};

class GaussianBlur : public Filter {
public:
    explicit GaussianBlur(double sigma) : sigma_(sigma) {
    }
    void Apply(Image& image) const override;

private:
    double sigma_;
};

class GlassDistortion : public Filter {
public:
    void Apply(Image& image) const override;
};
