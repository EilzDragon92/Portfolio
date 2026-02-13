/**
 * @file	Vault.cpp
 * @brief	Implementation of Vault class
 * @author	EilzDragon92
 */

#include "Core/Vault.h"

Vault::Vault() {
	;
}

Vault::~Vault() {
	clear();
}

int Vault::create(const QString &path) {
	int entryCnt = 0;


	/* Generate initial data */

	srcSize = COUNT_SIZE;
	dstSize = MAGIC_SIZE + SALT_SIZE + IV_SIZE + TAG_SIZE;

	srcBuff = new uint8_t[srcSize]{};
	dstBuff = new uint8_t[dstSize]{};

	memcpy(srcBuff, &entryCnt, COUNT_SIZE);
	memcpy(dstBuff, &magicNum, MAGIC_SIZE);


	/* Encrypt */

	if (aes.encrypt(srcBuff, dstBuff + MAGIC_SIZE, srcSize, pw.getData(), pw.getSize())) {
		reportError("Encryption failed");
		return 1;
	}


	/* Write vault */

	OpenFile(&file, path, "wb");

	if (file == nullptr) {
		reportError("Failed to open file");
		return 1;
	}

	if (fwrite(dstBuff, sizeof(uint8_t), dstSize, file) != dstSize) {
		reportError("ERROR: Failed to write file");
		return 1;
	}


	clear();

	return 0;
}

int Vault::open(const QString &path) {
	int cur = 0, entryCnt = 0;


	/* Open file pointer */

	OpenFile(&file, path, "rb");

	if (file == nullptr) {
		reportError("Failed to open file");
		return 1;
	}


	/* Get vault size */

	srcSize = GetFileSize(file);

	if (srcSize == -1) {
		reportError("Failed to read file size");
		return 1;
	}


	/* Read vault */

	srcBuff = new uint8_t[srcSize]{};

	if (fread(srcBuff, sizeof(uint8_t), srcSize, file) != srcSize) {
		reportError("Failed to read file");
		return 1;
	}


	/* Check magic number */

	if (memcmp(srcBuff, &magicNum, MAGIC_SIZE) != 0) {
		reportError("Magic number mismatch");
		return 1;
	}


	/* Decrypt */

	dstSize = srcSize - (MAGIC_SIZE + SALT_SIZE + IV_SIZE + TAG_SIZE);

	dstBuff = new uint8_t[dstSize]{};

	if (aes.decrypt(srcBuff + MAGIC_SIZE, dstBuff, srcSize - MAGIC_SIZE, pw.getData(), pw.getSize())) {
		reportError("Decryption failed");
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

int Vault::save(const QString &path) {
	int srcCur = 0, dstCur = 0;
	int entryCnt = static_cast<int>(entrySet.size());


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
		reportError("Encryption failed");
		return 1;
	}

	dstCur += SALT_SIZE + IV_SIZE + srcSize + TAG_SIZE;


	/* Save vault */

	OpenFile(&file, path, "wb");

	if (fwrite(dstBuff, sizeof(uint8_t), dstSize, file) != dstSize) {
		reportError("ERROR: Failed to write file");
		return 1;
	}


	clear();

	return 0;
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