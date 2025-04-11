#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <WS2tcpip.h>
#pragma warning(disable : 4996)

// Windows API's
#include <iostream>
#include <string>
#include <map>
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
#define TOTAL_BUFFER_SIZE BUFFER_SIZE + 16

namespace SCREEN
{
	const wchar_t CLASS_NAME[] = L"BasicWindow";
	int winX = CW_USEDEFAULT;
	int winY = CW_USEDEFAULT;
	int winWidth = 600;
	int winHeight = 800;
}

struct NetInfo {
	WSADATA wsaData;
	SOCKET socket;
	sockaddr_in addr;
};

struct ClientStorage {
	sockaddr_in addr;
	char* name = nullptr;
};

struct APP_RESULT
{
	int errorCode = 0;
	std::string errorMessage = "";
	std::string userMessage = "";
};

#define ID_EXIT				10001
#define ID_JOIN				10002
#define ID_FIELD_IP			10003
#define ID_FIELD_PORT		10004
#define ID_FIELD_USERNAME	10005