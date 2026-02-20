/**
 * @file	Vault.cpp
 * @brief	Implementation of entry management functions of Vault class
 * @author	EilzDragon92
 */

#include "Core/Vault.h"

int Vault::createEntry(const std::string &site, const std::string &acc, const Password &pw) {
	Entry newEntry = { site, acc, pw };

	auto res = entrySet.insert(newEntry);

	return res.second ? 0 : 1;
}

int Vault::updateEntry(const std::string &oldSite, const std::string &oldAcc, 
					   const std::string &newSite, const std::string &newAcc, const Password &newPW) {
	/* Check whether the target entry exists */

	Entry oldEntry = { oldSite, oldAcc };

	auto it = entrySet.find(oldEntry);

	if (it == entrySet.end()) return 1;


	/* Check new entry data conflicts with existing entry */

	Entry newEntry = { newSite, newAcc };

	it = entrySet.find(newEntry);

	if (it != entrySet.end()) return 2;
}

int Vault::deleteEntry(const std::string &site, const std::string &acc) {
	Entry tar = { site, acc };

	auto it = entrySet.find(tar);

	if (it == entrySet.end()) return 1;

	entrySet.erase(it);

	return 0;
}