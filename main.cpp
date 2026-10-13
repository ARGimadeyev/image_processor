#include <exception>
#include <string_view>
#include <vector>
#include "errors.h"
#include "image/image.h"
#include "parser/parser.h"
#include "data_stream/data_stream.h"
#include "filter/filters.h"
#include <iostream>

int main(int argc, char **argv) {
    try {
        const std::vector<std::string_view> raw_args(argv + 1, argv + argc);
        Args args = ParseArgs(raw_args);
        Image image = Read(args.input);
        for (const auto &filter_struc : args.filters) {
            const auto filter = CreateFilter(filter_struc);
            filter->Apply(image);
        }
        Write(args.output, image);
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }

    return 0;
}
