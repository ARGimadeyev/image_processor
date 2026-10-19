#pragma once

#include "../image/image.h"

class Filter {
public:
    virtual ~Filter() = default;
    virtual void Apply(Image& image) const = 0;
};

class PixelwiseFilter : public virtual Filter {
public:
    void Apply(Image& image) const override;

    virtual Color TransformPixel(const Color& pixel) const = 0;
};

class ConvolutionFilter : public virtual Filter {
public:
    void Apply(Image& image) const override;

    virtual Color ComputePixel(const Image& pixel, size_t x, size_t y) const = 0;
};

class Crop : public Filter {
public:
    explicit Crop(size_t width, size_t height) : width_(width), height_(height) {
    }
    void Apply(Image& image) const override;

private:
    size_t width_;
    size_t height_;
};

class GrayScale : public PixelwiseFilter {
public:
    Color TransformPixel(const Color& pixel) const override;
};

class Negative : public PixelwiseFilter {
public:
    Color TransformPixel(const Color& pixel) const override;
};

class Sharpening : public ConvolutionFilter {
public:
    Color ComputePixel(const Image& pixel, size_t x, size_t y) const override;
};

class EdgeDetection : public GrayScale, public ConvolutionFilter {
public:
    explicit EdgeDetection(double threshold) : threshold_(threshold) {
    }
    Color ComputePixel(const Image& pixel, size_t x, size_t y) const override;

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
    GlassDistortion(int64_t radius, double blur) : radius_(radius), blur_(blur) {
    }
    void Apply(Image& image) const override;

private:
    int64_t radius_;
    double blur_;
};
