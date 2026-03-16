/**
 * @file	Vault_file.cpp
 * @brief	Implementation of file management functions of Vault class
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
		// LCOV_EXCL_START
		reportError("[File] Open failed - Cannot create vault file\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	if (fwrite(dstBuff, sizeof(uint8_t), dstSize, file) != dstSize) {
		// LCOV_EXCL_START
		reportError("[File] Write failed - Cannot write vault file\n");
		return 1;
		// LCOV_EXCL_STOP
	}


	clear();

	return 0;
}

int Vault::openVault(const QString &path) {
	size_t cur = 0;
	uint32_t entryCnt = 0;

	lastError.clear();


	/* Open file pointer */

	OpenFile(&file, path, "rb");

	if (file == nullptr) {
		// LCOV_EXCL_START
		reportError("[File] Open failed - Cannot open vault file\n");
		return 1;
		// LCOV_EXCL_STOP
	}


	/* Get vault size */

	srcSize = GetFileSize(file);

	if (srcSize == -1) {
		// LCOV_EXCL_START
		reportError("[File] Size check failed - Cannot read vault file size\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	if (srcSize < MIN_SIZE) {
		reportError("[File] Validation failed - File is too small to be a valid vault\n");
		return 1;
	}


	if (srcSize > MAX_SIZE) {
		reportError("[File] Validation failed - File exceeds maximum size (2 GiB)\n");
		return 1;
	}


	/* Read vault */

	srcBuff = new uint8_t[srcSize]{};

	if (fread(srcBuff, sizeof(uint8_t), srcSize, file) != srcSize) {
		// LCOV_EXCL_START
		reportError("[File] Read failed - Cannot read vault file data\n");
		return 1;
		// LCOV_EXCL_STOP
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

	for (uint32_t i = 0; i < entryCnt; i++) {
		Entry entry;

		size_t bytes = entry.deser(dstBuff + cur, dstSize - cur);

		if (bytes == 0) {
			reportError("[Data] Deserialization failed - Invalid entry data\n");
			return 1;
		}

		cur += bytes;

		entrySet.insert(std::move(entry));
	}


	clear();

	return 0;
}

int Vault::saveVault(const QString &path) {
	size_t srcCur = 0, dstCur = 0;
	uint32_t entryCnt = static_cast<uint32_t>(entrySet.size());

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


	/* Save vault */

	OpenFile(&file, path, "wb");

	if (file == nullptr) {
		// LCOV_EXCL_START
		reportError("[File] Open failed - Cannot open vault file for writing\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	if (fwrite(dstBuff, sizeof(uint8_t), dstSize, file) != dstSize) {
		// LCOV_EXCL_START
		reportError("[File] Write failed - Cannot write vault file\n");
		return 1;
		// LCOV_EXCL_STOP
	}


	clear();

	return 0;
}

void Vault::closeVault() {
	entrySet.clear();
	pw.clean();
	clear();
}

bool Vault::verifyPW(const Password &curPW) const {
	return pw.equal(curPW);
}

int Vault::changePW(const Password &newPW, const QString &path) {
	lastError.clear();

	if (pw.setData(newPW)) {
		reportError("[Auth] Password change failed - Password exceeds maximum length (256 characters)\n");
		return 1;
	}

	if (saveVault(path)) return 1;

	return 0;
}

void Vault::setPW(const Password &pw) {
	this->pw = pw;
}