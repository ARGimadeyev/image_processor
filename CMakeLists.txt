cmake_minimum_required(VERSION 3.20)
project(image_processor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(image_processor
    main.cpp
    data_stream/data_stream.cpp
    filters/filters.cpp
    image/image.cpp
    parser/parser.cpp
)
target_compile_options(image_processor PRIVATE -Wall -Wextra -Wpedantic)
