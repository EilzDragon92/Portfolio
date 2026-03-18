/**
 * @file	Vault_file.cpp
 * @brief	Implementation of file management functions of Vault class
 * @author	EilzDragon92
 */

#include "Core/Vault.h"
#include "Utils/library.h"

int Vault::newVault(const QString &path) {
	uint32_t entryCnt = 0;

	lastError.clear();


	/* Generate initial data */

	srcSize = kCountSize;
	dstSize = kMagicSize + kSaltSize + kIVSize + srcSize + kTagSize;

	srcBuff = new uint8_t[srcSize]{};
	dstBuff = new uint8_t[dstSize]{};

	memcpy(srcBuff, &entryCnt, kCountSize);
	memcpy(dstBuff, &magicNum, kMagicSize);


	/* Encrypt */

	if (aes.encrypt(srcBuff, dstBuff + kMagicSize, srcSize, pw.getData(), pw.getSize())) {
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


	/* Sync file data to disk */

	if (SyncFile(file)) {
		// LCOV_EXCL_START
		reportError("[File] Sync failed - Cannot flush vault file to disk\n");
		return 1;
		// LCOV_EXCL_STOP 
	}


	clear();

	return 0;
}

int Vault::openVault(const QString &path) {
	std::set<Entry, EntryCmp> tmp;
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

	if (srcSize < kMinSize) {
		reportError("[File] Validation failed - File is too small to be a valid vault\n");
		return 1;
	}


	if (srcSize > kMaxSize) {
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

	if (memcmp(srcBuff, &magicNum, kMagicSize) != 0) {
		reportError("[File] Validation failed - Invalid vault file format\n");
		return 1;
	}


	/* Decrypt */

	dstSize = srcSize - (kMagicSize + kSaltSize + kIVSize + kTagSize);

	dstBuff = new uint8_t[dstSize]{};

	if (aes.decrypt(srcBuff + kMagicSize, dstBuff, srcSize - kMagicSize, pw.getData(), pw.getSize())) {
		reportError("[Auth] Decryption failed - Invalid password or corrupted vault\n");
		return 1;
	}


	/* Deserialize */

	memcpy(&entryCnt, dstBuff, kCountSize);
	cur += kCountSize;

	if (entryCnt * kMinEntrySize > dstSize - kCountSize) {
		reportError("[Data] Validation failed - Entry count exceeds available data\n");
		return 1;
	}


	for (uint32_t i = 0; i < entryCnt; i++) {
		Entry entry;

		size_t bytes = entry.deser(dstBuff + cur, dstSize - cur);

		if (bytes == 0) {
			reportError("[Data] Deserialization failed - Invalid entry data\n");
			return 1;
		}

		cur += bytes;

		tmp.insert(std::move(entry));
	}

	entrySet = std::move(tmp);


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

	dstSize = kMagicSize + kSaltSize + kIVSize + srcSize + kTagSize;

	srcBuff = new uint8_t[srcSize]{};
	dstBuff = new uint8_t[dstSize]{};


	/* Write entry count to buffer */

	memcpy(srcBuff + srcCur, &entryCnt, sizeof(uint32_t));
	srcCur += sizeof(uint32_t);


	/* Write entries to buffer */

	for (auto it = entrySet.begin(); it != entrySet.end(); it++) srcCur += it->ser(srcBuff + srcCur);

	memcpy(dstBuff + dstCur, &magicNum, kMagicSize);
	dstCur += kMagicSize;


	/* Encrypt */

	if (aes.encrypt(srcBuff, dstBuff + dstCur, srcSize, pw.getData(), pw.getSize())) {
		reportError("[Crypto] Encryption failed - Cannot encrypt vault data\n");
		return 1;
	}


	/* Save to temporary file */

	QString tmpPath = path + ".tmp";

	OpenFile(&file, tmpPath, "wb");

	if (file == nullptr) {
		// LCOV_EXCL_START
		reportError("[File] Open failed - Cannot open temporary file for writing\n");
		return 1;
		// LCOV_EXCL_STOP
	}

	if (fwrite(dstBuff, sizeof(uint8_t), dstSize, file) != dstSize) {
		// LCOV_EXCL_START
		reportError("[File] Write failed - Cannot write temporary file\n");
		RemoveFile(tmpPath);
		return 1;
		// LCOV_EXCL_STOP
	}


	/* Sync file data to disk */

	if (SyncFile(file)) {
		// LCOV_EXCL_START
		reportError("[File] Sync failed - Cannot flush vault file to disk\n");
		RemoveFile(tmpPath);
		return 1;
		// LCOV_EXCL_STOP 
	}


	/* Atomic replace: rename temporary file to vault file */

	if (RenameFile(tmpPath, path)) {
		// LCOV_EXCL_START
		reportError("[File] Rename failed - Cannot replace vault file\n");
		RemoveFile(tmpPath);
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