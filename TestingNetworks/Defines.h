#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>
#pragma warning(disable : 4996)

// Windows API's
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <thread>
#include <mutex>

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