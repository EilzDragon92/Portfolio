/**
 * @file	Vault.cpp
 * @brief	Implementation of Vault class
 * @author	EilzDragon92
 */

#include "Core/Vault.h"

int Vault::create(const QString &path, const Password &pw) {
	FILE *file;
	uint8_t *srcBuff, *dstBuff;
	uint32_t magicNum = MAGIC_NUM;
	int64_t srcSize = 0;
	int64_t dstSize = MAGIC_SIZE + SALT_SIZE + IV_SIZE + TAG_SIZE;

	OpenFile(&file, path, "wb");

	if (file == nullptr) {
		// ERROR: Failed to open file
		return 1;
	}

	srcBuff = new uint8_t[srcSize]{};
	dstBuff = new uint8_t[dstSize]{};

	memcpy(dstBuff, &magicNum, MAGIC_SIZE);

	aes.encrypt(srcBuff, dstBuff + MAGIC_SIZE, srcSize, pw.getData(), pw.getSize());

	if (fwrite(dstBuff, sizeof(uint8_t), dstSize, file) != dstSize) {
		// ERROR: Failed to write file
		return 1;
	}

	Wipe(srcBuff, srcSize);
	Wipe(dstBuff, dstSize);

	delete[] srcBuff;
	delete[] dstBuff;

	return 0;
}

int Vault::open(const QString &path, const Password &pw) {
	FILE *file;
	uint8_t *srcBuff, *dstBuff;
	uint32_t magicNum = MAGIC_NUM;
	int64_t srcSize, dstSize;

	OpenFile(&file, path, "rb");

	if (file == nullptr) {
		// ERROR: Failed to open file
		return 1;
	}

	srcSize = GetFileSize(file);

	if (srcSize == -1) {
		// ERROR: Failed to read file size
		return 1;
	}

	dstSize = srcSize - (MAGIC_SIZE + SALT_SIZE + IV_SIZE + TAG_SIZE);

	srcBuff = new uint8_t[srcSize]{};
	dstBuff = new uint8_t[dstSize]{};

	if (fread(srcBuff, sizeof(uint8_t), srcSize, file) != srcSize) {
		// ERROR: Failed to read file
		return 1;
	}

	if (memcmp(srcBuff, &magicNum, MAGIC_SIZE) != 0) {
		// ERROR: Magic number mismatch
		return 1;
	}

	aes.decrypt(srcBuff + MAGIC_NUM, dstBuff, srcSize - MAGIC_NUM, pw.getData(), pw.getSize());

	Wipe(srcBuff, srcSize);
	Wipe(dstBuff, dstSize);

	delete[] srcBuff;
	delete[] dstBuff;

	return 0;
}

int Vault::save(const QString &path) {
	return 0;
}