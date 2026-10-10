#pragma once

#include <fstream>
#include "../image/image.h"

template <class T>
T ReadTo(std::istream& in);

const std::string GetFileName(std::string_view path);

Image Read(std::string_view path);
