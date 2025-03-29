#pragma once
#include "Logger.h"

namespace NET
{
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
	/// Starts winsock, creates a socket, and initializes information regarding the connection.
	/// </summary>
	/// <param name="connection">The Net Struct to use and set up.</param>
	/// <param name="_ip">The IP to connect to.</param>
	/// <returns>0 = Successful Setup, otherwise the error code. </returns>
	int SetupServer(NetInfo& connection, unsigned long _ip, Debug& _debugger)
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
		int msgByteSize = recvfrom(_connection.socket, _buffer, BUFFER_SIZE, 0, _from, _fromSize);

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
		std::vector<sockaddr_in> connectedClients;

		// Start custom debugger (log files / messages)
		Debug debugger;
		debugger.StartLog();

		// Buffer to read in messages
		char buffer[BUFFER_SIZE];

		int errorCode = NET::SetupServer(serverInfo, ADDR_ANY, debugger);
		if (errorCode) { return errorCode; }

		// binds the udp socket to the socket of the server address
		// makes sure theres no binding error
		if (bind(serverInfo.socket, (sockaddr*)&serverInfo.addr, sizeof(serverInfo.addr)) == SOCKET_ERROR)
		{
			std::string msg = "Bind failed! Error Code: " + std::to_string(WSAGetLastError());
			Debug::Print(msg.c_str() + SERVER_PORT, LogType::System);
			debugger.Log(msg.c_str() + SERVER_PORT, LogType::System);

			// Clean Up Connections
			NET::CloseConnection(serverInfo.socket);
			debugger.CloseLog();

			return 1;
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

		// Main Server Loop
		while (true)
		{
			int sizeOfBytesRecv = NET::TryRecieve(serverInfo, buffer, (sockaddr*)&clientAddr, &clientAddrSize, debugger);

			if (sizeOfBytesRecv == WSAEWOULDBLOCK) { continue; }

			// break out of loop, so we can still clean socket
			if (sizeOfBytesRecv == SOCKET_ERROR) { break; }

			// Make debug message
			std::string receivedMsg = "User: [" + std::string(inet_ntoa(clientAddr.sin_addr)) + "]: " + buffer;

			// Print out to the console what was read in.
			Debug::Print(receivedMsg.c_str(), LogType::Client);
			debugger.Log(receivedMsg.c_str(), LogType::Client);
						
			// Write back to sender
			//	int errorCode = sendto(serverInfo.socket, UTIL::MSG_Client_Recv, (int)strlen(UTIL::MSG_Client_Recv), 0, (sockaddr*)&clientAddr, clientAddrSize);
			//	if (errorCode == SOCKET_ERROR)
			//	{
			//		std::string msg = "sendto failed! Code: " + std::to_string(WSAGetLastError());
			//		Debug::Print(msg.c_str(), LogType::Error);
			//		debugger.Log(msg.c_str(), LogType::Error);
			//	}

			bool unknownClient = true;
			for (const sockaddr_in& client : connectedClients)
			{
				if ((client.sin_addr.s_addr == clientAddr.sin_addr.s_addr) && (client.sin_port == clientAddr.sin_port))
				{
					unknownClient = false;
					break;
				}
			}

			if (unknownClient)
			{
				Debug::Print("Connected New User", LogType::Server);
				connectedClients.push_back(clientAddr);
			}

			// send to all connected clients, except client who sent it
			for (const sockaddr_in& client : connectedClients)
			{
				// not client who originally sent the message
				if (!(client.sin_addr.s_addr == clientAddr.sin_addr.s_addr && client.sin_port == clientAddr.sin_port))
				{
					sendto(serverInfo.socket, receivedMsg.c_str(), (int)strlen(receivedMsg.c_str()), 0, (sockaddr*)&client, sizeof(client));
				}
			}
		}

		// Cleanup
		NET::CloseConnection(serverInfo.socket);
		debugger.CloseLog();
		return 0;
	}

	/// <summary>
	/// Main functionality to run the Client
	/// - Should not use a Debug object.
	/// </summary>
	/// <returns>Any errors found </returns>
	int ClientRun()
	{
		// get ip to connect to from user
		std::string userIp = "";
		while (true)
		{
			userIp = UTIL::UserInputMsg("IP: ");

			if (inet_addr(userIp.c_str()) != INADDR_NONE)
			{
				// good ip entered
				break;
			}
		}

		// WSA Data is used to start up the windows sockets.
		NetInfo clientInfo;

		Debug debugger;
		debugger.StartLog();

		// Buffers to read messages to
		char clientBuffer[BUFFER_SIZE] = { 0 };
		char serverBuffer[BUFFER_SIZE] = { 0 };

		int errorCode = NET::SetupServer(clientInfo, inet_addr(userIp.c_str()), debugger);
		if (errorCode) { return errorCode; }

		Debug::Print("UDP Client ready. Type messages to send to the server.", LogType::System);

		bool runThreadLoop = true;
		
		std::thread sendThread([&]()
			{
				while (runThreadLoop)
				{
					// if buffer is empty
					if (UTIL::UserInputMsg(clientBuffer, "[You] : ")) { continue; }

					// Send message to server
					int bytesSent = sendto(clientInfo.socket, clientBuffer, BUFFER_SIZE, 0, (sockaddr*)&clientInfo.addr, sizeof(clientInfo.addr));
					// If error sending bytes
					if (bytesSent == SOCKET_ERROR)
					{
						// Cant put "msg" into the method call due to unscoped type
						std::string msg = "sendto failed! Code: " + std::to_string(WSAGetLastError());
						Debug::Print(msg.c_str(), LogType::Error);
						debugger.Log(msg.c_str(), LogType::Error);

						break;
					}

					// Check if user exited via msg
					if (_strnicmp(clientBuffer, "exit", 4) == 0)
					{
						std::string msg = "Exit called. Breaking out of loop.";
						Debug::Print(msg.c_str(), LogType::System);
						debugger.Log(msg.c_str(), LogType::System);

						runThreadLoop = false;
						break;
					}
				}
			}
		);

		std::thread recvThread([&]()
			{
				while (runThreadLoop)
				{
					// Receive response from server
					sockaddr_in fromAddr;
					int fromLen = sizeof(fromAddr);

					int sizeOfBytesRecv = NET::TryRecieve(clientInfo, serverBuffer, (sockaddr*)&fromAddr, &fromLen, debugger);
					// if it would have blocked, skip over it.
					if (sizeOfBytesRecv == WSAEWOULDBLOCK || sizeOfBytesRecv == WSAEINVAL) { continue; }
					// If it was an error, get out of the loop
					if (sizeOfBytesRecv == SOCKET_ERROR) { break; }

					// Erase current line (asks for users input)
					std::cout << "\x1b[2K" << '\r';

					Debug::Print(serverBuffer, LogType::Server);
					debugger.Log(serverBuffer, LogType::Server);

					// re print out user input message
					std::cout << "[You] : ";
				}
			}
		);

		// Blocks main thread from running.
		// Good b/c I want a thread for I&O
		sendThread.join();
		recvThread.join();

		NET::CloseConnection(clientInfo.socket);
		debugger.CloseLog();
		return 0;
	}
}