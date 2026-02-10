/**
 * @file	AES_GCM.cpp
 * @brief	Implementation of basic functions AES_GCM class
 * @author	EilzDragon92
 */

#include "Core/AES_GCM.h"

AES_GCM::AES_GCM() {
	memset(iv, 0, sizeof(uint8_t) * IV_SIZE);
	memset(salt, 0, sizeof(uint8_t) * SALT_SIZE);

	Lock(key, KEY_SIZE);
}

AES_GCM::~AES_GCM() {
	Wipe(iv, sizeof(uint8_t) * IV_SIZE);
	Wipe(key, sizeof(uint8_t) * KEY_SIZE);
	Wipe(salt, sizeof(uint8_t) * SALT_SIZE);

	Unlock(key, KEY_SIZE);

	if (ctx) {
		EVP_CIPHER_CTX_free(ctx);
		ctx = nullptr;
	}
}

void AES_GCM::reportError(const char *msg) {
	if (!ecb) return;

	std::string res;
	unsigned long code;
	char errStr[256];

	res += msg;

	while ((code = ERR_get_error()) != 0) {
		ERR_error_string_n(code, errStr, sizeof(errStr));

		res += " -> ";
		res += errStr;
		res += '\n';
	}

	ecb(res.c_str());
}