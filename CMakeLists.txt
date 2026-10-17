cmake_minimum_required(VERSION 3.20)
project(image_processor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(image_processor
    main.cpp
    data_stream/data_stream.cpp
    image/image.cpp
    image/color.cpp
    parser/parser.cpp
    filter/factory.cpp
    filter/filters/crop.cpp
    filter/filters/edge_detection.cpp
    filter/filters/gaussian_blur.cpp
    filter/filters/glass_distortion.cpp
    filter/filters/gray_scale.cpp
    filter/filters/negative.cpp
    filter/filters/sharpening.cpp
    filter/filter.cpp

)
target_compile_options(image_processor PRIVATE -Wall -Wextra -Wpedantic)
