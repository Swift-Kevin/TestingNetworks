#include <iostream>
#include <string>
// Note: Had to include this so the API would not yell about inet_pton being an issue
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>

// Constant Loopback IP for testing
// Change if running on a different machine
const char* SERVER_IP = "127.0.0.1";

// Defining the port and buffer size so I can reuse without having to worry about it
#define SERVER_PORT 31337
#define BUFFER_SIZE 512 

struct NetInfo {
	WSADATA wsaData;
	SOCKET socket;
	sockaddr_in addr;
};

void UserInputMsg(char* buffer)
{
	std::cout << "Enter message: ";
	std::cin >> buffer;
	std::cin.clear();
	std::cin.ignore(INT_MAX, '\n');
	buffer[BUFFER_SIZE - 1] = '\0';
}

void CloseConnection(SOCKET _toClose)
{
	closesocket(_toClose);
	WSACleanup();
}

int SetupConnection(NetInfo& connection, unsigned long _ip)
{
	// we are passing (2,2) in because we want to use Winsock Vers. 2.2 to communicate with
	if (WSAStartup(MAKEWORD(2, 2), &connection.wsaData) != 0) {
		std::cout << "WSAStartup failed!\n";
		return 1;
	}

	// - AF INET = tcp and udp layers 
	// - data gram = connectionless, unreliable buffers of a fixed maximum length
	// - 0 = no protocol specified, the service provider will decide!
	connection.socket = socket(AF_INET, SOCK_DGRAM, 0);
	// checks to make sure that we created the socket succesfully
	if (connection.socket == INVALID_SOCKET) {
		std::cout << "Socket creation failed! Error: " << WSAGetLastError() << "\n";
		WSACleanup();
		return 1;
	}

	connection.addr.sin_family = AF_INET;
	// htons converts from little endian to big endian, servers use little endian 
	connection.addr.sin_port = htons(SERVER_PORT);
	connection.addr.sin_addr.s_addr = _ip;

	return 0;
}

int TryRecieve(NetInfo& connection, char* buffer, sockaddr* from, int* fromLen)
{
	int msgByteSize = recvfrom(connection.socket, buffer, BUFFER_SIZE, 0, from, fromLen);

	// Was there in an error in recvfrom
	if (msgByteSize == SOCKET_ERROR) {
		std::cout << "recvfrom failed! Error: " << WSAGetLastError() << "\n";
		return -1;
	}

	// Make sure message is terminated
	buffer[msgByteSize] = '\0';

	return msgByteSize;
}

int ServerRun()
{
	// WSA Data is used to start up the windows sockets.
	NetInfo serverInfo;
	sockaddr_in clientAddr;
	
	// Buffer to read in messages
	char buffer[BUFFER_SIZE];

	if (int val = SetupConnection(serverInfo, ADDR_ANY)) { return val; }

	// binds the udp socket to the socket of he server address
	// makes sure theres no binding error
	if (bind(serverInfo.socket, (sockaddr*)&serverInfo.addr, sizeof(serverInfo.addr)) == SOCKET_ERROR) {
		std::cout << "Bind failed! Error: " << WSAGetLastError() << "\n";
		CloseConnection(serverInfo.socket);
		return 1;
	}

	// We successfully made a UDP server and its running on the port!
	std::cout << "UDP Server is running on port " << SERVER_PORT << "...\n";

	// Receive and respond to messages
	int clientAddrSize = sizeof(clientAddr);

	while (true)
	{
		int sizeOfBytesRecv = TryRecieve(serverInfo, buffer, (sockaddr*)&clientAddr, &clientAddrSize);
		if (sizeOfBytesRecv == SOCKET_ERROR) { break; } // break out of loop, so we can still clean socket

		// Print out to the console what was read in.
		std::cout << "Received: " << buffer << "\n";

		// Send message to client saying we got the message
		const char* response = "Message received!";
		sendto(serverInfo.socket, response, strlen(response), 0, (sockaddr*)&clientAddr, clientAddrSize);
	}

	// Cleanup
	CloseConnection(serverInfo.socket);
	return 0;
}

int ClientRun()
{
	// WSA Data is used to start up the windows sockets.
	NetInfo clientInfo;

	// Buffers to read messages to
	char clientBuffer[BUFFER_SIZE] = { 0 };
	char serverBuffer[BUFFER_SIZE] = { 0 };

	int val = SetupConnection(clientInfo, inet_addr(SERVER_IP));
	if (val) { return val; }

	std::cout << "UDP Client ready. Type messages to send to the server.\n";

	while (true) 
	{
		UserInputMsg(clientBuffer);

		// Send message to server
		int bytesSent = sendto(clientInfo.socket, clientBuffer, BUFFER_SIZE, 0, (sockaddr*)&clientInfo.addr, sizeof(clientInfo.addr));
		if (bytesSent == SOCKET_ERROR) {
			std::cout << "sendto failed! Error: " << WSAGetLastError() << "\n";
			break;
		}

		// Receive response from server
		sockaddr_in fromAddr;
		int fromLen = sizeof(fromAddr);

		int sizeOfBytesRecv = TryRecieve(clientInfo, serverBuffer, (sockaddr*)&fromAddr, &fromLen);
		if (sizeOfBytesRecv == SOCKET_ERROR) { break; }

		std::cout << "Server: " << serverBuffer << "\n";

		// Check if user exited via msg
		char exitCheck[4] = { clientBuffer[0], clientBuffer[1], clientBuffer[2], clientBuffer[3] };
		if (_stricmp(exitCheck, "exit") == 0) {
			std::cout << "Exit called. Breaking out of loop.\n";
			break;
		}
	}

	// Cleanup
	CloseConnection(clientInfo.socket);
	return 0;
}

int main()
{
	std::cout << "Server (1) or Client (2): ";
	int opt = 0;

	do { std::cin >> opt; } while (!(opt == 1 || opt == 2));

	opt == 1 ? ServerRun() : ClientRun();
}