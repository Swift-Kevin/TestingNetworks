#pragma once
#include "NetCode.h"

enum class CurrentWindow { MainMenu, ClientJoin, ClientError, Client, Server } currWind;

// Main Menu Unique HWNDs
HWND mainMenu;
struct MainMenuHandles {
	HWND appLabel;
	HWND clientButton;
	HWND serverButton;
	HWND exitButton;
} mainMenuHandles;

// Client Window Unique HWNDs
HWND clientMenu;
struct ClientWindowHandles {
	HWND ipLabel;
	HWND ipField;
	HWND portLabel;
	HWND portField;
	HWND usernameLabel;
	HWND usernameField;
	HWND joinButton;
	HWND joinLabel;
	HWND exitButton;
	HWND historyBox;
	HWND messageLabel;
	HWND messageField;
} clientWindows;

// Server Window Unique HWNDs
HWND serverMenu;
struct ServerWindowHandles {
	HWND controlBox;
	HWND exitButton;
	HWND historyBox;
} serverWindows;

namespace MyWinProcs
{
	void ToggleMainMenu(byte status)
	{
		byte toggle = status ? SW_SHOW : SW_HIDE;

		ShowWindow(mainMenu, toggle);
		ShowWindow(mainMenuHandles.clientButton, toggle);
		ShowWindow(mainMenuHandles.serverButton, toggle);
		ShowWindow(mainMenuHandles.exitButton, toggle);
	}

	void ToggleServerMenu(byte status)
	{
		byte toggle = status ? SW_SHOW : SW_HIDE;

		ShowWindow(serverMenu, toggle);
		ShowWindow(serverWindows.exitButton, toggle);
		ShowWindow(serverWindows.historyBox, toggle);
		ShowWindow(serverWindows.controlBox, toggle);
	}

	void ToggleClientJoinMenu(byte status)
	{
		byte toggle = status ? SW_SHOW : SW_HIDE;

		ShowWindow(clientMenu, toggle);
		ShowWindow(clientWindows.exitButton, toggle);
		ShowWindow(clientWindows.ipField, toggle);
		ShowWindow(clientWindows.portField, toggle);
		ShowWindow(clientWindows.usernameField, toggle);
		ShowWindow(clientWindows.joinButton, toggle);

		ShowWindow(clientWindows.ipLabel, toggle);
		ShowWindow(clientWindows.portLabel, toggle);
		ShowWindow(clientWindows.usernameLabel, toggle);
		ShowWindow(clientWindows.joinLabel, toggle);
	}

	void ToggleClientMenu(byte status)
	{
		byte toggle = status ? SW_SHOW : SW_HIDE;

		ToggleClientJoinMenu(status ? SW_HIDE : SW_SHOW);

		ShowWindow(clientMenu, toggle);
		ShowWindow(clientWindows.exitButton, toggle);
		ShowWindow(clientWindows.historyBox, toggle);
		ShowWindow(clientWindows.messageField, toggle);
		ShowWindow(clientWindows.messageLabel, toggle);
	}

