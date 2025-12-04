#pragma once

#include "PWLineEdit.h"
#include "header.h"

#include <QByteArray>

class Password {
public:
    Password() {};

    ~Password() {
        SecureWipeMemory(data, size);
    }

    size_t getSize() {
        return size;
    }

    bool isEmpty() {
        return size == 0;
    }

    const char *getData() {
        return data;
    }

    void setData(PWLineEdit &pwLine) {
        std::string tmp = pwLine.getText().toStdString(); // ERROR

        wipe();

        size = tmp.size();
        data = new char[size] {};

        for (size_t i = 0; i < size; i++) data[i] = tmp[i];

        SecureWipeMemory(&tmp[0], size);
    }

    void setData(Password &pw) {
        setData(pw.getData(), pw.getSize());
    }

    void setData(const char *str, size_t inputSize) {
        wipe();

        size = inputSize;
        data = new char[size];

        for (size_t i = 0; i < size; i++) data[i] = str[i];
    }

    Password(const Password &) = delete;
    Password &operator=(const Password &) = delete;
    Password(Password &&) = delete;
    Password &operator=(Password &&) = delete;

private:
    char *data = nullptr;
    size_t size = 0;

    void wipe() {
        SecureWipeMemory(data, size);

        delete[] data;

        data = nullptr;
        size = 0;
    }
};