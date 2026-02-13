/**
 * @file	Vault.h
 * @brief	Manages password vaults
 * @author	EilzDragon92
 */

#pragma once

#include "Common/header.h"
#include "Core/AES_GCM.h"
#include "Core/Entry.h"

 /**
  * @class	Vault
  * @brief	Manages password vaults
  */
class Vault {
public:
	/**
	 * @brief	Callback function for error reporting
	 * @param	errMsg	Error message string
	 */
	using ErrorCallback = std::function<void(const char *errMsg)>;

	/**
	 * @brief	Default constructor of Vault class
	 */
	Vault();

	/**
	 * @brief	Default destructor of Vault class
	 */
	~Vault();

	/**
	 * @brief	Create empty new vault
	 * @param   path    Vault file path
	 * @return  0 on success, 1 on failure
	 */
	int create(const QString &path);

	/**
	 * @brief	Open vault and read its data
	 * @param   path    Vault file path
	 * @return  0 on success, 1 on failure
	 */
	int open(const QString &path);

	/**
	 * @brief	Save vault with current data
	 * @param   path    Vault file path
	 * @return  0 on success, 1 on failure
	 */
	int save(const QString &path);

private:
	AES_GCM aes;
	Password pw;
	std::set<Entry, EntryCmp> entrySet;

	ErrorCallback ecb = nullptr;

	FILE *file = nullptr;
	uint8_t *srcBuff = nullptr, *dstBuff = nullptr;
	int64_t srcSize = 0, dstSize = 0;
	uint32_t magicNum = MAGIC_NUM;

	void clear();


	/* ==================================================
	 * Callback helper functions
	 * ================================================== */

	/**
	  * @brief	Report error via callback
	  * @param	msg		Error message string
	  */
	void reportError(const char *msg);
};