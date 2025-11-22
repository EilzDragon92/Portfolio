#include "AES_GCM.h"
#include "header.h"

AES_GCM::~AES_GCM() {
	SecureZeroMemory(buff, sizeof(uint8_t) * BUFF_SIZE * BLOCK_SIZE);
	SecureZeroMemory(iv, sizeof(uint8_t) * IV_SIZE);
	SecureZeroMemory(key, sizeof(uint8_t) * KEY_SIZE);
	SecureZeroMemory(salt, sizeof(uint8_t) * SALT_SIZE);

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
		ctx = NULL;
	}
}

int AES_GCM::encrypt(FILE *src, FILE *dst, const char *pw) {
	this->src = src, this->dst = dst;
	cancelled = false;

	if (encryptInit(pw)) return 1;

	if (encryptBatch()) return 1;

	if (encryptRemain()) return 1;

	if (encryptFinal()) return 1;

	if (encryptTag()) return 1;

	return 0;
}

int AES_GCM::decrypt(FILE *src, FILE *dst, const char *pw) {
	this->src = src, this->dst = dst;
	cancelled = false;

	if (decryptInit(pw)) return 1;

	if (decryptTag()) return 1;

	if (decryptBatch()) return 1;

	if (decryptRemain()) return 1;

	if (decryptFinal()) return 1;

	return 0;
}

int AES_GCM::readBuffer(void *buff, int size) {
	if (fread(buff, sizeof(uint8_t), size, src) != size) {
		reportError("ERROR: Failed to read file\n");
		return 1;
	}

	return 0;
}

int AES_GCM::writeBuffer(void *buff, int size) {
	if (fwrite(buff, sizeof(uint8_t), size, dst) != size) {
		reportError("ERROR: Failed to write file\n");
		return 1;
	}

	return 0;
}

int AES_GCM::encryptInit(const char *pw) {
	/* Generate salt, IV, and derive key */

	if (Random(salt, SALT_SIZE)) {
		reportError("ERROR: Failed to generate salt\n");
		return 1;
	}

	if (Random(iv, IV_SIZE)) {
		reportError("ERROR: Failed to generate initial vector\n");
		return 1;
	}

	if (Argon2id(salt, pw, key)) {
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


	/* Get source file size */

	size = GetFileSize(src);

	if (size == -1) {
		reportError("ERROR: Failed to read file size\n");
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

		if (reportProgress()) return 1;
	}

	return 0;
}

int AES_GCM::encryptRemain() {
	int crs = 0, rem = size % BLOCK_SIZE;

	while (cur + BLOCK_SIZE <= size) {
		if (readBuffer(buff[crs], BLOCK_SIZE)) return 1;

		if (encryptBlock(buff[crs], buff[crs], BLOCK_SIZE)) return 1;

		crs++;

		cur += BLOCK_SIZE;

		if (reportProgress()) return 1;
	}

	if (rem) {
		if (readBuffer(buff[crs], rem)) return 1;

		if (encryptBlock(buff[crs], buff[crs], rem)) return 1;

		cur += rem;

		if (reportProgress()) return 1;
	}

	if (writeBuffer(buff, BLOCK_SIZE * crs + rem)) return 1;

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

int AES_GCM::decryptInit(const char *pw) {
	/* Read salt, IV, and derive key */

	if (fread(salt, sizeof(uint8_t), SALT_SIZE, src) != SALT_SIZE) {
		reportError("ERROR: Failed to read salt\n");
		return 1;
	}

	if (fread(iv, sizeof(uint8_t), IV_SIZE, src) != IV_SIZE) {
		reportError("ERROR: Failed to read initial vector\n");
		return 1;
	}

	if (Argon2id(salt, pw, key)) {
		reportError("ERROR: Failed to derive key\n");
		return 1;
	}


	/* Set decryption context */

	if (!(ctx = EVP_CIPHER_CTX_new())) {
		reportError("ERROR: Failed to create context\n");
		return 1;
	}

	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
		reportError("ERROR: Failed to set algorithm\n");
		return 1;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL) != 1) {
		reportError("ERROR: Failed to set initial vector size\n");
		return 1;
	}

	if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
		reportError("ERROR: Failed to set key and initial vector\n");
		return 1;
	}


	/* Get source file size */

	size = GetFileSize(src);

	if (size == -1) {
		reportError("ERROR: Failed to read file size\n");
		return 1;
	}

	size -= SALT_SIZE + IV_SIZE + TAG_SIZE;

	return 0;
}

int AES_GCM::decryptTag() {
	uint8_t tag[TAG_SIZE];

	if (_fseeki64(src, -TAG_SIZE, SEEK_END)) {
		reportError("ERROR: Failed to move file pointer\n");
		return 1;
	}

	if (fread(tag, sizeof(uint8_t), TAG_SIZE, src) != TAG_SIZE) {
		reportError("ERROR: Failed to read authentication tag\n");
		return 1;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag) != 1) {
		reportError("ERROR: Failed to set authentication tag\n");
		return 1;
	}

	if (_fseeki64(src, SALT_SIZE + IV_SIZE, SEEK_SET)) {
		reportError("ERROR: Failed to reset file pointer\n");
		return 1;
	}

	return 0;
}

int AES_GCM::decryptBlock(uint8_t *src, uint8_t *dst, int srcLen) {
	int dstLen;

	if (EVP_DecryptUpdate(ctx, dst, &dstLen, src, srcLen) != 1) {
		reportError("ERROR: Failed to decrypt a block\n");
		return 1;
	}

	if (dstLen != srcLen) {
		reportError("ERROR: Failed to decrypt a block\n");
		return 1;
	}

	return 0;
}

int AES_GCM::decryptBatch() {
	while (cur + BUFF_SIZE * BLOCK_SIZE <= size) {
		if (readBuffer(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		for (int i = 0; i < BUFF_SIZE; i++) {
			if (decryptBlock(buff[i], buff[i], BLOCK_SIZE)) return 1;
		}

		if (writeBuffer(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		cur += BUFF_SIZE * BLOCK_SIZE;

		if (reportProgress()) return 1;
	}

	return 0;
}

int AES_GCM::decryptRemain() {
	int crs = 0, rem = size % BLOCK_SIZE;

	while (cur + BLOCK_SIZE <= size) {
		if (readBuffer(buff[crs], BLOCK_SIZE)) return 1;

		if (decryptBlock(buff[crs], buff[crs], BLOCK_SIZE)) return 1;

		crs++;

		cur += BLOCK_SIZE;

		if (reportProgress()) return 1;
	}

	if (rem) {
		if (readBuffer(buff[crs], rem)) return 1;

		if (decryptBlock(buff[crs], buff[crs], rem)) return 1;

		cur += rem;

		if (reportProgress()) return 1;
	}

	if (writeBuffer(buff, BLOCK_SIZE * crs + rem)) return 1;

	return 0;
}

int AES_GCM::decryptFinal() {
	uint8_t final[BLOCK_SIZE];
	int finalLen;

	if (EVP_DecryptFinal_ex(ctx, final, &finalLen) != 1) {
		reportError("ERROR: Failed to finalize decryption\n");
		return 1;
	}

	if (finalLen > 0 && writeBuffer(final, finalLen)) return 1;

	return 0;
}

int AES_GCM::reportProgress() {
	if (pcb) {
		pcb(cur * 100 / size, &cancelled);

		if (cancelled) return 1;
	}

	return 0;
}

void AES_GCM::reportError(const char *msg) {
	if (ecb) ecb(msg);
}