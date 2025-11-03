#include "aes-gcm.h"
#include "header.h"

#define AAD_LEN (IV_SIZE + SALT_SIZE)
#define BLOCK_SIZE 16
#define BUFF_SIZE 4096
#define IV_SIZE 12
#define TAG_SIZE 16

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
	srcFile = src, dstFile = dst;

	if (encryptInit(pw)) return 1;

	if (encryptAAD()) return 1;

	if (encryptBatch()) return 1;

	if (encryptRemain()) return 1;

	if (encryptFinal()) return 1;

	if (encryptTag()) return 1;

	return 0;
}

int AES_GCM::decrypt(FILE *src, FILE *dst, const char *pw) {
	srcFile = src, dstFile = dst;

	if (decryptInit(pw)) return 1;

	if (decryptAAD()) return 1;

	if (decryptTag()) return 1;

	if (decryptBatch()) return 1;

	if (decryptRemain()) return 1;

	if (decryptFinal()) return 1;

	return 0;
}

int AES_GCM::readBuffer(void *buff, int size) {
	if (fread(buff, sizeof(uint8_t), size, srcFile) != size) {
		printf("ERROR: Failed to read file\n");
		return 1;
	}

	return 0;
}

int AES_GCM::writeBuffer(void *buff, int size) {
	if (fwrite(buff, sizeof(uint8_t), size, dstFile) != size) {
		printf("ERROR: Failed to write file\n");
		return 1;
	}

	return 0;
}

int AES_GCM::encryptInit(const char *pw) {
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		printf("ERROR: Failed to create context\n");
		return 1;
	}

	if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
		printf("ERROR: Failed to set algorithm\n");
		return 1;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL) != 1) {
		printf("ERROR: Failed to set initial vector size\n");
		return 1;
	}

	if (Random(salt, SALT_SIZE)) {
		printf("ERROR: Failed to generate salt\n");
		return 1;
	}

	if (Argon2id(salt, pw, key)) {
		printf("ERROR: Failed to derive key\n");
		return 1;
	}

	if (Random(iv, IV_SIZE)) {
		printf("ERROR: Failed to generate initial vector\n");
		return 1;
	}

	if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
		printf("ERROR: Failed to set key and initial vector\n");
		return 1;
	}

	fileSize = GetFileSize(srcFile);

	if (fileSize == -1) {
		printf("ERROR: Failed to read file size\n");
		return 1;
	}

	if (fwrite(salt, sizeof(uint8_t), SALT_SIZE, dstFile) != SALT_SIZE) {
		printf("ERROR: Failed to write salt\n");
		return 1;
	}

	if (fwrite(iv, sizeof(uint8_t), IV_SIZE, dstFile) != IV_SIZE) {
		printf("ERROR: Failed to write initial vector\n");
		return 1;
	}

	return 0;
}

int AES_GCM::encryptAAD() {
	uint8_t aad[AAD_LEN];
	int len;

	memcpy(aad, salt, SALT_SIZE);
	memcpy(aad + SALT_SIZE, iv, IV_SIZE);

	if (EVP_EncryptUpdate(ctx, NULL, &len, aad, AAD_LEN) != 1) {
		printf("ERROR: Failed to provide AAD\n");
		return 1;
	}

	return 0;
}

int AES_GCM::encryptBlock(uint8_t *src, uint8_t *dst, int srcLen) {
	int dstLen;

	if (!EVP_EncryptUpdate(ctx, dst, &dstLen, src, srcLen)) {
		printf("ERROR: Failed to encrypt a block\n");
		return 1;
	}

	if (dstLen != srcLen) {
		printf("ERROR: Failed to encrypt a block\n");
		return 1;
	}

	return 0;
}

