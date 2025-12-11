/**
 * @file	AES_GCM.h
 * @brief	AES-256-GCM file encryption/decryption engine
 * @author	EilzDragon92
 *
 * Contents:
 *	- Definition of AES_GCM class
 *	- Implementation of callback setting functions
 */

#pragma once

#include "Common/header.h"

 /**
  * @class	AES_GCM
  * @brief	AES-256-GCM file encryption/decryption engine
  */
class AES_GCM {
public:
	/**
	 * @brief	Callback function for error reporting
	 * @param	errMsg	Error message string
	 */
	using ErrorCallback = std::function<void(const char *errMsg)>;

	/**
	 * @brief	Callback function for progress reporting
	 * @param	perc		Current progress in percentage
	 * @param	cancelled	Pointer of cancellation flag
	 */
	using ProgressCallback = std::function<void(int perc, bool *cancelled)>;

	/// Destructor of AES_GCM class
	~AES_GCM();

	int decrypt(FILE *src, FILE *dst, const char *pw, int plen);
	int encrypt(FILE *src, FILE *dst, const char *pw, int plen);

	void setErrorCb(ErrorCallback ecb) {
		this->ecb = ecb;
	}

	void setProgressCb(ProgressCallback pcb) {
		this->pcb = pcb;
	}

private:
	EVP_CIPHER_CTX *ctx = nullptr;	// OpenSSL encryption/decryption context

	FILE *src;	// Source file
	FILE *dst;	// Destination file

	ErrorCallback ecb = nullptr;		// Error reporting callback function
	ProgressCallback pcb = nullptr;		// Progress reporting callback function

	uint64_t cur = 0;	// Current bytes processed
	uint64_t size;		// Source file size

	uint8_t buff[BUFF_SIZE][BLOCK_SIZE];	// Buffer
	uint8_t iv[IV_SIZE];					// Initial vector
	uint8_t key[KEY_SIZE];					// Key derived from password
	uint8_t salt[SALT_SIZE];				// Key derivation salt

	std::atomic<bool> cancelled{ false };	// Cancellation flag


	/* ====================
	 * I/O helper functions
	 * ==================== */

	/**
	  * @brief	Read data from source file into buffer
	  * @param	buff	Destination buffer
	  * @param	size	Number of bytes to read
	  * @return	0 on success, 1 on failure
	  */
	int readBuffer(void *buff, int size);

	/**
	  * @brief	Write data from buffer to destination file
	  * @param	buff	Source buffer
	  * @param	size	Number of bytes to write
	  * @return	0 on success, 1 on failure
	  */
	int writeBuffer(void *buff, int size);


	/* ====================
	 * Decryption functions
	 * ==================== */

	int decryptInit(const char *pw, int plen);
	int decryptTag();
	int decryptBlock(uint8_t *src, uint8_t *dst, int srcLen);
	int decryptBatch();
	int decryptRemain();
	int decryptFinal();


	/* ====================
	 * Encryption functions
	 * ==================== */

	int encryptInit(const char *pw, int plen);
	int encryptBlock(uint8_t *src, uint8_t *dst, int srcLen);
	int encryptBatch();
	int encryptRemain();
	int encryptFinal();
	int encryptTag();


	/* =========================
	 * Callback helper functions
	 * ========================= */

	 /**
	   * @brief		Report current progress via callback
	   * @return	0 on continue, 1 on cancelled
	   */
	int reportProgress();

	/**
	  * @brief	Report error via callback
	  * @param	msg		Error message string
	  */
	void reportError(const char *msg);
};