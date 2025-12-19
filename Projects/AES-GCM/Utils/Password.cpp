/**
 * @file	Password.cpp
 * @brief	Implementation of Password class
 * @author	EilzDragon92
 */


#include "Utils/Password.h"

bool Password::isEmpty() {
	return size == 0;
}

const char *Password::getData() {
	return data;
}

int Password::getSize() {
	return size;
}

void Password::setData(const Password &pw) {
	setData(pw.data, pw.size);
}

void Password::setData(const QString &str) {
	std::string tmp = str.toStdString();

	setData(tmp);

	Wipe(const_cast<char *>(tmp.data()), tmp.size());

	tmp.clear();
}

void Password::setData(const string &str) {
	clean();

	size = str.size();
	data = new char[size + 1];

	for (int i = 0; i < size; i++) data[i] = str[i];

	data[size] = '\0';
}

void Password::setData(const char *str, size_t len) {
	clean();

	if (str && len > 0) {
		size = len;
		data = new char[size + 1];

		memcpy(data, str, size);

		data[size] = '\0';
	}
}

void Password::clean() {
	if (data != nullptr) {
		Wipe(data, size + 1);
		delete[] data;
	}

	data = nullptr;
	size = 0;
}