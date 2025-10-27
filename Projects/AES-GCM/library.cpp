#include "header.h"

int pnum;

int64_t GetFileSize(FILE *file) {
    if (_fseeki64(file, 0, SEEK_END)) return -1;

    int res = _ftelli64(file);

    rewind(file);

    return res;
}

int Random(uint8_t *dst, int size) {
    return BCryptGenRandom(NULL, dst, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
}