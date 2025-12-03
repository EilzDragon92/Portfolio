#pragma once

#include "header.h"

class FileHandle {
public:
    FileHandle() : file(nullptr) {}

    FileHandle(const char *path, const char *mode) {
        fopen_s(&file, path, mode);
    };

    ~FileHandle() {
        if (file) {
            fclose(file);
            file = nullptr;
        }
    }

    FileHandle(const FileHandle &) = delete;
    FileHandle &operator=(const FileHandle &) = delete;

    FileHandle(FileHandle &&other) noexcept : file(other.file) {
        other.file = nullptr;
    }

    FileHandle &operator=(FileHandle &&other) noexcept {
        if (this != &other) {
            if (file) fclose(file);
            file = other.file;
            other.file = nullptr;
        }

        return *this;
    }

    FILE *get() const {
        return file;
    }

    explicit operator bool() const {
        return file != nullptr;
    }

    FILE *release() {
        FILE *tmp = file;
        file = nullptr;
        return tmp;
    }

private:
    FILE *file = nullptr;
};