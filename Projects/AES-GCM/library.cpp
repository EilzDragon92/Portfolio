#include "header.h"
#include <argon2.h>

#define MEM_COST 512 * 1024
#define TIME_COST 4

int pnum;

int64_t GetFileSize(FILE *file) {
    if (_fseeki64(file, 0, SEEK_END)) return -1;

    int res = _ftelli64(file);

    rewind(file);

    return res;
}

int Argon2id(uint8_t salt[], const char pw[], uint8_t key[]) {
    return argon2id_hash_raw(TIME_COST, MEM_COST, pnum, pw, strlen(pw), salt, SALT_SIZE, key, KEY_SIZE);
}

int GetProcNum() {
    SYSTEM_INFO systemInfo;

    GetSystemInfo(&systemInfo);

    return systemInfo.dwNumberOfProcessors;
}

int Random(uint8_t *dst, int size) {
    return BCryptGenRandom(NULL, dst, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
}