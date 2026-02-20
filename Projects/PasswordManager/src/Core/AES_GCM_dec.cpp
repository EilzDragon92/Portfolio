/**
 * @file	AES_GCM_dec.cpp
 * @brief	Implementation of decryption functions AES_GCM class
 * @author	EilzDragon92
 */

#include "Core/AES_GCM.h"

int AES_GCM::decrypt(uint8_t *src, uint8_t *dst, size_t size, const char *pw, size_t plen) {
	this->src = src, this->dst = dst, this->size = size;
	srcCrs = 0, dstCrs = 0;

	if (decryptInit(pw, plen)) return 1;

	if (decryptTag()) return 1;

	if (decryptBuff()) return 1;

	if (decryptFinal()) return 1;

	return 0;
}

int AES_GCM::decryptInit(const char *pw, size_t plen) {
	/* Clear existing context */

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
		ctx = nullptr;
	}


	/* Read salt and IV */

	memcpy(salt, src + srcCrs, SALT_SIZE);
	srcCrs += SALT_SIZE;

	memcpy(iv, src + srcCrs, IV_SIZE);
	srcCrs += IV_SIZE;


	/* Derive key from password */

	if (Argon2id(salt, pw, plen, key)) {
		// LCOV_EXCL_START
		reportError("[Crypto] Key derivation failed - Argon2id error\n");
		return 1;
		// LCOV_EXCL_STOP
	}


	/* Set decryption context */

	if (!(ctx = EVP_CIPHER_CTX_new())) {
		// LCOV_EXCL_START
		reportError("[Crypto] Initialization failed - Cannot create context\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
		// LCOV_EXCL_START
		reportError("[Crypto] Initialization failed - Cannot set AES-256-GCM algorithm\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL) != 1) {
		// LCOV_EXCL_START
		reportError("[Crypto] Initialization failed - Cannot set initial vector size\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
		// LCOV_EXCL_START
		reportError("[Crypto] Initialization failed - Cannot set key and initial vector\n");
		return 1;
		// LCOV_EXCL_STOP
	}


	return 0;
}

int AES_GCM::decryptTag() {
	uint8_t tag[TAG_SIZE];

	memcpy(tag, src + size - TAG_SIZE, TAG_SIZE);

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag) != 1) {
		// LCOV_EXCL_START
		reportError("[Crypto] Tag failed - Cannot set authentication tag\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	return 0;
}

int AES_GCM::decryptBuff() {
	int len = size - SALT_SIZE - IV_SIZE - TAG_SIZE;
	int res;

	if (EVP_DecryptUpdate(ctx, dst, &res, src + srcCrs, len) != 1) {
		// LCOV_EXCL_START
		reportError("[Crypto] Encryption failed - Cannot encrypt buffer\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	if (res != len) {
		// LCOV_EXCL_START
		reportError("[Crypto] Encryption failed - Cannot encrypt buffer\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	dstCrs += len;

	return 0;
}

int AES_GCM::decryptFinal() {
	uint8_t final[BLOCK_SIZE];
	int finalLen;

	if (EVP_DecryptFinal_ex(ctx, final, &finalLen) != 1) {
		// LCOV_EXCL_START
		reportError("[Crypto] Finalization failed - Cannot finalize encryption\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	if (finalLen > 0) return 1;

	memcpy(dst + dstCrs, final, finalLen);
	dstCrs += finalLen;

	return 0;
}