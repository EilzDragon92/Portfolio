#pragma once
#pragma comment(lib, "bcrypt.lib")

#include <QString>
#include <windows.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <io.h>
#include <functional>
#include <bcrypt.h>

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

UserInput GetUserInput(int argc, char *argv[], MainGUI **outMainGUI);

int64_t GetFileSize(FILE *file);

int Argon2id(uint8_t salt[], const char pw[], uint8_t key[]);

int GetProcNum();

int Random(uint8_t *dst, int size);