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
	/* ==================================================
	 * Constructor and Destructor
	 * ================================================== */

	/**
	 * @brief	Default constructor of Vault class
	 */
	Vault();

	/**
	 * @brief	Default destructor of Vault class
	 */
	~Vault();


	/* ==================================================
	 * Vault file management functions
	 * ================================================== */

	/**
	 * @brief	Create empty new vault
	 * @param   path    Vault file path
	 * @return  0 on success, 1 on failure
	 */
	int newVault(const QString &path);

	/**
	 * @brief	Open vault and read its data
	 * @param   path    Vault file path
	 * @return  0 on success, 1 on failure
	 */
	int openVault(const QString &path);

	/**
	 * @brief	Save vault with current data
	 * @param   path    Vault file path
	 * @return	0 on success, 1 on failure
	 */
	int saveVault(const QString &path);


	/* ==================================================
	 * Entry CRUD functions
	 * ================================================== */

	/**
	 * @brief	Create new entry
	 * @param	site	Site name of the new entry
	 * @param	acc		Account of the new entry
	 * @param	pw		Password of the new entry
	 * @return	0 on success, 1 on failure
	 */
	int createEntry(const std::string &site, const std::string &acc, const Password &pw);

	/**
	 * @brief	Update an entry
	 * @param	oldSite		Site name of the target entry
	 * @param	oldAcc		Account of the target entry
	 * @param	newSite		New site name
	 * @param	newAcc		New account
	 * @param	newPw		New password
	 * @return	0 on success, 1 on failure
	 */
	int updateEntry(const std::string &oldSite, const std::string &oldAcc, 
					const std::string &newSite, const std::string &newAcc, const Password &newPW);

	/**
	 * @brief	Delete an entry
	 * @param	site	Site name of the target entry
	 * @param	acc		Account of the target entry
	 * @return	0 on success, 1 on failure
	 */
	int deleteEntry(const std::string &site, const std::string &acc);


	/* ==================================================
	 * Helper functions
	 * ================================================== */

	/**
	 * @brief	Generate a random password that includes at least one each of uppercase, lowercase, number, and special
	 * @param	dst		Destination password
	 * @param	vec		List of specials to be used
	 * @param	size	Destination password size
	 * @return	0 on success, 1 on failure
	 */
	int genPW(Password &dst, std::vector<bool> &spcList, int size);


	/* ==================================================
	 * Callback functions
	 * ================================================== */

	/**
	 * @brief	Callback function for error reporting
	 * @param	errMsg	Error message string
	 */
	using ErrorCallback = std::function<void(const char *errMsg)>;

private:
	AES_GCM aes;
	Password pw;
	std::set<Entry, EntryCmp> entrySet;

	ErrorCallback ecb = nullptr;

	FILE *file = nullptr;
	uint8_t *srcBuff = nullptr, *dstBuff = nullptr;
	int64_t srcSize = 0, dstSize = 0;
	uint32_t magicNum = MAGIC_NUM;


	/* ==================================================
	 * Helper functions
	 * ================================================== */

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