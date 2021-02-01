/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "PlayerWindow.h"

LPCTSTR PlayerWindow::s_className = _T("PlayerWindowClass");

PlayerWindow::PlayerWindow(HINSTANCE hInstance, LPCTSTR title, int width, int height)
{
	_hInstance = hInstance;

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)(PlayerWindow::WndProc);
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = _T("PlayerClass");
	RegisterClassEx(&wc);

	RECT windowRect;

	SetRect(&windowRect, 0, 0, width, height);
	AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);

	// Create Window
	_hWnd = CreateWindowEx(0, _T("PlayerClass"), title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
		NULL, NULL, hInstance, this);

	ShowWindow(_hWnd, SW_SHOWNORMAL);
	UpdateWindow(_hWnd);
}

PlayerWindow::~PlayerWindow()
{
	if (NULL != _hWnd)
		DestroyWindow(_hWnd);

	UnregisterClass(s_className, _hInstance);		// UnRegister Window Class
}


HWND PlayerWindow::GetHWnd() const
{
	return _hWnd;
}

LRESULT PlayerWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PlayerWindow* instance = (PlayerWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (message)
	{
	case WM_CREATE:
		{
			CREATESTRUCT* create = (CREATESTRUCT*) lParam;
			instance = (PlayerWindow*) create->lpCreateParams;
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)instance);
			return 0;
		}

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	case WM_DESTROY:
		return 0;

    case WM_SIZE:
		instance->OnResize(LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			return 0;
		}
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
