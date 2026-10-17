#pragma once

#include <memory>

#include "filter.h"
#include "../parser/parser.h"

std::unique_ptr<Filter> CreateFilter(const FilterSpec& spec);
