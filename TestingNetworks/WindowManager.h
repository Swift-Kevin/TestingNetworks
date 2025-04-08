#pragma once
#include "NetCode.h"

// Main Info
HWND mainMenu;
struct MainMenuHandles
{
	HWND clientButton;
	HWND serverButton;
	HWND exitButton;
	HWND appLabel;
} mainMenuHandles;

// Client Info
HWND clientMenu;
struct ClientWindowHandles {
	HWND historyBox;
	HWND messageBox;
} clientWindows;

// Server Info
HWND serverMenu;
struct ServerWindowHandles {
	HWND historyBox;
	HWND messageBox;
} serverWindows;

namespace WinMan
{
	void CreateMainMenu(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		HINSTANCE hInstance = cs->hInstance;

		RECT rc;
		GetClientRect(hwnd, &rc);

		int midX = rc.right * 0.5f;
		int midY = rc.bottom * 0.5f;

		mainMenu = CreateWindowEx(
			0, L"STATIC", L"MainMenu",
			WS_CHILD | WS_VISIBLE,
			0, 0, SCREEN::winWidth, SCREEN::winHeight,
			hwnd, nullptr, hInstance, nullptr
		);

		mainMenuHandles.appLabel = CreateWindowEx(
			0, L"STATIC", L"UDP Application",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			midX - 50, 10, 100, 50,
			mainMenu, nullptr, hInstance, nullptr
		);

		mainMenuHandles.serverButton = CreateWindowEx(
			0, L"BUTTON", L"Server",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			midX - 50, midY, 100, 50,
			mainMenu, (HMENU)1, hInstance, nullptr
		);

		mainMenuHandles.clientButton = CreateWindowEx(
			0, L"BUTTON", L"Client",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			midX - 50, midY + 75, 100, 50,
			mainMenu, (HMENU)2, hInstance, nullptr
		);

		mainMenuHandles.exitButton = CreateWindowEx(
			0, L"BUTTON", L"Exit",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			midX - 50, midY + 150, 100, 50,
			mainMenu, (HMENU)3, hInstance, nullptr
		);
	}

	void CreateServerMenu(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		HINSTANCE hInstance = cs->hInstance;

		RECT rc;
		GetClientRect(hwnd, &rc);

		int midX = rc.right * 0.5f;
		int midY = rc.bottom * 0.5f;

		clientWindows.historyBox = CreateWindowEx(
			0, L"STATIC", L"History",
			WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
			50,
			50,
			100,
			100,
			hwnd, (HMENU)4, hInstance, nullptr
		);

		clientWindows.messageBox = CreateWindowEx(
			WS_EX_CLIENTEDGE, L"EDIT", L"Message",
			WS_CHILD | ES_AUTOHSCROLL,
			50,
			200,
			SCREEN::winWidth - 100,
			SCREEN::winHeight - 250,
			hwnd, (HMENU)5, hInstance, nullptr
		);
	}

	void CreateClientMenu(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		HINSTANCE hInstance = cs->hInstance;

		RECT rc;
		GetClientRect(hwnd, &rc);

		int midX = rc.right * 0.5f;
		int midY = rc.bottom * 0.5f;
	}
	

	void ClientButtonHit()
	{

	}

}