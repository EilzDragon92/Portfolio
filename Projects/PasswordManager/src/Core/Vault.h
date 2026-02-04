/**
 * @file	Vault.h
 * @brief	Manages password vaults
 * @author	EilzDragon92
 */

#include "Common/header.h"
#include "Utils/Password.h"

struct entry {
	std::string site;
	std::string acc;
	Password pw;
	uint64_t id;
};

class Vault {
public:
	Vault();

	~Vault();

	/**
	 * @brief	Create new vault
	 */
	int create();

	/**
	 * @brief	Open existing vault
	 */
	int open();

private:
	uint8_t salt[SALT_SIZE];
	uint8_t iv[IV_SIZE];
};