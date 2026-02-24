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

uint32_t RandomRange(uint32_t min, uint32_t max) {
    uint32_t range = max - min + 1, limit = UINT32_MAX - UINT32_MAX % range;
    uint32_t tmp;

    do {
        Random(reinterpret_cast<uint8_t *>(&tmp), sizeof(uint32_t));
    } while (tmp >= limit);

    return min + tmp % range;
}

bool FileExists(const QString &path) {
#ifdef _WIN32
    std::wstring wpath = path.toStdWString();

    return std::filesystem::exists(wpath);

#else
    QByteArray qpath = path.toUtf8();

    return std::filesystem::exists(qpath.constData());

#endif
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
    size_t rem = size;

    while (rem > 0) {
        ssize_t res = getrandom(dst, rem, 0);

        if (res == -1) {
            if (errno == EINTR) continue;

            return -1;
        }

        dst += res;
        rem -= res;
    }

    return 0;

#endif
}

void Lock(void *buff, size_t size) {
#ifdef _WIN32
    VirtualLock(buff, size);

#else
    mlock(buff, size);

#endif
}

void OpenFile(FILE **file, const QString &path, const char *mode) {
#ifdef _WIN32
    std::wstring wpath = path.toStdWString();
    std::wstring wmode;

    for (const char *p = mode; *p; ++p) wmode += static_cast<wchar_t>(*p);

    _wfopen_s(file, wpath.c_str(), wmode.c_str());

#else
    QByteArray qpath = path.toUtf8();

    *file = fopen(qpath.constData(), mode);

#endif
}

void Shuffle(uint8_t *arr, int size) {
    for (int i = 0; i < size; i++) Swap(&arr[i], &arr[RandomRange(i, size - 1)]);
}

void Swap(uint8_t *a, uint8_t *b) {
    uint8_t tmp = *a;
    *a = *b;
    *b = tmp;
}

void Unlock(void *buff, size_t size) {
#ifdef _WIN32
    VirtualUnlock(buff, size);

#else
    munlock(buff, size);

#endif
}

void Wipe(void *buff, size_t size) {
#ifdef _WIN32
    SecureZeroMemory(buff, size);

#elif defined(__GLIBC__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 25
    explicit_bzero(buff, size);

#else
    volatile uint8_t *p = static_cast<volatile uint8_t *>(buff);

    while (size--) *p++ = 0;

    __asm__ __volatile__("" : : "r"(buff) : "memory");

#endif
}