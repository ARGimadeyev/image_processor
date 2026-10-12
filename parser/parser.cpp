#include "parser.h"
#include <cctype>
#include "../errors.h"
#include <string_view>
#include <vector>

namespace {

bool IsLetters(std::string_view s) {
    for (const auto i : s) {
        if (!std::isalpha(i)) {
            return false;
        }
    }
    return true;
}

bool IsFilterToken(std::string_view token) {
    return token.size() > 1 && token[0] == '-' && IsLetters(token.substr(1));
}

FilterSpec ParseFilter(const std::vector<std::string_view>& params, size_t& it) {
    FilterSpec filter;

    if (IsFilterToken(params[it])) {
        throw WrongArgumentsError();
    }

    filter.name = params[it].substr(1);
    ++it;

    for (; it < params.size() && !IsFilterToken(params[it]); ++it) {
        filter.params.push_back(params[it]);
    }
    return filter;
}
}  // namespace

Args ParseArgs(const std::vector<std::string_view>& params) {
    if (params.size() < 2u || params[0].starts_with('-') || params[1].starts_with('-')) {
        if (params.size() == 1 && params[0] == "--help") {
            throw Help();
        }
        throw NoInputOrOutputFiles();
    }

    Args arguments;
    arguments.input = params[0];
    arguments.output = params[1];

    for (size_t i = 2; i < params.size();) {
        arguments.filters.push_back(ParseFilter(params, i));
    }
    return arguments;
}