	LRESULT CALLBACK MainMenuProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_COMMAND:
		{
			HWND top = GetParent(hwnd);
			if (top)
			{
				SendMessage(top, msg, wParam, lParam);
			}

			if (LOWORD(wParam) == 1) // Go to Server
			{
				SetWindowText(top, L"Server Mode");
				ToggleMainMenu(SW_HIDE);
				ToggleServerMenu(SW_SHOW);
			}
			else if (LOWORD(wParam) == 2) // Go to Client Join
			{
				SetWindowText(top, L"Client Mode");
				ToggleMainMenu(SW_HIDE);
				ToggleClientJoinMenu(SW_SHOW);
			}
			else if (LOWORD(wParam) == ID_EXIT) // Exit App
			{
				PostQuitMessage(0);
			}

			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // match gray
			SetBkMode(hdcStatic, TRANSPARENT);
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
		}
		default:
			break;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK ClientMenuProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_COMMAND:
		{
			HWND top = GetParent(hwnd);
			if (top)
			{
				SendMessage(top, msg, wParam, lParam);
			}

			if (LOWORD(wParam) == ID_EXIT) // Go to Main Menu
			{
				SetWindowText(top, L"UDP App");
				ToggleClientMenu(SW_HIDE);
				ToggleClientJoinMenu(SW_HIDE);
				ToggleMainMenu(SW_SHOW);
			}
			else if (LOWORD(wParam) == ID_JOIN) // Go to Client Menu
			{
				// Check all 3 fields
				wchar_t ip[100], port[100], user[100];
				GetWindowText(clientWindows.ipField, ip, 100);
				GetWindowText(clientWindows.portField, port, 100);
				GetWindowText(clientWindows.usernameField, user, 100);
				bool ipFilled = wcslen(ip) > 0;
				bool portFilled = wcslen(port) > 0;
				bool userFilled = wcslen(user) > 0;

				if (ipFilled && portFilled && userFilled)
				{
					ToggleClientMenu(SW_SHOW);
				}
				else
				{
					SetWindowText(clientWindows.joinLabel, L"Ensure All Fields are filled in");
				}
			}
			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // match gray
			SetBkMode(hdcStatic, TRANSPARENT);

			//byte r = rand() % 120 + 120;
			//byte g = rand() % 120 + 120;
			//byte b = rand() % 120 + 120;

			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
		}
		default:
			break;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK ServerMenuProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_CREATE:
		{
			NET::ServerRun();
			break;
		}
		case WM_COMMAND:
		{
			HWND top = GetParent(hwnd);
			if (top)
			{
				SendMessage(top, msg, wParam, lParam);
			}

			if (LOWORD(wParam) == ID_EXIT) // Go to Main Menu
			{
				SetWindowText(top, L"UDP App");
				ToggleServerMenu(SW_HIDE);
				ToggleMainMenu(SW_SHOW);
			}

			break;
		}
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetBkColor(hdcStatic, RGB(255, 255, 255)); // match gray
			SetBkMode(hdcStatic, TRANSPARENT);
			return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
		}
		default:
			break;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

namespace WinMan
{
	struct ReusedData
	{
		HWND hwnd = {};
		UINT msg = {};
		WPARAM wParam = {};
		LPARAM lParam = {};
		RECT rc = {};
		HINSTANCE hInstance = {};

		int midX = 0;
		int midY = 0;
	};

	ReusedData CreateReusedData(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		ReusedData retVal;
		retVal.hwnd = hwnd;
		retVal.lParam = lParam;
		retVal.msg = msg;
		retVal.wParam = wParam;

		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		HINSTANCE hInstance = cs->hInstance;
		retVal.hInstance = hInstance;

		RECT rc;
		GetClientRect(hwnd, &rc);
		retVal.rc = rc;

		int midX = (int)(rc.right * 0.5f);
		int midY = (int)(rc.bottom * 0.5f);
		retVal.midX = midX;
		retVal.midY = midY;

		return retVal;
	}

	void CreateMainMenu(ReusedData& data)
	{
		mainMenu = CreateWindowEx(
			0, L"STATIC", L"",
			WS_CHILD | WS_VISIBLE,
			0, 0, SCREEN::winWidth, SCREEN::winHeight,
			data.hwnd, nullptr, data.hInstance, nullptr
		);

		mainMenuHandles.serverButton = CreateWindowEx(
			0, L"BUTTON", L"Server",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			data.midX - 50, data.midY, 100, 50,
			mainMenu, (HMENU)1, data.hInstance, nullptr
		);

		mainMenuHandles.clientButton = CreateWindowEx(
			0, L"BUTTON", L"Client",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			data.midX - 50, data.midY + 75, 100, 50,
			mainMenu, (HMENU)2, data.hInstance, nullptr
		);

		mainMenuHandles.exitButton = CreateWindowEx(
			0, L"BUTTON", L"Exit",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			500, 0, 100, 100,
			mainMenu, (HMENU)ID_EXIT, data.hInstance, nullptr
		);

		mainMenuHandles.appLabel = CreateWindowEx(
			0, L"STATIC", L"",
			WS_CHILD | WS_VISIBLE | SS_CENTER,
			0, 0, 500, 100,
			data.hwnd, nullptr, data.hInstance, nullptr
		);

		SetWindowLongPtr(mainMenu, GWLP_WNDPROC, (LONG_PTR)MyWinProcs::MainMenuProc);

		// Update app label
		SetWindowText(mainMenuHandles.appLabel, L"UDP Chat Application by Kevin Wall");

		// why the absolute !@#& does a function take that many freaking parameters
		// ALL FOR THE FONT GAWDANGIT
		HFONT hFont = CreateFont(
			24,                        // height
			0, 0, 0,                   // width, escapement, orientation
			FW_BOLD,                  // weight
			FALSE, FALSE, FALSE,       // italic, underline, strikeout
			DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI")
		);

		SendMessage(mainMenuHandles.appLabel, WM_SETFONT, (WPARAM)hFont, TRUE);
	}

