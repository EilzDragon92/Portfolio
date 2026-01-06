/**
 * @file	header.h
 * @brief	Common header file for AES-GCM file encryption/decryption application
 * @author	EilzDragon92
 * 
 * Contents:
 *	- Libraries
 *	- Macros
 *	- Utility function declarations
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
	#include <cerrno>
	#include <sys/random.h>
	#include <thread>
	#include <unistd.h>

#endif

#define FONT_SCALE 1.2	/// GUI font scale

#define KEY_SIZE 32				/// AES-GCM key size in bytes
#define SALT_SIZE 16			/// Argon2id salt size in bytes
#define MEM_COST (512 * 1024)	/// Argon2id memory cost in KiB
#define TIME_COST 4				/// Argon2id time cost 

#define BLOCK_SIZE 16	/// AES-GCM block size in bytes
#define BUFF_SIZE 4096	/// Buffer size in blocks (64 KiB)
#define IV_SIZE 12		/// Initial vector size in bytes
#define TAG_SIZE 16		/// Authentication tag size in bytes

#define MAX_SIZE 16ULL * 0xffffffffULL	/// Maximum source file size that can be processed (64 GB)

/**
 * @brief	Get the size of a file in bytes
 * @param	file	File pointer in read binary mode
 * @return	file size in bytes on success, -1 on failure
 */
int64_t GetFileSize(FILE *file);

/**
 * @brief	Derive a key using Argon2id
 * @param	salt	Input buffer for salt
 * @param	pw		Input buffer for password
 * @param	plen	Password length in bytes
 * @param	key		Output buffer for key
 * @return	0 on success, non-zero on failure
 */
int Argon2id(uint8_t salt[], const char pw[], size_t plen, uint8_t key[]);

/**
 * @brief	Get the number of processors available
 * @return	Number of logical processors available
 */
int GetProcNum();

/**
 * @brief	Generates cryptographically secure random bytes
 * @param	dst		Output buffer for random bytes
 * @param	size	Output buffer size
 * @return	0 on success, non-zero on failure
 */
int Random(uint8_t *dst, size_t size);

/**
 * @brief	Delete a file
 * @param	path	File path
 * @return	0 on success, non-zero on failure
 */
int RemoveFile(const char *path);

/**
 * @brief	Move file pointer to specific position
 * @param	file	File pointer
 * @param	dist	Distance from reference point
 * @param	ref		Reference point
 * @return	0 on success, non-zero on failure
 */
int Seek(FILE *file, int64_t dist, int ref);

/**
 * @brief	Open a file
 * @param	file	File pointer
 * @param	path	File path
 * @param	mode	Mode
 */
void OpenFile(FILE **file, const char *path, const char *mode);

/**
 * @brief	Securely wipe data in memory
 * @param	buff	Buffer to wipe
 * @param	size	Buffer size in bytes
 */
void Wipe(void *buff, size_t size);