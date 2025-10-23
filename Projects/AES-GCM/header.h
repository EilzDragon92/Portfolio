#pragma once
#pragma comment(lib, "bcrypt.lib")

#include <QString>

#include <windows.h>
#include <stdint.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <io.h>
#include <bcrypt.h>

#define KEY_SIZE 32
#define SALT_SIZE 16

using namespace std;

struct UserInput {
    bool valid;
    int mode;
    QString src;
    QString dst;
    QString pw;
};

extern int pnum;

UserInput GetUserInput(int argc, char *argv[]);

int Argon2id(uint8_t salt[], const char pw[], uint8_t key[]);

int GetFileSize(FILE *file);

int Random(uint8_t *dst, int size);