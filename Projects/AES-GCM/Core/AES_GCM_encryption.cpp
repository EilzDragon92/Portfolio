/**
 * @file	AES_GCM_encryption.cpp
 * @brief	Implementation of encryption function of AES_GCM class
 * @author	EilzDragon92
 */

#include "Core/AES_GCM.h"

int AES_GCM::encrypt(FILE *src, FILE *dst, const char *pw, size_t plen) {
	this->src = src;
	this->dst = dst;
	cancelled = false;
	cur = 0;

	if (encryptInit(pw, plen)) return 1;

	if (encryptBatch()) return 1;

	if (encryptRemain()) return 1;

	if (encryptFinal()) return 1;

	if (encryptTag()) return 1;

	return 0;
}

int AES_GCM::encryptInit(const char *pw, size_t plen) {
	/* Clear existing context */

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
		ctx = nullptr;
	}


	/* Get source file size */

	size = GetFileSize(src);

	if (size == -1) {
		reportError("ERROR: Failed to read file size\n");
		return 1;
	}

	if (size > MAX_SIZE) {
		reportError("ERROR: File is too large\n");
		return 1;
	}


	/* Generate salt and IV */

	if (Random(salt, SALT_SIZE)) {
		reportError("ERROR: Failed to generate salt\n");
		return 1;
	}

	if (Random(iv, IV_SIZE)) {
		reportError("ERROR: Failed to generate initial vector\n");
		return 1;
	}


	/* Derive key from password */

	if (Argon2id(salt, pw, plen, key)) {
		reportError("ERROR: Failed to derive key\n");
		return 1;
	}


	/* Set encryption context */

	if (!(ctx = EVP_CIPHER_CTX_new())) {
		reportError("ERROR: Failed to create context\n");
		return 1;
	}

	if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
		reportError("ERROR: Failed to set algorithm\n");
		return 1;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL) != 1) {
		reportError("ERROR: Failed to set initial vector size\n");
		return 1;
	}

	if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
		reportError("ERROR: Failed to set key and initial vector\n");
		return 1;
	}


	/* Write salt and IV */

	if (fwrite(salt, sizeof(uint8_t), SALT_SIZE, dst) != SALT_SIZE) {
		reportError("ERROR: Failed to write salt\n");
		return 1;
	}

	if (fwrite(iv, sizeof(uint8_t), IV_SIZE, dst) != IV_SIZE) {
		reportError("ERROR: Failed to write initial vector\n");
		return 1;
	}

	return 0;
}

int AES_GCM::encryptBlock(uint8_t *src, uint8_t *dst, int srcLen) {
	int dstLen;

	if (EVP_EncryptUpdate(ctx, dst, &dstLen, src, srcLen) != 1) {
		reportError("ERROR: Failed to encrypt a block\n");
		return 1;
	}

	if (dstLen != srcLen) {
		reportError("ERROR: Failed to encrypt a block\n");
		return 1;
	}

	return 0;
}

int AES_GCM::encryptBatch() {
	while (cur + BUFF_SIZE * BLOCK_SIZE <= size) {
		if (readBuffer(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		for (int i = 0; i < BUFF_SIZE; i++) {
			if (encryptBlock(buff[i], buff[i], BLOCK_SIZE)) return 1;
		}

		if (writeBuffer(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		cur += BUFF_SIZE * BLOCK_SIZE;
	}

	if (reportProgress()) return 1;

	return 0;
}

int AES_GCM::encryptRemain() {
	int crs = 0, rem = size % (BUFF_SIZE * BLOCK_SIZE);

	if (readBuffer(buff, rem)) return 1;


	/* Encrypt remaining full blocks */

	while (cur + BLOCK_SIZE <= size) {
		if (encryptBlock(buff[crs], buff[crs], BLOCK_SIZE)) return 1;

		crs++;

		cur += BLOCK_SIZE;
	}


	/* Encrypt remaining partial block */

	rem = size % BLOCK_SIZE;

	if (rem) {
		if (encryptBlock(buff[crs], buff[crs], rem)) return 1;

		cur += rem;
	}


	if (writeBuffer(buff, BLOCK_SIZE * crs + rem)) return 1;

	if (reportProgress()) return 1;

	return 0;
}

int AES_GCM::encryptFinal() {
	uint8_t final[BLOCK_SIZE];
	int finalLen;

	if (EVP_EncryptFinal_ex(ctx, final, &finalLen) != 1) {
		reportError("ERROR: Failed to finalize encryption\n");
		return 1;
	}

	if (finalLen > 0 && writeBuffer(final, finalLen)) return 1;

	return 0;
}

int AES_GCM::encryptTag() {
	uint8_t tag[TAG_SIZE];

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag) != 1) {
		reportError("ERROR: Failed to get authentication tag\n");
		return 1;
	}

	if (fwrite(tag, sizeof(uint8_t), TAG_SIZE, dst) != TAG_SIZE) {
		reportError("ERROR: Failed to write authentication tag\n");
		return 1;
	}

	return 0;
}