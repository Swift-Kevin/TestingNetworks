#include <iostream>
// Note: Had to include this so the API would not yell about inet_pton being an issue
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>

// Constant Loopback IP for testing
// ChatGPT: Change if running on a different machine
const char* SERVER_IP = "127.0.0.1";

// Defining the port and buffer size so I can reuse without having to worry about it
#define SERVER_PORT 31337
#define BUFFER_SIZE 512 

int ServerRun()
{
	// WSA Data is used to start up the windows sockets.
	WSADATA wsaData;

	SOCKET udpSocket;
	sockaddr_in serverAddr, clientAddr;
	// Create a buffer with the given size so we can read in messages.
	// The buffer size is defined at the top
	char buffer[BUFFER_SIZE];

	// ChatGPT: Step 1: Initialize Winsock

	// we are passing (2,2) in because we want to use Winsock Vers. 2.2 to communicate with
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "WSAStartup failed!\n";
		return 1;
	}

	// ChatGPT: Step 2: Create UDP socket

	// - AF INET = tcp and udp layers 
	// - data gram = connectionless, unreliable buffers of a fixed maximum length
	// - 0 = no protocol specified, the service provider will decide!
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	// checks to make sure that we created the socket succesfully
	if (udpSocket == INVALID_SOCKET) {
		std::cout << "Socket creation failed! Error: " << WSAGetLastError() << "\n";
		WSACleanup();
		return 1;
	}

	// ChatGPT: Step 3: Bind the socket

	serverAddr.sin_family = AF_INET;
	// htons converts from little endian to big endian
	// servers use little endian 
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	// binds the udp socket to the socket of he server address
	// makes sure theres no binding error
	if (bind(udpSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cout << "Bind failed! Error: " << WSAGetLastError() << "\n";
		closesocket(udpSocket);
		WSACleanup();
		return 1;
	}

	// We successfully made a UDP server and its running on the port!
	std::cout << "UDP Server is running on port " << SERVER_PORT << "...\n";

	// ChatGPT: Step 4: Receive and respond to messages

	// how large the client is
	int clientAddrSize = sizeof(clientAddr);
	while (true)
	{
		int sizeOfBytesRecv = recvfrom(udpSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrSize);

		// Any errors in reciving data	
		if (sizeOfBytesRecv == SOCKET_ERROR)
		{
			std::cout << "recvfrom failed! Error: " << WSAGetLastError() << "\n";
			break;
		}

		// ChatGPT: Null-terminate received data
		buffer[sizeOfBytesRecv] = '\0';

		// Print out to the console what was read in.
		std::cout << "Received: " << buffer << "\n";

		// ChatGPT: Send response

		// Send a response back saying we recieved the message
		const char* response = "Message received!";
		// Sends back it to the client address
		sendto(udpSocket, response, strlen(response), 0, (sockaddr*)&clientAddr, clientAddrSize);
	}

	// ChatGPT: Step 5: Cleanup
	closesocket(udpSocket);
	WSACleanup();
	return 0;
}

int ClientRun()
{
	// WSA Data is used to start up the windows sockets.
	WSADATA wsaData;
	SOCKET udpSocket;
	sockaddr_in serverAddr;

	// Create a buffer with the given size so we can read in messages.
	// The buffer size is defined at the top
	char buffer[BUFFER_SIZE];

	// ChatGPT: Step 1: Initialize Winsock

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cout << "WSAStartup failed!\n";
		return 1;
	}

	// ChatGPT: Step 2: Create UDP socket

	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (udpSocket == INVALID_SOCKET) {
		std::cout << "Socket creation failed! Error: " << WSAGetLastError() << "\n";
		WSACleanup();
		return 1;
	}

	// ChatGPT: Step 3: Set up server address

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	std::cout << "UDP Client ready. Type messages to send to the server.\n";

	while (true) {
		std::cout << "Enter message: ";
		std::cin.getline(buffer, BUFFER_SIZE);

		// ChatGPT: Send message to server
		int bytesSent = sendto(udpSocket, buffer, strlen(buffer), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));
		if (bytesSent == SOCKET_ERROR) {
			std::cout << "sendto failed! Error: " << WSAGetLastError() << "\n";
			break;
		}

		// ChatGPT: Receive response from server
		sockaddr_in fromAddr;
		int fromLen = sizeof(fromAddr);

		int sizeOfBytesRecv = recvfrom(udpSocket, buffer, BUFFER_SIZE, 0, (sockaddr*)&fromAddr, &fromLen);
		if (sizeOfBytesRecv == SOCKET_ERROR) {
			std::cout << "recvfrom failed! Error: " << WSAGetLastError() << "\n";
			break;
		}

		// ChatGPT: Null-terminate received data
		buffer[sizeOfBytesRecv] = '\0'; 
		std::cout << "Server: " << buffer << "\n";

		// Reason why its down here:
		// At least have the server know that we exited!
		if (buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't')
		{
			std::cout << "Exit called. Breaking out of loop.\n";
			break;
		}
	}

	// ChatGPT: Cleanup
	closesocket(udpSocket);
	WSACleanup();
	return 0;
}

int main()
{
	std::cout << "Server (1) or Client (2): ";
	int opt = 0;

	do
	{
		std::cin >> opt;
	} while (!(opt == 1 || opt == 2));

	if (opt == 1)
	{
		ServerRun();
	}
	else if (opt == 2)
	{
		ClientRun();
	}
}