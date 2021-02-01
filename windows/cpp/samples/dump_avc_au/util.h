/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

// Windows specific

inline std::wstring getExeDir()
{
	WCHAR exedir[MAX_PATH];
	GetModuleFileName(NULL, exedir, MAX_PATH);
	PathRemoveFileSpec(exedir);
	return std::wstring(exedir);
}

inline bool makeDir(std::wstring dir)
{
	if (CreateDirectory(dir.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
		return true;

	return false;
}

inline void printError(const wchar_t* action, const primo::error::ErrorInfo* e)
{
	using namespace std;

	if (action)
	{
		wcout << action << L": ";
	}

	if (primo::error::ErrorFacility::Success == e->facility())
	{
		wcout << L"Success" << endl;
		return;
	}

	if (e->message())
	{
		wcout << e->message() << L", ";
	}

	wcout << L"facility:" << e->facility()
		  << L", error:" << e->code()
		  << L", hint:" << e->hint()
		  << endl;
}