#pragma once
#include "Logger.h"

namespace NET
{
	struct ThreadInfo
	{
		NetInfo& netInfo;
		Debug& debugger;
		char* buffer;
		bool& runThreadLoop;
	};

	/// <summary>
	/// Cleans Up and Close a Connection to a sockets.
	/// </summary>
	/// <param name="_toClose">The socket to clean/close.</param>
	void CloseConnection(SOCKET _toClose)
	{
		closesocket(_toClose);
		WSACleanup();
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="_connection"></param>
	/// <param name="_debugger"></param>
	/// <returns></returns>
	int ShutdownProtocol(int _errorCode, NetInfo& _connection, Debug& _debugger)
	{
		NET::CloseConnection(_connection.socket);
		_debugger.CloseLog();
		return _errorCode;
	}

	/// <summary>
	/// Starts winsock, creates a socket, and initializes information regarding the connection.
	/// </summary>
	/// <param name="connection">The Net Struct to use and set up.</param>
	/// <param name="_ip">The IP to connect to.</param>
	/// <returns>0 = Successful Setup, otherwise the error code. </returns>
	int SetupConnection(NetInfo& connection, unsigned long _ip, Debug& _debugger)
	{
		int error = 0;
		// we are passing (2,2) in because we want to use Winsock Vers. 2.2 to communicate with
		if (WSAStartup(MAKEWORD(2, 2), &connection.wsaData) != 0)
		{
			// Log Errors
			Debug::Print("WSAStartup failed.", LogType::Error);
			_debugger.Log("WSAStartup failed.", LogType::Error);

			return 1;
		}

		// - AF INET = tcp and udp layers 
		// - data gram = connectionless, unreliable buffers of a fixed maximum length
		// - 0 = no protocol specified, the service provider will decide!
		connection.socket = socket(AF_INET, SOCK_DGRAM, 0);
		// checks to make sure that we created the socket succesfully
		if (connection.socket == INVALID_SOCKET)
		{
			error = WSAGetLastError();

			// Log Errors
			std::string msg = "Socket creation failed! Error Code: " + std::to_string(error);
			Debug::Print(msg.c_str(), LogType::Error);
			_debugger.Log(msg.c_str(), LogType::Error);

			WSACleanup();
			return error;
		}

		u_long ioMode = 1;
		if (ioctlsocket(connection.socket, FIONBIO, &ioMode) == SOCKET_ERROR)
		{
			// Log Errors
			std::string msg = "ioctlsocket failed with error: " + std::to_string(WSAGetLastError());
			Debug::Print(msg.c_str(), LogType::Error);
			_debugger.Log(msg.c_str(), LogType::Error);

			WSACleanup();
			return error;
		}

		connection.addr.sin_family = AF_INET;
		// htons converts from little endian to big endian, servers use little endian 
		connection.addr.sin_port = htons(SERVER_PORT);
		connection.addr.sin_addr.s_addr = _ip;

		/*
			Server Binds later in its own function
		*/

		Debug::Print("Started socket.", LogType::System);
		_debugger.Log("Started socket.", LogType::System);

		return 0;
	}

	/// <summary>
	/// Tries to recieve a message from a socket.
	/// </summary>
	/// <param name="_connection">The Net Struct to use and set up.</param>
	/// <param name="_buffer">Used to store the recieved message into.</param>
	/// <param name="_from">The address to receive from.</param>
	/// <param name="_fromSize">How large the address we are reading in from is.</param>
	/// <returns>The size in bytes of the message received. -1 = Error in message</returns>
	int TryRecieve(NetInfo& _connection, char* _buffer, sockaddr* _from, int* _fromSize, Debug& _debugger)
	{
		int msgByteSize = recvfrom(_connection.socket, _buffer, TOTAL_BUFFER_SIZE, 0, _from, _fromSize);

		// Was there in an error in recvfrom
		if (msgByteSize <= 0)
		{
			int lastErr = WSAGetLastError();

			if (lastErr == WSAEWOULDBLOCK)
			{
				return WSAEWOULDBLOCK;
			}
			else if (lastErr == WSAEINVAL)
			{
				return lastErr;
			}
			else
			{
				// Log Errors
				std::string msg = "recvfrom failed! Error Code: " + std::to_string(lastErr);
				Debug::Print(msg.c_str(), LogType::Error);
				_debugger.Log(msg.c_str(), LogType::Error);
				return -1;
			}
		}

		// Make sure message is terminated
		if (msgByteSize > 0 && msgByteSize < BUFFER_SIZE)
		{
			_buffer[msgByteSize] = '\0';
		}

		if (msgByteSize == -1)
		{
			msgByteSize = 0;
		}

		return msgByteSize;
	}

	/// <summary>
	/// Main Functionality for Running the Server
	/// </summary>
	/// <returns>Any errors found </returns>
	int ServerRun()
	{
		// WSA Data is used to start up the windows sockets.
		NetInfo serverInfo;
		std::string ip = "";

		// Multiple Clients
		//std::map<sockaddr_in, std::string> connectedClients;
		std::vector<ClientStorage> connectedClients;

		// Start custom debugger (log files / messages)
		Debug debugger;
		debugger.StartLog();

		// Buffer to read in messages
		char buffer[TOTAL_BUFFER_SIZE];

		int errorCode = NET::SetupConnection(serverInfo, ADDR_ANY, debugger);
		if (errorCode) { return ShutdownProtocol(errorCode, serverInfo, debugger); }

		// binds the udp socket to the socket of the server address
		// makes sure theres no binding error
		if (bind(serverInfo.socket, (sockaddr*)&serverInfo.addr, sizeof(serverInfo.addr)) == SOCKET_ERROR)
		{
			std::string msg = "Bind failed! Error Code: " + std::to_string(WSAGetLastError());
			Debug::Print(msg.c_str() + SERVER_PORT, LogType::System);
			debugger.Log(msg.c_str() + SERVER_PORT, LogType::System);

			return ShutdownProtocol(WSAGetLastError(), serverInfo, debugger);
		}

		// Print Success
		std::string msg = "UDP Server Running.";
		Debug::Print(msg.c_str(), LogType::System);
		debugger.Log(msg.c_str(), LogType::System);

		// Print IP
		msg = "IP: " + UTIL::GetIP();
		Debug::Print(msg.c_str(), LogType::System);
		debugger.Log(msg.c_str(), LogType::System);

		// Print Port
		msg = "Port: " + std::to_string(SERVER_PORT);
		Debug::Print(msg.c_str(), LogType::System);
		debugger.Log(msg.c_str(), LogType::System);

		// Receive and respond to messages
		sockaddr_in clientAddr;
		clientAddr.sin_family = AF_INET;
		clientAddr.sin_port = htons(SERVER_PORT);
		clientAddr.sin_addr.s_addr = inet_addr(UTIL::GetIP().c_str());

		int clientAddrSize = sizeof(clientAddr);
		int sizeOfBytesRecv = 0;
		UTIL::BufferTypes bufferType;

		bool runServer = true;

		// Main Server Loop
		while (runServer)
		{
			// Server Runner wants to quit.
			if (GetAsyncKeyState(VK_ESCAPE))
			{
				// tell all connected clients to disconnect.
				// break out of loop
				runServer = false;
			}

			sizeOfBytesRecv = NET::TryRecieve(serverInfo, buffer, (sockaddr*)&clientAddr, &clientAddrSize, debugger);

			// Check for errors
			if (sizeOfBytesRecv == WSAEWOULDBLOCK) { continue; }
			if (sizeOfBytesRecv == SOCKET_ERROR) { break; }

			bufferType = (UTIL::BufferTypes)((char)buffer[0]);

			switch (bufferType)
			{
			case UTIL::BufferTypes::Server:
			{
				// Client should not be sending 'server' status messages.
				// there is an error in the code somewhere causing this.
				throw("ERROR in Client Code. Client sent message with first character as 'Server' (0). Check call stack.");
				break;
			}
			case UTIL::BufferTypes::Client:
			{
				// Make debug message
				std::string rcvMsg = (buffer + 1);

				int idx = UTIL::FindIDXOfClient(connectedClients, clientAddr);
				if (idx == -1)
				{
					std::string msg = "Not found in list.";
					Debug::Print(msg.c_str(), LogType::Server);
					debugger.Log(msg.c_str(), LogType::Server);
					break;
				}

				std::string printMsg = connectedClients[idx].name + rcvMsg;
				printMsg = (char)UTIL::BufferTypes::Client + printMsg;

				// Print out to the console what was read in.
				Debug::PrintUserMessage((buffer + 1), connectedClients[idx].name);
				debugger.Log(printMsg.c_str(), LogType::Client);

				// send to all connected clients, except client who sent it
				for (const auto& [saddr, name] : connectedClients)
				{
					// not client who originally sent the message
					if (!(saddr.sin_addr.s_addr == clientAddr.sin_addr.s_addr && saddr.sin_port == clientAddr.sin_port))
					{
						sendto(serverInfo.socket, printMsg.c_str(), (int)strlen(printMsg.c_str()), 0, (sockaddr*)&saddr, sizeof(saddr));
					}
				}
				break;
			}
			case UTIL::BufferTypes::Join:
			{
				std::string msg = "New User - ";
				msg += (buffer + 1);

				Debug::Print(msg.c_str(), LogType::Server);

				ClientStorage add;
				add.addr = clientAddr;

				char* dynName = new char[15];
				strcpy_s(dynName, 15, buffer + 1);
				add.name = dynName;

				connectedClients.push_back(add);
				break;
			}
			case UTIL::BufferTypes::Disconnect:
			{
				int idx = UTIL::FindIDXOfClient(connectedClients, clientAddr);
				if (idx == -1)
				{
					std::string msg = "D/C Case -- Not found in list.";
					Debug::Print(msg.c_str(), LogType::Server);
					debugger.Log(msg.c_str(), LogType::Server);
					break;
				}

				delete[] connectedClients[idx].name;
				connectedClients.erase(connectedClients.begin() + idx);

				break;
			}
			default:
				break;
			}
		}

		// Cleanup
		return ShutdownProtocol(0, serverInfo, debugger);
	}

	void ClientSend(ThreadInfo* _ti)
	{
		while (_ti->runThreadLoop)
		{
			// if buffer is empty
			if (UTIL::UserInputMsg(_ti->buffer, "[You] : ")) { continue; }

			// Send message to server
			int bytesSent = sendto(_ti->netInfo.socket, _ti->buffer, TOTAL_BUFFER_SIZE, 0, (sockaddr*)&_ti->netInfo.addr, sizeof(_ti->netInfo.addr));
			// If error sending bytes
			if (bytesSent == SOCKET_ERROR)
			{
				// Cant put "msg" into the method call due to unscoped type
				std::string msg = "sendto failed! Code: " + std::to_string(WSAGetLastError());
				Debug::Print(msg.c_str(), LogType::Error);
				_ti->debugger.Log(msg.c_str(), LogType::Error);

				break;
			}

			// Check if user exited via msg
			if (_strnicmp(_ti->buffer + 1, "exit", 4) == 0)
			{
				std::string msg = "Exit called. Breaking out of loop.";
				Debug::Print(msg.c_str(), LogType::System);
				_ti->debugger.Log(msg.c_str(), LogType::System);

				_ti->buffer[0] = (char)UTIL::BufferTypes::Disconnect;
				int errorCode = sendto(_ti->netInfo.socket, _ti->buffer, TOTAL_BUFFER_SIZE, 0, (sockaddr*)&_ti->netInfo.addr, sizeof(_ti->netInfo.addr));
				if (errorCode == SOCKET_ERROR)
				{
					msg = "sendto - EXIT - failed! Code: " + std::to_string(WSAGetLastError());
					Debug::Print(msg.c_str(), LogType::Error);
					_ti->debugger.Log(msg.c_str(), LogType::Error);
				}

				_ti->runThreadLoop = false;
				break;
			}
		}
	}

	void ClientRecv(ThreadInfo* _ti)
	{
		while (_ti->runThreadLoop)
		{
			// Receive response from server
			sockaddr_in fromAddr;
			int fromLen = sizeof(fromAddr);

			int sizeOfBytesRecv = NET::TryRecieve(_ti->netInfo, _ti->buffer, (sockaddr*)&fromAddr, &fromLen, _ti->debugger);
			// if it would have blocked, skip over it.
			if (sizeOfBytesRecv == WSAEWOULDBLOCK || sizeOfBytesRecv == WSAEINVAL) { continue; }
			// If it was an error, get out of the loop
			if (sizeOfBytesRecv == SOCKET_ERROR) { break; }

			// Erase current line (asks for users input)
			std::cout << "\x1b[2K";

			LogType debugType = (bool)_ti->buffer[0] ? LogType::Server : LogType::Client;

			UTIL::BufferTypes bufferType = (UTIL::BufferTypes)((char)_ti->buffer[0]);
			if (bufferType == UTIL::BufferTypes::Disconnect || bufferType == UTIL::BufferTypes::Join)
				continue;

			std::string bufConv = _ti->buffer;
			std::string name = bufConv.substr(1, 15);

			Debug::PrintUserMessage(_ti->buffer + 15, name);
			_ti->debugger.Log(_ti->buffer + 1, LogType::Client);

			// re print out user input message
			std::cout << "[You] : ";
		}
	}

	/// <summary>
	/// Main functionality to run the Client
	/// - Should not use a Debug object.
	/// </summary>
	/// <returns>Any errors found </returns>
	int ClientRun()
	{
		std::string clientName = UTIL::UserInputMsg("Display Name (1-15 characters): ", 15);

		// Get IP to connect to from user
		std::string userEnteredIP = "";
		do {
			userEnteredIP = UTIL::UserInputMsg("IP: ");
		} while (inet_addr(userEnteredIP.c_str()) == INADDR_NONE);

		// WSA Data is used to start up the windows sockets.
		NetInfo clientInfo;

		Debug debugger;
		debugger.StartLog();

		// Buffers to read messages to
		char clientBuffer[TOTAL_BUFFER_SIZE] = { 0 };
		char serverBuffer[TOTAL_BUFFER_SIZE] = { 0 };

		int errorCode = NET::SetupConnection(clientInfo, inet_addr(userEnteredIP.c_str()), debugger);
		if (errorCode) { return ShutdownProtocol(errorCode, clientInfo, debugger); }

		// Send name over to server
		clientName = (char)UTIL::BufferTypes::Join + clientName + '\0';
		int bytesSent = sendto(clientInfo.socket, clientName.c_str(), TOTAL_BUFFER_SIZE, 0, (sockaddr*)&clientInfo.addr, sizeof(clientInfo.addr));

		Debug::Print("UDP Client ready. Type messages to send to the server.", LogType::System);
		bool runThreadLoop = true;

		ThreadInfo sendThreadInfo = { clientInfo, debugger, clientBuffer, runThreadLoop };
		ThreadInfo recvThreadInfo = { clientInfo, debugger, serverBuffer, runThreadLoop };

		std::thread sendThread(ClientSend, &sendThreadInfo);
		std::thread recvThread(ClientRecv, &recvThreadInfo);

		// Blocks main thread from running.
		// Good b/c I want a thread for I&O
		sendThread.join();
		recvThread.join();

		// Cleanup
		return ShutdownProtocol(0, clientInfo, debugger);
	}
}