/**
 * @file	Vault.cpp
 * @brief	Implementation of basic functions Vault class
 * @author	EilzDragon92
 */

#include "Core/Vault.h"

Vault::Vault() {
	;
}

Vault::~Vault() {
	clear();
}

void Vault::clear() {
	if (file) {
		fclose(file);
		file = nullptr;
	}

	if (srcBuff) {
		Wipe(srcBuff, srcSize);
		delete[] srcBuff;
		srcBuff = nullptr;
	}

	if (dstBuff) {
		Wipe(dstBuff, dstSize);
		delete[] dstBuff;
		dstBuff = nullptr;
	}

	srcSize = 0;
	dstSize = 0;
}

void Vault::reportError(const char *msg) {
	if (ecb) ecb(msg);

	clear();
}