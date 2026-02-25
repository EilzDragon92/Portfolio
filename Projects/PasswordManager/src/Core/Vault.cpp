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

int Vault::genPW(Password &dst, std::vector<bool> &spcList, int pwSize) {
	std::string pool;
	const char lower[] = "abcdefghijklmnopqrstuvwxyz";
	const char upper[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const char num[] = "0123456789";
	const char spc[] = "`~!@#$%^&*()-_=+[{]}\\|;:\'\",<.>/?";
	char *res;
	int poolSize;


	/* Check the list size is valid */

	if (spcList.size() != 32) {
		reportError("");
		return 1;
	}


	/* Check the password size is valid */

	if (pwSize < 8) {
		reportError("");
		return 1;
	}


	/* Add characters to pool */

	pool += lower;
	pool += upper;
	pool += num;

	for (int i = 0; i < 32; i++) if (spcList[i]) pool += spc[i];

	poolSize = pool.size();


	/* Generate password */

	res = new char[pwSize] {};

	res[0] = lower[RandomRange(0, 25)];
	res[1] = upper[RandomRange(0, 25)];
	res[2] = num[RandomRange(0, 9)];
	res[3] = pool[RandomRange(62, poolSize - 1)];

	for (int i = 4; i < pwSize; i++) res[i] = pool[RandomRange(0, poolSize - 1)];

	Shuffle(reinterpret_cast<uint8_t *>(res), pwSize);

	dst.setData(res, pwSize);

	
	/* Cleanup */

	Wipe(pool.data(), pool.size());
	Wipe(res, sizeof(res));

	delete[] res;


	return 0;
}

void Vault::setPW(Password &pw) {
	this->pw = pw;
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