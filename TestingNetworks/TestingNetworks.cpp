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

		WinMan::CreateAppHandles(hwnd, msg, wParam, lParam);

		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		RECT rect;
		GetClientRect(hwnd, &rect);
		FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));

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