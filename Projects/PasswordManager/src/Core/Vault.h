/**
 * @file	Vault.h
 * @brief	Password encryption, storage, and retrieval class
 * @author	EilzDragon92
 */

#include "Common/header.h"

class Vault {
public:
	Vault();

	~Vault();

private:
	/**
	 * @brief	Create new password vault
	 */
	int createVault();

	/**
	 * @brief	Open existing password vault
	 */
	int openVault();
};