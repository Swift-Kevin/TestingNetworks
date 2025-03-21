#pragma once
#include "Utils.h"

enum class LogType : byte
{
	Log = 0b1,
	Server = 0b10,
	System = 0b11,
	Debug = 0b100,
	Warning = 0b101,
	Client = 0b110,
	Error = 0b111
};



class Debug
{
private:
	
	// Member Fields
	std::ofstream logFile;
	bool fileAlreadyMade = false;
	std::string fileName = "";
	
	// Methods
	
	/// <summary>
	/// Creates a prefix for the log type
	/// </summary>
	/// <param name="_type">The type to convert.</param>
	/// <returns>The Prefix to write out to the file.</returns>
	static const char* ConvertLogType(LogType _type)
	{
		switch (_type)
		{
		case LogType::Log:
			return "[Log] : ";
		case LogType::System:
			return "[System] : ";
		case LogType::Server:
			return "[Server] : ";
		case LogType::Debug:
			return "[Debug] : ";
		case LogType::Warning:
			return "[Warning] : ";
		case LogType::Error:
			return "[Error] : ";
		case LogType::Client:
			return "[Client] : ";
		default:
			break;
		}
		return "INVALID";
	}


public:

	/// <summary>
	/// Writes out the given line to the log file timestamped from the servers start.
	/// </summary>
	/// <param name="_tolog">The message to write out.</param>
	/// <param name="_type">The severity of the log.</param>
	void Log(const char* _toLog, LogType _type)
	{
		logFile.open(fileName, std::ios_base::app);
		if (logFile.is_open())
		{
			// write to console then log it
			logFile << ConvertLogType(_type) << _toLog << '\n';
		}
		logFile.close();
	}

	/// <summary>
	/// Starts up the console log with a timestamp to when it was called. 
	/// </summary>
	void StartLog()
	{
		if (!fileAlreadyMade)
		{
			fileAlreadyMade = !fileAlreadyMade;

			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			char buffer[128];

			fileName += "./Logs/";
			strftime(buffer, 100, "%Y-%m-%d_%H-%M-%S", std::localtime(&now));
			fileName += buffer;
			fileName += "_Log.txt";

			// Check to make sure the folder is there.
			if (!std::filesystem::is_directory("./Logs/"))
			{
				std::filesystem::create_directory("./Logs/");
			}

			logFile.open(fileName, std::ios_base::app);
			if (logFile.is_open())
			{
				logFile << ConvertLogType(LogType::System) << UTIL::MSG_LogStart << '\n';
				Print(UTIL::MSG_LogStart, LogType::System);
			}
			logFile.close();
		}
		else
		{
			Print("StartLog() was called, but was already started.", LogType::Error);
		}
	}

	/// <summary>
	/// Closes the log that was paired since the start, allows for more logs to be ran during servers run time.
	/// </summary>
	void CloseLog()
	{
		logFile.open(fileName);
		if (logFile.is_open())
		{
			Print(UTIL::MSG_LogEnd, LogType::System);
			logFile << ConvertLogType(LogType::System) << UTIL::MSG_LogEnd;
		}
		logFile.close();

		// Allow for more logs to be made after server started. Only if we closed out the log.
		fileAlreadyMade = !fileAlreadyMade;
		// Clean File Name
		fileName = "";
	}

	/// <summary>
	/// Prints out a message to the console. Takes care of null termination.
	/// </summary>
	/// <param name="_message">The message to print.</param>
	/// <param name="_type">The type to prefix the print with.</param>
	static void Print(const char* _message, LogType _type)
	{
		std::cout << ConvertLogType(_type) << _message << "\n\0";
	}

};