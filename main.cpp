#include <exception>
#include <string_view>
#include <vector>
#include "errors.h"
#include "parser/parser.h"

#include <iostream>
int main(int argc, char **argv) {
    try {
        try {
            const std::vector<std::string_view> raw_args(argv + 1, argv + argc);
            Args args = ParseArgs(raw_args);
        } catch (const ParseError &e) {
            std::cerr << e.what() << '\n';
            std::cerr << "Правильный порядок аргументов\n";
        } catch (const Help &e) {
            std::cerr << e.what() << '\n';
        } catch (const ImageProcessorError &e) {
            std::cerr << e.what() << '\n';
            return 1;
        }
    } catch (const ImageProcessorError &e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "unexpected: " << e.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Неизвестная ошибка\n";
        return 1;
    }
    // std::cout << args.input << '\n' << args.output << '\n';
    // for (const auto &filter : args.filters) {
    //     std::cout << filter.name << ' ';
    //     for (const auto j : filter.params) {
    //         std::cout << j << ' ';
    //     }
    //     std::cout << '\n';
    // }

    return 0;
}
