/**
 * @file	library.h
 * @brief	Implementation of utility functions
 * @author	EilzDragon92
 */

#include "Common/header.h"

int64_t GetFileSize(FILE *file) {
    int64_t size;

#ifdef _WIN32
    if (_fseeki64(file, 0, SEEK_END)) return -1;
    size = _ftelli64(file);

#else
    if (fseeko(file, 0, SEEK_END)) return -1;
    size = ftello(file);

#endif
    rewind(file);
    return size;
}

int Argon2id(uint8_t salt[], const char pw[], size_t plen, uint8_t key[]) {
    return argon2id_hash_raw(TIME_COST, MEM_COST, GetProcNum(), pw, plen, salt, SALT_SIZE, key, KEY_SIZE);
}

int GetProcNum() {
#ifdef _WIN32
    SYSTEM_INFO systemInfo;

    GetSystemInfo(&systemInfo);

    return systemInfo.dwNumberOfProcessors;

#else
    return std::thread::hardware_concurrency();

#endif
}

int Random(uint8_t *dst, size_t size) {
#ifdef _WIN32
    return BCryptGenRandom(NULL, dst, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);

#else
    ssize_t result = getrandom(dst, size, 0);
    return (result == static_cast<ssize_t>(size)) ? 0 : -1;

#endif
}

int Seek(FILE *file, int64_t offset, int origin) {
#ifdef _WIN32
    return _fseeki64(file, offset, origin);

#else
    return fseeko(file, offset, origin);

#endif
}

int Unlink(const char *path) {
#ifdef _WIN32
    return _unlink(path);

#else
    return unlink(path);

#endif
}

void Wipe(void *ptr, size_t size) {
#ifdef _WIN32
    SecureZeroMemory(ptr, size);

#else
    volatile uint8_t *p = static_cast<volatile uint8_t *>(ptr);
    while (size--) *p++ = 0;

#endif
}