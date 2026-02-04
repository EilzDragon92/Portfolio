/**
 * @file	main.cpp
 * @brief	Entry point of the application
 * @author	EilzDragon92
 */

#include "Common/header.h"

int main() {
	uint8_t buff[4];

	Random(buff, 4);

	for (int i = 0; i < 4; i++) printf("%02x ", buff[i]);

	return 0;
}