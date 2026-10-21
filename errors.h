#pragma once

#include <stdexcept>
#include <string>
#include <string_view>

inline std::string ToString(std::string_view s) {
    return static_cast<std::string>(s);
}

class ImageProcessorError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// File Errors

class FileError : public ImageProcessorError {
public:
    explicit FileError(std::string_view filename, std::string reason)
        : ImageProcessorError("Ошибка с файлом: " + ToString(filename) + ". " + reason) {
    }
};

class FileOpeningError : public FileError {
public:
    explicit FileOpeningError(std::string_view filename) : FileError(filename, "Не удалось открыть файл") {
    }
};

class FileReadingError : public FileError {
public:
    explicit FileReadingError(std::string_view filename)
        : FileError(filename, "Не удалось прочитать данные") {
    }
};

class FileWritingError : public FileError {
public:
    explicit FileWritingError(std::string_view filename) : FileError(filename, "Не удалось записать данные") {
    }
};

class FileInvalidSignature : public FileError {
public:
    explicit FileInvalidSignature(std::string_view filename)
        : FileError(filename, "Неподходящая разметка файла (используйте .bmp)") {
    }
};

class FileInvalidDimensionsError : public FileError {
public:
    explicit FileInvalidDimensionsError(std::string_view filename)
        : FileError(filename, "Размеры изображения не соответствуют данным BMP") {
    }
};

// Parse Errors

class ParseError : public ImageProcessorError {
public:
    explicit ParseError(std::string reason) : ImageProcessorError("Некорректные аргументы: " + reason) {
    }
};

class NoInputOrOutputFiles : public ParseError {
public:
    explicit NoInputOrOutputFiles() : ParseError("Нету входного и/или выходного файлов") {
    }
};

class WrongArgumentsError : public ParseError {
public:
    explicit WrongArgumentsError() : ParseError("Аргументы поданы неправильно (--help)") {
    }
};

// Фильтры

class UnkdownFilter : public ParseError {
public:
    explicit UnkdownFilter(std::string_view filter) : ParseError("Неизвестный фильтр: " + ToString(filter)) {
    }
};

// Пoддержка

class Help : public ImageProcessorError {
public:
    explicit Help() : ImageProcessorError("Как использовать?") {
    }
};
