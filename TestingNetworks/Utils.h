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

	enum class BufferTypes
	{
		Server,
		User
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
		std::strncpy(_buffer + 1, buf.c_str(), BUFFER_SIZE - 1);
		std::cin.clear();

		_buffer[BUFFER_SIZE - 1] = '\0';
		// This is a user message. prepend that to buffer.
		_buffer[0] = (int)UTIL::BufferTypes::User;
	
		return buf.empty();
	}

	/// <summary>
	/// Gets an input from the user with a prompt
	/// </summary>
	/// <param name="_prompt">To display to the user, the question to ask</param>
	/// <returns>The users response</returns>
	std::string UserInputMsg(const char* _prompt)
	{
		std::cout << _prompt;
		
		std::string buf = "";
		std::getline(std::cin, buf, '\n');
		std::cin.clear();

		return buf;
	}

	/// <summary>
	/// Gets the IP of the current machine
	/// </summary>
	/// <returns>IP in string format</returns>
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