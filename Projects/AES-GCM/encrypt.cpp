#include "header.h"

#define BUFF_SIZE 4096
#define BLOC_SIZE 16
#define IVEC_SIZE 12

class AES_GCM {
public:
	void encrypt(const char *pw) {
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

		if (!(ctx = EVP_CIPHER_CTX_new())) {
			printf("ERROR: Failed to create context\n");
			return;
		}

		if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
			printf("ERROR: Failed to set algorithm\n");
			return;
		}

		if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, IVEC_SIZE, NULL) != 1) {
			printf("ERROR: Failed to set initial vector size\n");
			return;
		}

		if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, ivec) != 1) {
			printf("ERROR: Failed to set key and initial vector\n");
			return;
		}
	}

private:
	EVP_CIPHER_CTX *ctx;
	uint8_t ivec[IVEC_SIZE], key[KEY_SIZE], salt[SALT_SIZE];
};