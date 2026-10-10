cmake_minimum_required(VERSION 3.20)
project(brawl CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(brawl
    main.cpp
    filters/filters.cpp
    image/image.cpp
    parser/parser.cpp
)
target_compile_options(image_porcessor PRIVATE -Wall -Wextra -Wpedantic)