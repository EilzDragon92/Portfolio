#include "header.h"

int fileSize, pnum;

int Random(uint8_t *dst, int size) {
    return BCryptGenRandom(NULL, dst, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
}