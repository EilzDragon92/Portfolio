#include "header.h"

#define AAD_LEN (IV_SIZE + SALT_SIZE)
#define BLOC_SIZE 16
#define BUFF_SIZE 4096
#define IV_SIZE 12
#define TAG_SIZE 16

class AES_GCM {
public:
	~AES_GCM() {
		if (ctx) {
			EVP_CIPHER_CTX_free(ctx);
			ctx = NULL;
		}
	}

	int setup(FILE *src, FILE *dst) {
		srcFile = src, dstFile = dst;

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

		if (EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv) != 1) {
			printf("ERROR: Failed to set key and initial vector\n");
			return 1;
		}

		fileSize = GetFileSize(srcFile);

		if (fileSize == -1) {
			printf("ERROR: Failed to read file size\n");
			return 1;
		}

		return 0;
	}

	int encryptSetup(const char *pw) {
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

	int encryptAAD() {
		uint8_t aad[AAD_LEN];

		memcpy(aad, salt, SALT_SIZE);
		memcpy(aad + SALT_SIZE, iv, IV_SIZE);

		if (EVP_EncryptUpdate(ctx, NULL, NULL, aad, AAD_LEN) != 1) {
			printf("ERROR: Failed to put AAD\n");
			return 1;
		}

		return 0;
	}

	int encryptBlock(uint8_t *src, uint8_t *dst, int srcLen) {
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

	int encryptBatch(uint8_t buff[][BLOC_SIZE]) {
		while (cur + BUFF_SIZE * BLOC_SIZE <= fileSize) {
			if (readBuffer(buff, BUFF_SIZE * BLOC_SIZE)) return 1;

			for (int i = 0; i < BUFF_SIZE; i++) {
				if (encryptBlock(buff[i], buff[i], BLOC_SIZE)) return 1;
			}

			if (writeBuffer(buff, BUFF_SIZE * BLOC_SIZE)) return 1;

			cur += BUFF_SIZE * BLOC_SIZE;
		}

		return 0;
	}

	int encryptRemain(uint8_t buff[][BLOC_SIZE]) {
		int crs = 0, rem = fileSize % BLOC_SIZE;

		while (cur + BLOC_SIZE <= fileSize) {
			if (readBuffer(buff[crs], BLOC_SIZE)) return 1;

			if (encryptBlock(buff[crs], buff[crs], BLOC_SIZE)) return 1;

			crs++;

			cur += BLOC_SIZE;
		}

		if (rem) {
			if (readBuffer(buff[crs], rem)) return 1;

			if (encryptBlock(buff[crs], buff[crs], rem)) return 1;

			cur += rem;
		}

		if (writeBuffer(buff, BLOC_SIZE * crs + rem)) return 1;

		return 0;
	}

	int encryptFinal() {
		uint8_t final[BLOC_SIZE];
		int finalLen;

		if (EVP_EncryptFinal_ex(ctx, final, &finalLen) != 1) {
			printf("ERROR: Failed to finalize encryption\n");
			return 1;
		}

		if (finalLen > 0 && writeBuffer(final, finalLen)) return 1;

		return 0;
	}

	int encryptTag() {
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

	int encryptFile(FILE *src, FILE *dst, const char *pw) {
		uint8_t buff[BUFF_SIZE][BLOC_SIZE];

		if (setup(src, dst)) return 1;

		if (encryptSetup(pw)) return 1;

		if (encryptAAD()) return 1;

		if (encryptBatch(buff)) return 1;

		if (encryptRemain(buff)) return 1;

		if (encryptFinal()) return 1;

		if (encryptTag()) return 1;

		return 0;
	}

	int decryptFile(FILE *src, FILE *dst, const char *pw) {
		// read salt and ivec

		if (setup(src, dst)) return 1;

		return 0;
	}

private:
	EVP_CIPHER_CTX *ctx;
	FILE *srcFile, *dstFile;
	uint64_t cur = 0, fileSize;
	uint8_t iv[IV_SIZE], key[KEY_SIZE], salt[SALT_SIZE];

	int readBuffer(void *buff, int size) {
		if (fread(buff, sizeof(uint8_t), size, srcFile) != size) {
			printf("ERROR: Failed to read file\n");
			return 1;
		}

		return 0;
	}

	int writeBuffer(void *buff, int size) {
		if (fwrite(buff, sizeof(uint8_t), size, dstFile) != size) {
			printf("ERROR: Failed to write file\n");
			return 1;
		}
		
		return 0;
	}
};