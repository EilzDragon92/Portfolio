/**
 * @file	header.h
 * @brief	Common header file for AES-GCM file encryption/decryption application
 * @author	EilzDragon92
 *
 * This file contains libraries, macros, and utility function declarations
 * This file is included in all the other files
 */

#pragma once

/* Qt framework libraries */
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

/* System libraries */
#include <argon2.h>
#include <cstdio>
#include <filesystem>
#include <openssl/evp.h>

/* String libraries */
#include <cstring>
#include <string>

/* OS-specific libraries */
#ifdef _WIN32
	#pragma comment(lib, "bcrypt.lib")
	#include <windows.h>
	#include <bcrypt.h>
#else
	#include <sys/random.h>
	#include <unistd.h>
	#include <thread>
#endif

#define FONT_SCALE 1.2	/// GUI font scale

#define KEY_SIZE 32				/// AES-GCM key size in bytes
#define SALT_SIZE 16			/// Argon2id salt size in bytes
#define MEM_COST (1024 * 1024)	/// Argon2id memory cost in KiB
#define TIME_COST 8				/// Argon2id time cost 

#define BLOCK_SIZE 16	/// AES-GCM block size in bytes
#define BUFF_SIZE 4096	/// Buffer size in blocks (64 KiB)
#define IV_SIZE 12		/// Initial vector size in bytes
#define TAG_SIZE 16		/// Authentication tag size in bytes

#define MAX_SIZE 16ULL * 0xffffffffULL	/// Maximum source file size that can be processed (64 GB)

using std::string;

int64_t GetFileSize(FILE *file);

int Argon2id(uint8_t salt[], const char pw[], size_t plen, uint8_t key[]);

int GetProcNum();

int Random(uint8_t *dst, size_t size);

int Seek(FILE *file, int64_t dist, int ref);

int Unlink(const char *path);

void Wipe(void *buff, size_t size);