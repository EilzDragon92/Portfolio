#pragma once
#pragma comment(lib, "bcrypt.lib")

#include <QString>
#include <atomic>
#include <functional>
#include <openssl/evp.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>
#include <bcrypt.h>

#define FONT_SCALE 1.2
#define KEY_SIZE 32
#define SALT_SIZE 16

struct UserInput {
    bool valid;
    int mode;
    QString src;
    QString dst;
    QString pw;
};

int64_t GetFileSize(FILE *file);

int Argon2id(uint8_t salt[], const char pw[], size_t plen, uint8_t key[]);

int GetProcNum();

int Random(uint8_t *dst, size_t size);

int RemoveFile(const char *path);

void SecureWipeMemory(void *ptr, size_t size);