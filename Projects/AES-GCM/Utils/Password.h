/**
 * @file	Password.h
 * @brief	RAII class that securely handles password
 * @author	EilzDragon92
 */

#pragma once

#include "Common/header.h"

/**
 * @class	Password
 * @brief	RAII class that securely handles password
 */
class Password {
public:
	/**
	 * @brief   Destructor of Password class
	 */
	~Password() {
		clean();
	}

	/**
	 * @brief   Check the password data is empty
	 * @return	true if empty
	 */
	bool isEmpty();

	/**
	 * @brief   Get password data
	 * @return	Password data
	 */
	const char *getData();

	/**
	 * @brief   Get password size
	 * @return	Password size
	 */
	int getSize();

	/**
	 * @brief   Set password data
	 * @param	pw	Source
	 */
	void setData(const Password &pw);

	/**
	 * @brief   Set password data
	 * @param	pw	Source
	 */
	void setData(const QString &str);

	/**
	 * @brief   Set password data
	 * @param	pw	Source
	 */
	void setData(const string &str);

	/**
	 * @brief   Set password data
	 * @param	str		Source
	 * @param	len		Password length
	 */
	void setData(const char *str, size_t len);

private:
	char *data = nullptr;
	size_t size = 0;

	/**
	 * @brief   Securely wipe password data
	 */
	void clean();
};