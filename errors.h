#pragma once

#include <stdexcept>
#include <string>

inline std::string ToString(std::string_view s) {
    return static_cast<std::string>(s);
}

class ImageProcessorError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

class FileError : public ImageProcessorError {
public:
    using ImageProcessorError::ImageProcessorError;

    explicit FileError(std::string_view filename, std::string reason)
        : ImageProcessorError("Ошибка с файлом: " + ToString(filename) + "." + reason) {
    }
};

class FileOpeningError : public FileError {
public:
    using FileError::FileError;

    explicit FileOpeningError(std::string_view filename) : FileError(std::move(filename), "Не удалось открыть файл") {
    }
};

class FileReadingError : public FileError {
public:
    using FileError::FileError;

    explicit FileReadingError(std::string_view filename)
        : FileError(std::move(filename), "Не удалось прочитать данные") {
    }
};

class FileInvalidSignature : public FileError {
public:
    using FileError::FileError;

    explicit FileInvalidSignature(std::string_view filename)
        : FileError(std::move(filename), "Неподходящая разметка файла (используйте .bmp)") {
    }
};
