#include "WindowManager.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		HINSTANCE hInstance = cs->hInstance;

		RECT rc;
		GetClientRect(hwnd, &rc);

		int midX = (int)(rc.right * 0.5f);
		int midY = (int)(rc.bottom * 0.5f);

		WinMan::CreateMainMenu(hwnd, msg, wParam, lParam);
		WinMan::CreateServerMenu(hwnd, msg, wParam, lParam);
		WinMan::CreateClientMenu(hwnd, msg, wParam, lParam);

		break;
	}
	case WM_COMMAND:
	{
		HWND top = GetParent(hwnd);
		if (top)
		{
			SendMessage(top, msg, wParam, lParam);
			break;
		}

		if (LOWORD(wParam) == 1)
		{
			SetWindowText(hwnd, L"Server Mode");
			
			// Show Server Boxes
			ShowWindow(clientWindows.messageBox, SW_SHOW);
			ShowWindow(clientWindows.historyBox, SW_SHOW);

			// Hide Main Menu Buttons
			ShowWindow(mainMenuHandles.clientButton, SW_HIDE);
			ShowWindow(mainMenuHandles.serverButton, SW_HIDE);
			ShowWindow(mainMenuHandles.exitButton, SW_HIDE);

		}
		else if (LOWORD(wParam) == 2)
		{
			SetWindowText(hwnd, L"Client Mode");

		}
		else if (LOWORD(wParam) == 3)
		{
			PostQuitMessage(0);
		}

		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rect;
		GetClientRect(hwnd, &rect);
		FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

		// Draw Content after refresh?

		EndPaint(hwnd, &ps);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}
//
//int main()
//{
//	/*std::cout << "Server (1) or Client (2): ";
//	
//	int opt = 0;
//	std::string buf = "";
//
//	do 
//	{ 
//		std::getline(std::cin, buf, '\n');
//		std::cin.clear();
//		
//		opt = (int)buf[0] - '0';
//
//	} while (!(opt == 1 || opt == 2));
//
//	opt == 1 ? NET::ServerRun() : NET::ClientRun();*/
//
//
//
//	// Register the window class.
//	const wchar_t CLASS_NAME[] = L"Sample Window Class";
//
//	WNDCLASS wc = { };
//
//	wc.lpfnWndProc = WindowProc;
//	wc.hInstance = hInstance;
//	wc.lpszClassName = CLASS_NAME;
//
//	HWND hwnd = CreateWindowEx(
//		0,                              // Optional window styles.
//		CLASS_NAME,                     // Window class
//		L"Learn to Program Windows",    // Window text
//		WS_OVERLAPPEDWINDOW,            // Window style
//
//		// Size and position
//		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
//
//		NULL,       // Parent window    
//		NULL,       // Menu
//		hInstance,  // Instance handle
//		NULL        // Additional application data
//	);
//
//	if (hwnd == NULL)
//	{
//		return 0;
//	}
//
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = SCREEN::CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		SCREEN::CLASS_NAME,
		L"UDP App",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		SCREEN::winX,
		SCREEN::winY,
		SCREEN::winWidth,
		SCREEN::winHeight,
		nullptr, nullptr, hInstance, nullptr
	);

	if (!hwnd) return 0;

	ShowWindow(hwnd, nCmdShow);

	// Message loop
	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}