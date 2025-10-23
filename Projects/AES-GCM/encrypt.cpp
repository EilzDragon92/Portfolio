#include "header.h"

#define BUFF_SIZE 4096
#define BLOC_SIZE 16
#define IVEC_SIZE 12

class AES_GCM {
public:
	int setup(FILE *src) {
		if (!(ctx = EVP_CIPHER_CTX_new())) {
			printf("ERROR: Failed to create context\n");
			return 1;
		}

		if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
			printf("ERROR: Failed to set algorithm\n");
			return 1;
		}

		if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IVEC_SIZE, NULL) != 1) {
			printf("ERROR: Failed to set initial vector size\n");
			return 1;
		}

		if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, ivec) != 1) {
			printf("ERROR: Failed to set key and initial vector\n");
			return 1;
		}

		fsize = GetFileSize(src);

		if (fsize == -1) {
			printf("ERROR: Failed to read file size\n");
			return 1;
		}

		return 0;
	}

	void encrypt(FILE *src, FILE *dst, const char *pw) {
		if (Random(salt, SALT_SIZE)) {
			printf("ERROR: Failed to generate salt\n");
			return;
		}

		if (Argon2id(salt, pw, key)) {
			printf("ERROR: Failed to derive key\n");
			return;
		}

		if (Random(ivec, IVEC_SIZE)) {
			printf("ERROR: Failed to generate initial vector\n");
			return;
		}

		if (setup(src)) return;

		while (cur + BUFF_SIZE * BLOC_SIZE < fsize) {
			if (!readBuffer(src)) {
				printf("ERROR: Failed to read file\n");
				return;
			}

			// encrypt buffer

			if (!writeBuffer(dst)) {
				printf("ERROR: Failed to write file\n");
				return;
			}

			cur += BUFF_SIZE * BLOC_SIZE;
		}
	}

	void decrypt(FILE *src, FILE *dst, const char *pw) {
		// read salt and ivec

		if (setup(src)) return;
	}

private:
	EVP_CIPHER_CTX *ctx;
	uint8_t buff[BUFF_SIZE][BLOC_SIZE];
	uint8_t ivec[IVEC_SIZE], key[KEY_SIZE], salt[SALT_SIZE];
	int cur = 0, fsize;

	bool readBuffer(FILE *file) {
		return fread(buff, sizeof(uint8_t), BUFF_SIZE * BLOC_SIZE, file) == BUFF_SIZE * BLOC_SIZE;
	}

	bool writeBuffer(FILE *file) {
		return fwrite(buff, sizeof(uint8_t), BUFF_SIZE * BLOC_SIZE, file) == BUFF_SIZE * BLOC_SIZE;
	}
};