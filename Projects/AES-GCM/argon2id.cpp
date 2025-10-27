#include "header.h"
#include <argon2.h>

#define MEMO_COST 512 * 1024
#define TIME_COST 4

int Argon2id(uint8_t salt[], const char pw[], uint8_t key[]) {
    return argon2id_hash_raw(MEMO_COST, TIME_COST, pnum, pw, strlen(pw), salt, SALT_SIZE, key, KEY_SIZE);
}