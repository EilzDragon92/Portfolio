/**
 * @file	Vault.cpp
 * @brief	Implementation of basic functions Vault class
 * @author	EilzDragon92
 */

#include "Core/Vault.h"
#include "Utils/library.h"

Vault::Vault() {
	aes.setErrorCb([this](const char *msg) {
		lastError = msg;
	});
}

Vault::~Vault() {
	clear();
}

const std::set<Entry, EntryCmp> &Vault::getEntries() const {
	return entrySet;
}

const std::string &Vault::getLastError() const {
	return lastError;
}

int Vault::getEntryCount() const {
	return static_cast<int>(entrySet.size());
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
	lastError = msg;

	if (ecb) ecb(lastError.c_str());

	clear();
}