	void CreateClientMenu(ReusedData& data)
	{
		clientMenu = CreateWindowEx(
			0, L"STATIC", L"",
			WS_CHILD | WS_VISIBLE,
			0, 0, SCREEN::winWidth, SCREEN::winHeight,
			data.hwnd, nullptr, data.hInstance, nullptr
		);

		// Exit
		{
			clientWindows.exitButton = CreateWindowEx(
				0, L"BUTTON", L"Exit",
				WS_CHILD | BS_PUSHBUTTON,
				500, 0, 100, 100,
				clientMenu, (HMENU)ID_EXIT, data.hInstance, nullptr
			);
		}

		// IP
		{
			clientWindows.ipField = CreateWindowEx(
				WS_EX_CLIENTEDGE, L"EDIT", L"",
				WS_CHILD | ES_AUTOHSCROLL ,
				data.midX - 150,
				data.midY - 250,
				300,
				100,
				clientMenu, (HMENU)ID_FIELD_IP, data.hInstance, nullptr
			);

			clientWindows.ipLabel = CreateWindowEx(
				0, L"STATIC", L"IP: ",
				WS_CHILD | WS_VISIBLE | SS_CENTER,
				0,
				data.midY - 265,
				SCREEN::winWidth,
				15,
				clientMenu, nullptr, data.hInstance, nullptr
			);
		}

		// Port
		{
			clientWindows.portField = CreateWindowEx(
				WS_EX_CLIENTEDGE, L"EDIT", L"",
				WS_CHILD | ES_AUTOHSCROLL,
				data.midX - 150,
				data.midY - 100,
				300,
				100,
				clientMenu, (HMENU)ID_FIELD_PORT, data.hInstance, nullptr
			);

			SetWindowText(clientWindows.portField, L"31337");

			clientWindows.portLabel = CreateWindowEx(
				0, L"STATIC", L"Port: ",
				WS_CHILD | WS_VISIBLE | SS_CENTER,
				0,
				data.midY - 115,
				SCREEN::winWidth,
				15,
				clientMenu, nullptr, data.hInstance, nullptr
			);
		}

		// Username
		{
			clientWindows.usernameField = CreateWindowEx(
				WS_EX_CLIENTEDGE, L"EDIT", L"",
				WS_CHILD | ES_AUTOHSCROLL,
				data.midX - 150,
				data.midY + 50,
				300,
				100,
				clientMenu, (HMENU)ID_FIELD_USERNAME, data.hInstance, nullptr
			);

			clientWindows.usernameLabel = CreateWindowEx(
				0, L"STATIC", L"Username: ",
				WS_CHILD | WS_VISIBLE | SS_CENTER,
				0,
				data.midY + 35,
				SCREEN::winWidth,
				15,
				clientMenu, nullptr, data.hInstance, nullptr
			);
		}

		// Join
		{
			clientWindows.joinButton = CreateWindowEx(
				0, L"BUTTON", L"Join",
				WS_CHILD | BS_PUSHBUTTON,
				data.midX - 50,
				data.midY + 200,
				100,
				100,
				clientMenu, (HMENU)ID_JOIN, data.hInstance, nullptr
			);

			clientWindows.joinLabel = CreateWindowEx(
				0, L"STATIC", L"",
				WS_CHILD | WS_VISIBLE | SS_CENTER,
				0,
				data.midY + 185,
				SCREEN::winWidth,
				15,
				clientMenu, nullptr, data.hInstance, nullptr
			);
		}

		// History
		{
			clientWindows.historyBox = CreateWindowEx(
				WS_EX_CLIENTEDGE, L"EDIT", L"",
				WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
				0,
				100,
				SCREEN::winWidth,
				SCREEN::winHeight - 230,
				clientMenu, (HMENU)6, data.hInstance, nullptr
			);
		}

		// Message
		{
			clientWindows.messageField = CreateWindowEx(
				WS_EX_CLIENTEDGE, L"EDIT", L"",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
				0,
				SCREEN::winHeight - 120,
				SCREEN::winWidth,
				100,
				clientMenu, (HMENU)7, data.hInstance, nullptr
			);

			clientWindows.messageLabel = CreateWindowEx(
				0, L"STATIC", L"Message (max characters 512): ",
				WS_CHILD | WS_VISIBLE,
				0,
				SCREEN::winHeight - 135,
				SCREEN::winWidth,
				15,
				clientMenu, nullptr, data.hInstance, nullptr
			);
		}

		SetWindowLongPtr(clientMenu, GWLP_WNDPROC, (LONG_PTR)MyWinProcs::ClientMenuProc);

		ShowWindow(clientWindows.messageField, SW_HIDE);
		ShowWindow(clientWindows.historyBox, SW_HIDE);
		ShowWindow(clientWindows.exitButton, SW_HIDE);
		ShowWindow(clientWindows.ipField, SW_HIDE);
		ShowWindow(clientWindows.portField, SW_HIDE);
		ShowWindow(clientWindows.usernameField, SW_HIDE);
		ShowWindow(clientWindows.joinButton, SW_HIDE);

		ShowWindow(clientWindows.ipLabel, SW_HIDE);
		ShowWindow(clientWindows.portLabel, SW_HIDE);
		ShowWindow(clientWindows.usernameLabel, SW_HIDE);
		ShowWindow(clientWindows.messageLabel, SW_HIDE);
		ShowWindow(clientWindows.joinLabel, SW_HIDE);
	}

