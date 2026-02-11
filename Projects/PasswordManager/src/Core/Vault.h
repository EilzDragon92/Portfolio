/**
 * @file	Vault.h
 * @brief	Manages password vaults
 * @author	EilzDragon92
 */

#include "Common/header.h"
#include "Core/AES_GCM.h"
#include "Utils/Password.h"

struct Entry {
	size_t size;
	std::string site;
	std::string acc;
	Password pw;
};

struct entryCmp {
	bool operator()(const Entry &a, const Entry &b) const {
		if (a.site != b.site) return a.site.compare(b.site);
		if (a.acc != b.acc) return a.acc.compare(b.acc);

		return false;
	}
};

class Vault {
public:
	/**
	 * @brief	Default constructor of Vault class
	 */
	Vault();

	/**
	 * @brief	Default destructor of Vault class
	 */
	~Vault();

	/**
	 * @brief	Create new vault
	 * @param   path    Vault file path
	 * @param   pw      Master password
	 * @return  0 on success, 1 on failure
	 */
	int create(const QString &path, const Password &pw);

	/**
	 * @brief	Open vault
	 * @param   path    Vault file path
	 * @param   pw      Master password
	 * @return  0 on success, 1 on failure
	 */
	int open(const QString &path, const Password &pw);

	/**
	 * @brief	Save vault
	 * @param   path    Vault file path
	 * @return  0 on success, 1 on failure
	 */
	int save(const QString &path);

private:
	AES_GCM aes;

	std::set<Entry, entryCmp> entrySet;
};