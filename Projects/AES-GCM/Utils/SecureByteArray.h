#pragma once

#include "header.h"

#include <QByteArray>

class SecureByteArray {
public:
    explicit SecureByteArray(QByteArray &&data) : data(std::move(data)) {};

    ~SecureByteArray() {
        data.detach();

        SecureZeroMemory(data.data(), data.size());

        data.clear();
        data.squeeze();
    }

    const char *constData() const {
        return data.constData();
    }

    SecureByteArray(const SecureByteArray &) = delete;
    SecureByteArray &operator=(const SecureByteArray &) = delete;
    SecureByteArray(SecureByteArray &&) = delete;
    SecureByteArray &operator=(SecureByteArray &&) = delete;

private:
    QByteArray data;
};