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
    ssize_t res;

    while (rem > 0) {
        res = getrandom(dst, rem, 0);

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

int RemoveFile(const QString &path) {
#ifdef _WIN32
    std::wstring wpath = path.toStdWString();

    return _wunlink(wpath.c_str());

#else
    QByteArray qpath = path.toUtf8();

    return unlink(qpath.constData());

#endif
}

int Seek(FILE *file, int64_t offset, int origin) {
#ifdef _WIN32
    return _fseeki64(file, offset, origin);

#else
    return fseeko(file, offset, origin);

#endif
}

void OpenFile(FILE **file, const QString &path, const char *mode) {
#ifdef _WIN32
    std::wstring wpath = path.toStdWString();
    std::wstring wmode;
    
    for (const char *p = mode; *p; ++p) wmode += static_cast<wchar_t>(*p);

    *file = _wfopen(wpath.c_str(), wmode.c_str());

#else
    QByteArray qpath = path.toUtf8();

    *file = fopen(qpath.constData(), mode);

#endif
}

void Wipe(void *ptr, size_t size) {
#ifdef _WIN32
    SecureZeroMemory(ptr, size);

#elif defined(__GLIBC__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 25
    explicit_bzero(ptr, size);

#else
    volatile uint8_t *p = static_cast<volatile uint8_t *>(ptr);
    while (size--) *p++ = 0;
    __asm__ __volatile__("" : : "r"(ptr) : "memory");
#endif
}