int AES_GCM::encryptBatch() {
	while (cur + BUFF_SIZE * BLOCK_SIZE <= fileSize) {
		if (readBuffer(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		for (int i = 0; i < BUFF_SIZE; i++) {
			if (encryptBlock(buff[i], buff[i], BLOCK_SIZE)) return 1;
		}

		if (writeBuffer(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		cur += BUFF_SIZE * BLOCK_SIZE;
	}

	return 0;
}

int AES_GCM::encryptRemain() {
	int crs = 0, rem = fileSize % BLOCK_SIZE;

	while (cur + BLOCK_SIZE <= fileSize) {
		if (readBuffer(buff[crs], BLOCK_SIZE)) return 1;

		if (encryptBlock(buff[crs], buff[crs], BLOCK_SIZE)) return 1;

		crs++;

		cur += BLOCK_SIZE;
	}

	if (rem) {
		if (readBuffer(buff[crs], rem)) return 1;

		if (encryptBlock(buff[crs], buff[crs], rem)) return 1;

		cur += rem;
	}

	if (writeBuffer(buff, BLOCK_SIZE * crs + rem)) return 1;

	return 0;
}

int AES_GCM::encryptFinal() {
	uint8_t final[BLOCK_SIZE];
	int finalLen;

	if (EVP_EncryptFinal_ex(ctx, final, &finalLen) != 1) {
		printf("ERROR: Failed to finalize encryption\n");
		return 1;
	}

	if (finalLen > 0 && writeBuffer(final, finalLen)) return 1;

	return 0;
}

int AES_GCM::encryptTag() {
	uint8_t tag[TAG_SIZE];

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, TAG_SIZE, tag) != 1) {
		printf("ERROR: Failed to get authentication tag\n");
		return 1;
	}

	if (fwrite(tag, sizeof(uint8_t), TAG_SIZE, dstFile) != TAG_SIZE) {
		printf("ERROR: Failed to write authentication tag\n");
		return 1;
	}

	return 0;
}

int AES_GCM::decryptInit(const char *pw) {
	if (!(ctx = EVP_CIPHER_CTX_new())) {
		printf("ERROR: Failed to create context\n");
		return 1;
	}

	if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
		printf("ERROR: Failed to set algorithm\n");
		return 1;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IV_SIZE, NULL) != 1) {
		printf("ERROR: Failed to set initial vector size\n");
		return 1;
	}

	if (EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
		printf("ERROR: Failed to set key and initial vector\n");
		return 1;
	}

	fileSize = GetFileSize(srcFile);

	if (fileSize == -1) {
		printf("ERROR: Failed to read file size\n");
		return 1;
	}

	fileSize -= TAG_SIZE;

	if (fread(salt, sizeof(uint8_t), SALT_SIZE, srcFile) != SALT_SIZE) {
		printf("ERROR: Failed to read salt\n");
		return 1;
	}

	if (Argon2id(salt, pw, key)) {
		printf("ERROR: Failed to derive key\n");
		return 1;
	}

	if (fread(iv, sizeof(uint8_t), IV_SIZE, srcFile) != IV_SIZE) {
		printf("ERROR: Failed to read initial vector\n");
		return 1;
	}

	return 0;
}

int AES_GCM::decryptAAD() {
	uint8_t aad[AAD_LEN];
	int len;

	memcpy(aad, salt, SALT_SIZE);
	memcpy(aad + SALT_SIZE, iv, IV_SIZE);

	if (EVP_DecryptUpdate(ctx, NULL, &len, aad, AAD_LEN) != 1) {
		printf("ERROR: Failed to provide AAD\n");
		return 1;
	}

	return 0;
}

int AES_GCM::decryptTag() {
	uint8_t tag[TAG_SIZE];

	if (_fseeki64(srcFile, fileSize, SEEK_SET)) {
		printf("ERROR: Failed to move file pointer\n");
		return 1;
	}

	if (fread(tag, sizeof(uint8_t), TAG_SIZE, srcFile) != TAG_SIZE) {
		printf("ERROR: Failed to read authentication tag\n");
		return 1;
	}

	if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, TAG_SIZE, tag) != 1) {
		printf("ERROR: Failed to set authentication tag\n");
		return 1;
	}

	if (_fseeki64(srcFile, SALT_SIZE + IV_SIZE, SEEK_SET)) {
		printf("ERROR: Failed to reset file pointer\n");
		return 1;
	}

	return 0;
}

int AES_GCM::decryptBlock(uint8_t *src, uint8_t *dst, int srcLen) {
	int dstLen;

	if (!EVP_DecryptUpdate(ctx, dst, &dstLen, src, srcLen)) {
		printf("ERROR: Failed to decrypt a block\n");
		return 1;
	}

	if (dstLen != srcLen) {
		printf("ERROR: Failed to decrypt a block\n");
		return 1;
	}

	return 0;
}

int AES_GCM::decryptBatch() {
	while (cur + BUFF_SIZE * BLOCK_SIZE <= fileSize) {
		if (readBuffer(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		for (int i = 0; i < BUFF_SIZE; i++) {
			if (decryptBlock(buff[i], buff[i], BLOCK_SIZE)) return 1;
		}

		if (writeBuffer(buff, BUFF_SIZE * BLOCK_SIZE)) return 1;

		cur += BUFF_SIZE * BLOCK_SIZE;
	}

	return 0;
}

int AES_GCM::decryptRemain() {
	int crs = 0, rem = fileSize % BLOCK_SIZE;

	while (cur + BLOCK_SIZE <= fileSize) {
		if (readBuffer(buff[crs], BLOCK_SIZE)) return 1;

		if (decryptBlock(buff[crs], buff[crs], BLOCK_SIZE)) return 1;

		crs++;

		cur += BLOCK_SIZE;
	}

	if (rem) {
		if (readBuffer(buff[crs], rem)) return 1;

		if (decryptBlock(buff[crs], buff[crs], rem)) return 1;

		cur += rem;
	}

	if (writeBuffer(buff, BLOCK_SIZE * crs + rem)) return 1;

	return 0;
}

int AES_GCM::decryptFinal() {
	uint8_t final[BLOCK_SIZE];
	int finalLen;

	if (EVP_DecryptFinal_ex(ctx, final, &finalLen) != 1) {
		printf("ERROR: Failed to finalize decryption\n");
		return 1;
	}

	if (finalLen > 0 && writeBuffer(final, finalLen)) return 1;

	return 0;
}