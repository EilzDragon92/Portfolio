#pragma once

#include "header.h"

#define BLOCK_SIZE 16
#define BUFF_SIZE 4096
#define IV_SIZE 12
#define TAG_SIZE 16

#define MAX_SIZE 16 * 0xffffffff

class AES_GCM {
public:
	using ErrorCallback = std::function<void(const char *errMsg)>;
	using ProgressCallback = std::function<void(int perc, bool *cancelled)>;

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
	EVP_CIPHER_CTX *ctx = nullptr;
	FILE *src, *dst;
	ErrorCallback ecb = nullptr;
	ProgressCallback pcb = nullptr;
	uint64_t cur = 0, size;
	uint8_t buff[BUFF_SIZE][BLOCK_SIZE], iv[IV_SIZE], key[KEY_SIZE], salt[SALT_SIZE];
	std::atomic<bool> cancelled{ false };

	int readBuffer(void *buff, int size);
	int writeBuffer(void *buff, int size);

	int decryptInit(const char *pw, int plen);
	int decryptTag();
	int decryptBlock(uint8_t *src, uint8_t *dst, int srcLen);
	int decryptBatch();
	int decryptRemain();
	int decryptFinal();

	int encryptInit(const char *pw, int plen);
	int encryptBlock(uint8_t *src, uint8_t *dst, int srcLen);
	int encryptBatch();
	int encryptRemain();
	int encryptFinal();
	int encryptTag();
	
	int reportProgress();
	void reportError(const char *msg);
};