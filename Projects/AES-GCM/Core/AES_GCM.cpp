#include "AES_GCM.h"

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

int AES_GCM::reportProgress() {
	if (pcb) {
		bool tmp = cancelled.load();

		pcb(cur * 100 / size, &tmp);
		cancelled.store(tmp);

		if (cancelled) return 1;
	}
	return 0;
}

void AES_GCM::reportError(const char *msg) {
	if (ecb) ecb(msg);
}