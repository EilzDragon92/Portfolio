/**
 * @file	Password.h
 * @brief	RAII class that securely handles file pointer
 * @author	EilzDragon92
 * 
 * This file is not used in current version.
 */

#pragma once

#include "Common/header.h"

class File {
public:
    File() {
        ;
    }

    File(const char *path, const char *mode) {
        set(path, mode);
    };

    ~File() {
        if (file) {
            fclose(file);
            file = nullptr;
        }
    }

    FILE *get() {
        return file;
    }

    bool check() {
        return (file != nullptr && res == 0);
    }

    void set(const char *path, const char *mode) {
        res = fopen_s(&file, path, mode);
    }

private:
    FILE *file = nullptr;
    errno_t res = 1;
};