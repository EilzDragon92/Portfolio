#include "Common/header.h"

/**
 * @brief	Get the size of a file in bytes
 * @param	file	File pointer in read binary mode
 * @return	file size in bytes on success, -1 on failure
 */
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

/**
 * @brief	Derive a key using Argon2id
 * @param	salt	Input buffer for salt
 * @param	pw		Input buffer for password
 * @param	plen	Password length in bytes
 * @param	key		Output buffer for key
 * @return	0 on success, non-zero on failure
 */
int Argon2id(uint8_t salt[], const char pw[], size_t plen, uint8_t key[]) {
    return argon2id_hash_raw(TIME_COST, MEM_COST, GetProcNum(), pw, plen, salt, SALT_SIZE, key, KEY_SIZE);
}

/**
 * @brief	Get the number of processors available
 * @return	Number of logical processors available
 */
int GetProcNum() {
#ifdef _WIN32
    SYSTEM_INFO systemInfo;

    GetSystemInfo(&systemInfo);

    return systemInfo.dwNumberOfProcessors;

#else
    return std::thread::hardware_concurrency();

#endif
}

/**
 * @brief	Generates cryptographically secure random bytes
 * @param	dst		Output buffer for random bytes
 * @param	size	Output buffer size
 */
int Random(uint8_t *dst, size_t size) {
#ifdef _WIN32
    return BCryptGenRandom(NULL, dst, size, BCRYPT_USE_SYSTEM_PREFERRED_RNG);

#else
    ssize_t result = getrandom(dst, size, 0);
    return (result == static_cast<ssize_t>(size)) ? 0 : -1;

#endif
}

/**
 * @brief	Move file pointer to specific position
 * @param	file	File pointer
 * @param	dist	Distance from reference point
 * @param	ref		Reference point
 * @return	0 on success, non-zero on failure
 */
int Seek(FILE *file, int64_t offset, int origin) {
#ifdef _WIN32
    return _fseeki64(file, offset, origin);

#else
    return fseeko(file, offset, origin);

#endif
}

/**
 * @brief	Delete a file
 * @param	path	Path of the file
 * @return	0 on success, non-zero on failure
 */
int Unlink(const char *path) {
#ifdef _WIN32
    return _unlink(path);

#else
    return unlink(path);

#endif
}

/**
 * @brief	Securely wipe data in memory
 * @param	buff	Buffer to wipe
 * @param	size	Buffer size in bytes
 */
void Wipe(void *ptr, size_t size) {
#ifdef _WIN32
    SecureZeroMemory(ptr, size);

#else
    volatile uint8_t *p = static_cast<volatile uint8_t *>(ptr);
    while (size--) *p++ = 0;

#endif
}