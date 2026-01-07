/**
 * @file	AES_GCM.cpp
 * @brief	Implementation of basic functions of AES_GCM class
 * @author	EilzDragon92
 *
 * Contents:
 *	- Implementation of destructor and helper functions of AES_GCM class
 */

#include "Core/AES_GCM.h"

AES_GCM::~AES_GCM() {
	Wipe(buff, sizeof(uint8_t) * BUFF_SIZE * BLOCK_SIZE);
	Wipe(iv, sizeof(uint8_t) * IV_SIZE);
	Wipe(key, sizeof(uint8_t) * KEY_SIZE);
	Wipe(salt, sizeof(uint8_t) * SALT_SIZE);

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
		ctx = nullptr;
	}
}

int AES_GCM::readBuffer(void *buff, int size) {
	if (fread(buff, sizeof(uint8_t), size, src) != size) {
		reportError("[File] Read failed - Cannot read source file data\n");
		return 1;
	}

	return 0;
}

int AES_GCM::writeBuffer(void *buff, int size) {
	if (fwrite(buff, sizeof(uint8_t), size, dst) != size) {
		reportError("[File] Write failed - Cannot write destination file data\n");
		return 1;
	}

	return 0;
}

int AES_GCM::reportProgress() {
	if (pcb) {
		bool tmp = cancelled.load();
		uint64_t perc = size == 0 ? 100 : cur * 100 / size;

		pcb(perc, &tmp);
		cancelled.store(tmp);

		if (cancelled) return 1;
	}

	return 0;
}

void AES_GCM::reportError(const char *msg) {
	if (!ecb) return;

	std::string res = msg;
	unsigned long code;
	char buff[256];

	while (code = ERR_get_error()) {
		ERR_error_string_n(code, buff, sizeof(buff));

		res += " -> ";
		res += buff;
		res += '\n';
	}

	ecb(res.c_str());
}