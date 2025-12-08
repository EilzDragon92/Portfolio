#pragma once

#include "Common/header.h"

/* RAII class that securely handles password */
class Password {
public:
	~Password() {
		clean();
	}

	bool isEmpty();

	const char *getData();

	int getSize();

	void setData(const Password &pw);

	void setData(const QString &str);

	void setData(const string &str);

	void setData(const char *str, size_t len);

private:
	char *data = nullptr;
	size_t size = 0;

	void clean();
};