	void CreateServerMenu(ReusedData& data)
	{
		serverMenu = CreateWindowEx(
			0, L"STATIC", L"",
			WS_CHILD,
			0, 0, SCREEN::winWidth, SCREEN::winHeight,
			data.hwnd, nullptr, data.hInstance, nullptr
		);

		serverWindows.historyBox = CreateWindowEx(
			WS_EX_CLIENTEDGE, L"EDIT", L"",
			WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
			0,
			100,
			SCREEN::winWidth,
			SCREEN::winHeight - 200,
			serverMenu, (HMENU)5, data.hInstance, nullptr
		);

		serverWindows.controlBox = CreateWindowEx(
			0, L"STATIC", L"",
			WS_CHILD,
			0, 100, SCREEN::winWidth, 100,
			serverMenu, (HMENU)2, data.hInstance, nullptr
		);

		serverWindows.exitButton = CreateWindowEx(
			0, L"BUTTON", L"Exit",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			500, 0, 100, 100,
			serverMenu, (HMENU)ID_EXIT, data.hInstance, nullptr
		);

		SetWindowLongPtr(serverMenu, GWLP_WNDPROC, (LONG_PTR)MyWinProcs::ServerMenuProc);

		ShowWindow(serverWindows.historyBox, SW_HIDE);
		ShowWindow(serverWindows.exitButton, SW_HIDE);
		ShowWindow(serverWindows.controlBox, SW_HIDE);
	}

	void CreateAppHandles(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		ReusedData reusedData = CreateReusedData(hwnd, msg, wParam, lParam);

		CreateMainMenu(reusedData);
		CreateClientMenu(reusedData);
		CreateServerMenu(reusedData);

		ShowWindow(mainMenu, SW_SHOW);
		ShowWindow(serverMenu, SW_HIDE);
		ShowWindow(clientMenu, SW_HIDE);
	}
}