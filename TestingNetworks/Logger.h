#pragma once
#include "Utils.h"

enum class LogType
{
	Log,
	Server,
	System,
	Debug,
	Warning,
	Error
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
	const char* ConvertLogType(LogType _type)
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
			logFile << ConvertLogType(_type) << _toLog;
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

			logFile.open(fileName);
			if (logFile.is_open())
			{
				logFile << ConvertLogType(LogType::System) << "Started Logging.\n\0";
				Print("Started Logging.\n\0", LogType::Server);
			}
			logFile.close();
		}
		else
		{
			Print("StartLog() was called, but was already started.\n\0", LogType::Error);
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
			Print("End of Console Logging...\n\0", LogType::System);
			logFile << ConvertLogType(LogType::System) << "Closing Console...\n\0";
		}
		logFile.close();

		// Allow for more logs to be made after server started. Only if we closed out the log.
		fileAlreadyMade = !fileAlreadyMade;
	}

	/// <summary>
	/// Prints out a message to the console.
	/// </summary>
	/// <param name="_message">The message to print.</param>
	/// <param name="_type">The type to prefix the print with.</param>
	void Print(const char* _message, LogType _type)
	{
		std::cout << ConvertLogType(_type) << _message;
	}

};