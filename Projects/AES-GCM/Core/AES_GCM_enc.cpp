/**
 * @file	AES_GCM_enc.cpp
 * @brief	Implementation of encryption function of AES_GCM class
 * @author	EilzDragon92
 */

#include "Core/AES_GCM.h"

int AES_GCM::encrypt(FILE *src, FILE *dst, const char *pw, size_t plen) {
	this->src = src;
	this->dst = dst;
	cancelled = false;
	prog = 0;

	hasAsyncWrite = false;

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

	// LCOV_EXCL_START
	if (size == -1) {
		reportError("[File] Size check failed - Cannot read source file size\n");
		return 1;
	}
	// LCOV_EXCL_STOP

	if (size > MAX_SIZE) {
		reportError("[File] Validation failed - File should be at most 64 GiB\n");
		return 1;
	}


	/* Generate salt and IV */

	// LCOV_EXCL_START
	if (Random(salt, SALT_SIZE)) {
		reportError("[Crypto] Random failed - Cannot generate salt\n");
		return 1;
	}

	if (Random(iv, IV_SIZE)) {
		reportError("[Crypto] Random failed - Cannot generate initial vector\n");
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


	/* Set encryption context */

	// LCOV_EXCL_START
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		reportError("[Crypto] Initialization failed - Cannot create context\n");
		return 1;
	}

	if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
		reportError("[Crypto] Initialization failed - Cannot set AES-256-GCM algorithm\n");
		return 1;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL) != 1) {
		reportError("[Crypto] Initialization failed - Cannot set initial vector size\n");
		return 1;
	}

	if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
		reportError("[Crypto] Initialization failed - Cannot set key and initial vector\n");
		return 1;
	}
	// LCOV_EXCL_STOP


	/* Write salt and IV */

	// LCOV_EXCL_START
	if (fwrite(salt, sizeof(uint8_t), SALT_SIZE, dst) != SALT_SIZE) {
		reportError("[File] Write failed - Cannot write salt to destination file header\n");
		return 1;
	}

	if (fwrite(iv, sizeof(uint8_t), IV_SIZE, dst) != IV_SIZE) {
		reportError("[File] Write failed - Cannot write initial vector to destination file header\n");
		return 1;
	}
	// LCOV_EXCL_STOP

	return 0;
}

int AES_GCM::encryptBuff(void *src, void *dst, int srcLen) {
	int dstLen;

	// LCOV_EXCL_START
	if (EVP_EncryptUpdate(ctx, static_cast<unsigned char *>(dst), &dstLen, static_cast<unsigned char *>(src), srcLen) != 1) {
		reportError("[Crypto] Encryption failed - Cannot encrypt buffer\n");
		return 1;
	}

	if (dstLen != srcLen) {
		reportError("[Crypto] Encryption failed - Cannot encrypt buffer\n");
		return 1;
	}
	// LCOV_EXCL_STOP

	return 0;
}

int AES_GCM::encryptBatch() {
	int cur = 0;

	while (prog + BUFF_SIZE * BLOCK_SIZE <= size) {
		/* Read in main thread */

		if (readTo(buff[cur].data, BUFF_SIZE * BLOCK_SIZE)) return 1;


		/* Encrypt in main thread */

		if (encryptBuff(buff[cur].data, buff[cur].data, BUFF_SIZE * BLOCK_SIZE)) return 1;


		/* Wait for the previous write to finish */

		if (hasAsyncWrite && asyncWriteResult.get() != 0) return 1;


		/* Asynchronous write in another thread */

		buff[cur].size = BUFF_SIZE * BLOCK_SIZE;

		asyncWriteResult = std::async(std::launch::async, [this, cur]() {
			return writeFrom(buff[cur].data, buff[cur].size);
		});

		hasAsyncWrite = true;


		/* Swap buffer */

		cur = 1 - cur;


		/* Update progress */

		prog += BUFF_SIZE * BLOCK_SIZE;

		if (reportProgress()) return 1;
	}


	/* Wait for the last write to finish */

	if (hasAsyncWrite) {
		if (asyncWriteResult.get() != 0) return 1;

		hasAsyncWrite = false;
	}

	return 0;
}

int AES_GCM::encryptRemain() {
	int crs = 0, rem = size % (BUFF_SIZE * BLOCK_SIZE);

	if (readTo(buff[0].data, rem)) return 1;


	/* Encrypt remaining full blocks */

	while (prog + BLOCK_SIZE <= size) {
		if (encryptBuff(buff[0].data[crs], buff[0].data[crs], BLOCK_SIZE)) return 1;

		crs++;

		prog += BLOCK_SIZE;
	}


	/* Encrypt remaining partial block */

	rem = size % BLOCK_SIZE;

	if (rem) {
		if (encryptBuff(buff[0].data[crs], buff[0].data[crs], rem)) return 1;

		prog += rem;
	}

	if (writeFrom(buff[0].data, BLOCK_SIZE * crs + rem)) return 1;


	if (reportProgress()) return 1;

	return 0;
}

int AES_GCM::encryptFinal() {
	uint8_t final[BLOCK_SIZE];
	int finalLen;

	// LCOV_EXCL_START
	if (EVP_EncryptFinal_ex(ctx, final, &finalLen) != 1) {
		reportError("[Crypto] Finalization failed - Cannot finalize encryption\n");
		return 1;
	}

	if (finalLen > 0 && writeFrom(final, finalLen)) return 1;
	// LCOV_EXCL_STOP

	return 0;
}

int AES_GCM::encryptTag() {
	uint8_t tag[TAG_SIZE];

	// LCOV_EXCL_START
	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag) != 1) {
		reportError("[Crypto] Tag Error - Cannot get authentication tag\n");
		return 1;
	}

	if (fwrite(tag, sizeof(uint8_t), TAG_SIZE, dst) != TAG_SIZE) {
		reportError("[File] Write failed - Cannot write authentication tag on destination file\n");
		return 1;
	}
	// LCOV_EXCL_STOP

	return 0;
}