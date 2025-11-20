#pragma once
#pragma comment(lib, "bcrypt.lib")

#include <QString>
#include <functional>
#include <io.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdint.h>
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

class MainGUI;

extern int pnum;

int64_t GetFileSize(FILE *file);

int Argon2id(uint8_t salt[], const char pw[], uint8_t key[]);

int GetProcNum();

int Random(uint8_t *dst, int size);