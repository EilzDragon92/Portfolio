/**
 * @file	header.h
 * @brief	Common header file for includes, macros, utility function declarations
 * @author	EilzDragon92
 */

#pragma once

/* Qt framework libraries */
#include <QString>

/* System libraries */
#include <argon2.h>
#include <cstdio>
#include <filesystem>

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
	#include <sys/mman.h>
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
#define BUFF_NUM 2		/// Number of buffers for swapping
#define BUFF_SIZE 4096	/// Buffer size in blocks
#define IV_SIZE 12		/// Initial vector size in bytes
#define TAG_SIZE 16		/// Authentication tag size in bytes

/**
 * @brief	Check a file exists
 * @param	path	File path
 * @return	1 if file exists, 0 if file not exists
 */
bool FileExists(const QString &path);

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
 * @brief	Lock memory space so that it cannot be swapped
 * @param	buff	Buffer to lock
 * @param	size	Buffer size in bytes
 */
void Lock(void *ptr, size_t size);

/**
 * @brief	Open a file
 * @param	file	File pointer
 * @param	path	File path
 * @param	mode	Mode
 */
void OpenFile(FILE **file, const QString &path, const char *mode);

/**
 * @brief	Unlock memory space so that it can be swapped
 * @param	buff	Buffer to unlock
 * @param	size	Buffer size in bytes
 */
void Unlock(void *ptr, size_t size);

/**
 * @brief	Securely wipe data in memory
 * @param	buff	Buffer to wipe
 * @param	size	Buffer size in bytes
 */
void Wipe(void *buff, size_t size);