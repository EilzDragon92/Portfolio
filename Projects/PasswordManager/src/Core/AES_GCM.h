/**
 * @file	AES_GCM.h
 * @brief	AES-GCM encryption/decryption engine
 * @author	EilzDragon92
 */

#include "Common/header.h"

class AES_GCM {
public:
	/**
	 * @brief	Callback function for error reporting
	 * @param	errMsg	Error message string
	 */
	using ErrorCallback = std::function<void(const char *errMsg)>;

	/**
	 * @brief	Default constructor of AES_GCM class
	 */
	AES_GCM();

	/**
	 * @brief	Destructor of AES_GCM class
	 */
	~AES_GCM();

	AES_GCM(const AES_GCM &) = delete;				// Delete copy constructor
	AES_GCM &operator=(const AES_GCM &) = delete;	// Delete copy assignment operator
	AES_GCM(AES_GCM &&) = delete;					// Delete move constructor
	AES_GCM &operator=(AES_GCM &&) = delete;		// Delete move assignment operator

	/**
	  * @brief		Decrypt a buffer
	  * @param		src		Source buffer
	  * @param		dst		Destination buffer
	  * @param		size	Buffer size
	  * @param		pw		Password
	  * @param		plen	Password length
	  * @return		0 on success, 1 on failure
	  */
	int decrypt(uint8_t *src, uint8_t *dst, int size, const char *pw, size_t plen);

	/**
	  * @brief		Encrypt a buffer
	  * @param		src			Source buffer
	  * @param		dst			Destination buffer
	  * @param		srcSize		Source buffer size
	  * @param		dstSize		Destination buffer size
	  * @param		pw			Password
	  * @param		plen		Password length
	  * @return		0 on success, 1 on failure
	  */
	int encrypt(uint8_t *src, uint8_t *dst, int size, const char *pw, size_t plen);

private:
	EVP_CIPHER_CTX *ctx = nullptr;	// OpenSSL encryption/decryption context

	ErrorCallback ecb = nullptr;	// Error reporting callback function

	uint8_t iv[IV_SIZE];		// Initial vector
	uint8_t key[KEY_SIZE];		// Key derived from password
	uint8_t salt[SALT_SIZE];	// Key derivation salt

	uint8_t *src;	// Source buffer
	uint8_t *dst;	// Destination buffer

	int cur = 0;	// Current read/write position in buffer
	int size = 0;	// Source buffer size


	/* ==================================================
	 * Decryption functions
	 * ================================================== */

	/**
	 * @brief	Intialize decryption context
	 * @param	pw		Password
	 * @param	plen	Password length
	 * @return	0 on success, 1 on failure
	 */
	int decryptInit(const char *pw, size_t plen);

	/**
	 * @brief	Read and verify authentication tag
	 * @return	0 on success, 1 on failure
	 */
	int decryptTag();

	/**
	 * @brief	Decrypt buffer
	 * @return	0 on success, 1 on failure
	 */
	int decryptBuff();

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
	int encryptInit(const char *pw, size_t plen);

	/**
	 * @brief	Encrypt buffer
	 * @return	0 on success, 1 on failure
	 */
	int encryptBuff();

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
	  * @brief	Report error via callback
	  * @param	msg		Error message string
	  */
	void reportError(const char *msg);
};