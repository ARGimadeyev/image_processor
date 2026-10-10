#include <string_view>
#include <vector>
#include "parser/parser.h"

#include <iostream>
int main(int argc, char **argv) {
    const std::vector<std::string_view> raw_args(argv + 1, argv + argc);
    Args args = ParseArgs(raw_args);

    std::cout << args.input << '\n' << args.output << '\n';
    for (const auto &filter : args.filters) {
        std::cout << filter.name << ' ';
        for (const auto j : filter.params) {
            std::cout << j << ' ';
        }
        std::cout << '\n';
    }
    return 0;
}
