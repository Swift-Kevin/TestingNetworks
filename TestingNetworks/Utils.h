#pragma once
#include "Defines.h"

namespace UTIL
{
	void UserInputMsg(char* _buffer, const char* _prompt)
	{
		std::cout << _prompt;
		std::cin >> _buffer;
		std::cin.clear();
		std::cin.ignore(INT_MAX, '\n');
		_buffer[BUFFER_SIZE - 1] = '\0';
	}
}