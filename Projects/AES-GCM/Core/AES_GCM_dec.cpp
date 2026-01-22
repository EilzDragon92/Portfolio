/**
 * @file	AES_GCM_dec.cpp
 * @brief	Implementation of decryption function of AES_GCM class
 * @author	EilzDragon92
 */

#include "Core/AES_GCM.h"

int AES_GCM::decrypt(FILE *src, FILE *dst, const char *pw, size_t plen) {
	this->src = src;
	this->dst = dst;
	cancelled = false;
	cur = 0;

	if (decryptInit(pw, plen)) return 1;

	if (decryptTag()) return 1;

	if (decryptBatch()) return 1;

	if (decryptRemain()) return 1;

	if (decryptFinal()) return 1;

	return 0;
}

int AES_GCM::decryptInit(const char *pw, size_t plen) {
	/* Clear existing context */

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
		ctx = nullptr;
	}


	/* Get source file size */

	size = GetFileSize(src);

	// LCOV_EXCL_START
	if (size == -1) {
		reportError("[File] Size check failed - Cannot read source file size\n");
		return 1;
	}
	// LCOV_EXCL_STOP

	if (size < SALT_SIZE + IV_SIZE + TAG_SIZE) {
		reportError("[File] Validation failed - File should be at least 44 bytes\n");
		return 1;
	}

	size -= SALT_SIZE + IV_SIZE + TAG_SIZE;


	/* Read salt and IV */

	// LCOV_EXCL_START
	if (fread(salt, sizeof(uint8_t), SALT_SIZE, src) != SALT_SIZE) {
		reportError("[File] Read failed - Cannot read salt from source file header\n");
		return 1;
	}

	if (fread(iv, sizeof(uint8_t), IV_SIZE, src) != IV_SIZE) {
		reportError("[File] Read failed - Cannot read initial vector from source file header\n");
		return 1;
	}
	// LCOV_EXCL_STOP


	/* Derive key from password */

	// LCOV_EXCL_START
	if (Argon2id(salt, pw, plen, key)) {
		reportError("[Crypto] Key derivation failed - Argon2id error\n");
		return 1;
	}
	// LCOV_EXCL_STOP


	/* Set decryption context */

	// LCOV_EXCL_START
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		reportError("[Crypto] Initialization failed - Cannot create context\n");
		return 1;
	}

	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
		reportError("[Crypto] Initialization failed - Cannot set AES-256-GCM algorithm\n");
		return 1;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL) != 1) {
		reportError("[Crypto] Initialization failed - Cannot set initial vector size\n");
		return 1;
	}

	if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
		reportError("[Crypto] Initialization failed - Cannot set key and initial vector\n");
		return 1;
	}
	// LCOV_EXCL_STOP

	return 0;
}

int AES_GCM::decryptTag() {
	uint8_t tag[TAG_SIZE];

	// LCOV_EXCL_START
	if (Seek(src, -TAG_SIZE, SEEK_END)) {
		reportError("[File] Seek failed - Cannot move file pointer to authentication tag\n");
		return 1;
	}

	if (fread(tag, sizeof(uint8_t), TAG_SIZE, src) != TAG_SIZE) {
		reportError("[File] Read failed - Cannot read authentication tag\n");
		return 1;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag) != 1) {
		reportError("[Crypto] Tag failed - Cannot set authentication tag\n");
		return 1;
	}

	if (Seek(src, SALT_SIZE + IV_SIZE, SEEK_SET)) {
		reportError("[File] Seek failed - Cannot move file pointer to data\n");
		return 1;
	}
	// LCOV_EXCL_STOP

	return 0;
}

int AES_GCM::decryptBuff(void *src, void *dst, int srcLen) {
	int dstLen;

	// LCOV_EXCL_START
	if (EVP_DecryptUpdate(ctx, static_cast<unsigned char *>(dst), &dstLen, static_cast<unsigned char *>(src), srcLen) != 1) {
		reportError("[Crypto] Decryption failed - Cannot decrypt block\n");
		return 1;
	}

	if (dstLen != srcLen) {
		reportError("[Crypto] Decryption failed - Cannot decrypt block\n");
		return 1;
	}
	// LCOV_EXCL_STOP

	return 0;
}

int AES_GCM::decryptBatch() {
	while (cur + BUFF_SIZE * BLOCK_SIZE <= size) {
		if (readTo(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		if (decryptBuff(buff, buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		if (writeFrom(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		cur += BUFF_SIZE * BLOCK_SIZE;

		if (reportProgress()) return 1;
	}

	return 0;
}

int AES_GCM::decryptRemain() {
	int crs = 0, rem = size % (BUFF_SIZE * BLOCK_SIZE);

	if (readTo(buff, rem)) return 1;


	/* Decrypt remaining full blocks */

	while (cur + BLOCK_SIZE <= size) {
		if (decryptBuff(buff[crs], buff[crs], BLOCK_SIZE)) return 1;

		crs++;

		cur += BLOCK_SIZE;
	}


	/* Decrypt remaining partial block */

	rem = size % BLOCK_SIZE;

	if (rem) {
		if (decryptBuff(buff[crs], buff[crs], rem)) return 1;

		cur += rem;
	}


	if (writeFrom(buff, BLOCK_SIZE * crs + rem)) return 1;

	if (reportProgress()) return 1;

	return 0;
}

int AES_GCM::decryptFinal() {
	uint8_t final[BLOCK_SIZE];
	int finalLen;

	if (EVP_DecryptFinal_ex(ctx, final, &finalLen) != 1) {
		reportError("[Auth] Verification failed - Invalid password or corrupted file\n");
		return 1;
	}

	if (finalLen > 0 && writeFrom(final, finalLen)) return 1;  // LCOV_EXCL_LINE

	return 0;
}