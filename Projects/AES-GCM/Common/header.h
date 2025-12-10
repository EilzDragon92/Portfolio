#pragma once
#pragma comment(lib, "bcrypt.lib")

#include <QApplication>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QFileInfo>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QScreen>
#include <QStackedWidget>
#include <QThread>
#include <QWidget>

#include <argon2.h>
#include <cstdio>
#include <openssl/evp.h>
#include <string>
#include <windows.h>
#include <bcrypt.h>

#define FONT_SCALE 1.2

#define KEY_SIZE 32
#define SALT_SIZE 16
#define MEM_COST (1024 * 1024)	// Argon2id memory cost
#define TIME_COST 8				// Argon2id time cost

#define BLOCK_SIZE 16
#define BUFF_SIZE 4096	// Number of blocks in buffer
#define IV_SIZE 12		// Number of bytes in initial vector
#define TAG_SIZE 16		// Number of bytes in authentication tag

#define MAX_SIZE 16ULL * 0xffffffffULL	// Maximum file size program can handle

using std::string;

int64_t GetFileSize(FILE *file);

int Argon2id(uint8_t salt[], const char pw[], size_t plen, uint8_t key[]);

int GetProcNum();

int Random(uint8_t *dst, size_t size);

int Seek(FILE *file, int64_t offset, int origin);

int Unlink(const char *path);

void Wipe(void *ptr, size_t size);