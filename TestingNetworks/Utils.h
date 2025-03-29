#pragma once
#include "Defines.h"

namespace UTIL
{
	static const char* MSG_LogStart = "Server Started.";
	static const char* MSG_LogEnd = "End of Console Logging.";
	static const char* MSG_Client_Recv = "Message received!";

	enum class ConsoleColor
	{
		Black,
		Red,
		Green,
		Yellow,
		Blue,
		Magenta,
		Cyan,
		White,
		Default = 9
	};

	/// <summary>
	/// Asks the user for input, made for NETWORKING CALLS
	/// </summary>
	/// <param name="_buffer">The buffer to fill with input</param>
	/// <param name="_prompt">A prompt displayed to the user</param>
	/// <returns>Status of the buffer if its empty or not.</returns>
	bool UserInputMsg(char* _buffer, const char* _prompt)
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
	
		return buf.empty();
	}

	std::string UserInputMsg(const char* _prompt)
	{
		std::cout << _prompt;
		
		std::string buf = "";
		std::getline(std::cin, buf, '\n');
		std::cin.clear();

		return buf;
	}

	std::string GetIP()
	{
		char hostBuffer[256];
		gethostname(hostBuffer, sizeof(hostBuffer));

		hostent* host = gethostbyname(hostBuffer);
		std::string serverIP;
		if (host)
		{
			serverIP = inet_ntoa(*(in_addr*)host->h_addr);
		}

		return serverIP;
	}

	/// <summary>
	/// Sets the console's foreground color
	/// </summary>
	/// <param name="foreColor"></param>
	void SetForegroundColor(ConsoleColor foreColor)
	{
		std::cout << "\x1B" << "[" << (int)foreColor + 30 << "m";
	}
}