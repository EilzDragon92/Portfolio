/**
 * @file	Vault.cpp
 * @brief	Implementation of vault file management functions of Vault class
 * @author	EilzDragon92
 */

#include "Core/Vault.h"

int Vault::newVault(const QString &path) {
	uint32_t entryCnt = 0;

	lastError.clear();


	/* Generate initial data */

	srcSize = COUNT_SIZE;
	dstSize = MAGIC_SIZE + SALT_SIZE + IV_SIZE + srcSize + TAG_SIZE;

	srcBuff = new uint8_t[srcSize]{};
	dstBuff = new uint8_t[dstSize]{};

	memcpy(srcBuff, &entryCnt, COUNT_SIZE);
	memcpy(dstBuff, &magicNum, MAGIC_SIZE);


	/* Encrypt */

	if (aes.encrypt(srcBuff, dstBuff + MAGIC_SIZE, srcSize, pw.getData(), pw.getSize())) {
		reportError("[Crypto] Encryption failed - Cannot encrypt vault data\n");
		return 1;
	}


	/* Write vault */

	OpenFile(&file, path, "wb");

	if (file == nullptr) {
		reportError("[File] Open failed - Cannot create vault file\n");
		return 1;
	}

	if (fwrite(dstBuff, sizeof(uint8_t), dstSize, file) != dstSize) {
		reportError("[File] Write failed - Cannot write vault file\n");
		return 1;
	}


	clear();

	return 0;
}

int Vault::openVault(const QString &path) {
	uint32_t entryCnt = 0;
	int cur = 0;

	lastError.clear();


	/* Open file pointer */

	OpenFile(&file, path, "rb");

	if (file == nullptr) {
		reportError("[File] Open failed - Cannot open vault file\n");
		return 1;
	}


	/* Get vault size */

	srcSize = GetFileSize(file);

	if (srcSize == -1) {
		reportError("[File] Size check failed - Cannot read vault file size\n");
		return 1;
	}


	/* Read vault */

	srcBuff = new uint8_t[srcSize]{};

	if (fread(srcBuff, sizeof(uint8_t), srcSize, file) != srcSize) {
		reportError("[File] Read failed - Cannot read vault file data\n");
		return 1;
	}


	/* Check magic number */

	if (memcmp(srcBuff, &magicNum, MAGIC_SIZE) != 0) {
		reportError("[File] Validation failed - Invalid vault file format\n");
		return 1;
	}


	/* Decrypt */

	dstSize = srcSize - (MAGIC_SIZE + SALT_SIZE + IV_SIZE + TAG_SIZE);

	dstBuff = new uint8_t[dstSize]{};

	if (aes.decrypt(srcBuff + MAGIC_SIZE, dstBuff, srcSize - MAGIC_SIZE, pw.getData(), pw.getSize())) {
		reportError("[Auth] Decryption failed - Invalid password or corrupted vault\n");
		return 1;
	}


	/* Deserialize */

	memcpy(&entryCnt, dstBuff, COUNT_SIZE);
	cur += COUNT_SIZE;

	for (int i = 0; i < entryCnt; i++) {
		Entry entry;

		cur += entry.deser(dstBuff + cur);

		entrySet.insert(std::move(entry));
	}


	clear();

	return 0;
}

int Vault::saveVault(const QString &path) {
	uint32_t entryCnt = static_cast<uint32_t>(entrySet.size());
	int srcCur = 0, dstCur = 0;

	lastError.clear();


	/* Calculate vault size */

	srcSize = sizeof(uint32_t);

	for (auto it = entrySet.begin(); it != entrySet.end(); it++) srcSize += it->size();

	dstSize = MAGIC_SIZE + SALT_SIZE + IV_SIZE + srcSize + TAG_SIZE;

	srcBuff = new uint8_t[srcSize]{};
	dstBuff = new uint8_t[dstSize]{};


	/* Write entry count to buffer */

	memcpy(srcBuff + srcCur, &entryCnt, sizeof(uint32_t));
	srcCur += sizeof(uint32_t);


	/* Write entries to buffer */

	for (auto it = entrySet.begin(); it != entrySet.end(); it++) srcCur += it->ser(srcBuff + srcCur);

	memcpy(dstBuff + dstCur, &magicNum, MAGIC_SIZE);
	dstCur += MAGIC_SIZE;


	/* Encrypt */

	if (aes.encrypt(srcBuff, dstBuff + dstCur, srcSize, pw.getData(), pw.getSize())) {
		reportError("[Crypto] Encryption failed - Cannot encrypt vault data\n");
		return 1;
	}

	dstCur += SALT_SIZE + IV_SIZE + srcSize + TAG_SIZE;


	/* Save vault */

	OpenFile(&file, path, "wb");

	if (file == nullptr) {
		reportError("[File] Open failed - Cannot open vault file for writing\n");
		return 1;
	}

	if (fwrite(dstBuff, sizeof(uint8_t), dstSize, file) != dstSize) {
		reportError("[File] Write failed - Cannot write vault file\n");
		return 1;
	}


	clear();

	return 0;
}

void Vault::closeVault() {
	entrySet.clear();
	pw.clean();
	clear();
}

int Vault::changePW(const Password &curPW, const Password &newPW, const QString &path) {
	lastError.clear();


	/* Verify current password with constant-time comparison */

	if (!pw.compare(curPW)) {
		reportError("[Auth] Verification failed - Current password is incorrect\n");
		return 1;
	}


	/* Set new password */

	pw.setData(newPW);


	/* Re-encrypt and save vault */

	if (saveVault(path)) return 2;


	return 0;
}

void Vault::setPW(Password &pw) {
	this->pw = pw;
}