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

	/**
	 * @brief	Destructor of AES_GCM class
	 */
	~AES_GCM();

	/**
	  * @brief		Decrypt a file
	  * @param		src		Source file
	  * @param		dst		Destination file
	  * @param		pw		Password
	  * @param		plen	Password length
	  * @return		0 on success, 1 on failure
	  */
	int decrypt(FILE *src, FILE *dst, const char *pw, int plen);

	/**
	  * @brief		Encrypt a file
	  * @param		src		Source file
	  * @param		dst		Destination file
	  * @param		pw		Password
	  * @param		plen	Password length
	  * @return		0 on success, 1 on failure
	  */
	int encrypt(FILE *src, FILE *dst, const char *pw, int plen);

	/**
	 * @brief	Set error callback function
	 * @param	ecb		Error callback function
	 */
	void setErrorCb(ErrorCallback ecb) {
		this->ecb = ecb;
	}

	/**
	 * @brief	Set progress callback function
	 * @param	pcb		Progress callback function
	 */
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


	/* ==================================================
	 * I/O helper functions
	 * ================================================== */

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


	/* ==================================================
	 * Decryption functions
	 * ================================================== */

	/**
	 * @brief	Intialize decryption context
	 * @param	pw		Password
	 * @param	plen	Password length
	 * @return	0 on success, 1 on failure
	 */
	int decryptInit(const char *pw, int plen);

	/**
	 * @brief	Read and verify authentication tag
	 * @return	0 on success, 1 on failure
	 */
	int decryptTag();

	/**
	 * @brief	Decrypt a block
	 * @param	src		Source buffer
	 * @param	dst		Destination buffer
	 * @param	srcLen	Source buffer length
	 * @return	0 on success, 1 on failure
	 */
	int decryptBlock(uint8_t *src, uint8_t *dst, int srcLen);

	/**
	 * @brief	Decrypt multiple blocks in a batch
	 * @return	0 on success, 1 on failure
	 */
	int decryptBatch();

	/**
	 * @brief	Decrypt remaining data smaller than buffer
	 * @return	0 on success, 1 on failure
	 */
	int decryptRemain();

	/**
	 * @brief	Finialize decryption
	 * @return	0 on success, 1 on failure
	 */
	int decryptFinal();


	/* ==================================================
	 * Encryption functions
	 * ================================================== */

	/**
	 * @brief	Intialize encryption context 
	 * @param	pw		Password
	 * @param	plen	Password length
	 * @return	0 on success, 1 on failure
	 */
	int encryptInit(const char *pw, int plen);

	/**
	 * @brief	Encrypt a block
	 * @param	src		Source buffer
	 * @param	dst		Destination buffer
	 * @param	srcLen	Source buffer length
	 * @return	0 on success, 1 on failure
	 */
	int encryptBlock(uint8_t *src, uint8_t *dst, int srcLen);

	/**
	 * @brief	Encrypt multiple blocks in a batch
	 * @return	0 on success, 1 on failure
	 */
	int encryptBatch();

	/**
	 * @brief	Encrypt remaining data smaller than buffer
	 * @return	0 on success, 1 on failure
	 */
	int encryptRemain();

	/**
	 * @brief	Finialize encryption
	 * @return	0 on success, 1 on failure
	 */
	int encryptFinal();

	/**
	 * @brief	Generate and write authentication tag
	 * @return	0 on success, 1 on failure
	 */
	int encryptTag();


	/* ==================================================
	 * Callback helper functions
	 * ================================================== */

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