#include <exception>
#include <iostream>
#include <string_view>
#include <vector>
#include "errors.h"
#include "image/image.h"
#include "parser/parser.h"
#include "data_stream/data_stream.h"
#include "filter/factory.h"

int main(int argc, char** argv) {
    try {
        if (argc == 1) {
            throw Help();
        }

        const std::vector<std::string_view> raw_args(argv + 1, argv + argc);
        const Args args = ParseArgs(raw_args);

        Image image = Read(args.input);
        for (const auto& spec : args.filters) {
            const auto filter = CreateFilter(spec);
            filter->Apply(image);
        }
        Write(args.output, image);
        return 0;

    } catch (const Help&) {
        std::cout << "Использование: image_processor input.bmp output.bmp "
                     "[-filter параметры...]\n";
        return 0;

    } catch (const ParseError& error) {
        std::cerr << error.what() << '\n' << "Запустите программу с --help для справки.\n";
        return 1;

    } catch (const ImageProcessorError& error) {
        std::cerr << error.what() << '\n';
        return 1;

    } catch (const std::exception& error) {
        std::cerr << "Неожиданная ошибка: " << error.what() << '\n';
        return 1;
    }
}
