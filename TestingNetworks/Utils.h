#pragma once
#include "Defines.h"

namespace UTIL
{
	static const char* Message_LogStart = "Server Started.";
	static const char* Message_LogEnd = "End of Console Logging.";

	void UserInputMsg(char* _buffer, const char* _prompt)
	{
		std::cout << _prompt;
		
		// Get Input
		std::string buf = "";
		std::getline(std::cin, buf, '\n');

		// Copy it over to buffer
		std::strncpy(_buffer, buf.c_str(), BUFFER_SIZE - 1);

		std::cin.clear();
		//std::cin.ignore(INT_MAX, '\n');
		_buffer[BUFFER_SIZE - 1] = '\0';
	}
}