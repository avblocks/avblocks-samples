/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

#include <windows.h>
#include <tchar.h>

class PlayerWindow
{
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	static LPCTSTR s_className;

	virtual void OnResize(int width, int height){}

	HWND _hWnd;
	HINSTANCE _hInstance;

public:
	PlayerWindow(HINSTANCE hInstance, LPCTSTR title, int width, int height);
	~PlayerWindow();

	HWND GetHWnd() const;
};
