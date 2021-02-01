/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#pragma once

inline std::wstring getExeDir()
{
	WCHAR exedir[MAX_PATH];
	GetModuleFileName(NULL, exedir, MAX_PATH);
	PathRemoveFileSpec(exedir);
	return std::wstring(exedir);
}

inline bool compareNoCase(const wchar_t* arg1, const wchar_t* arg2)
{
	return (0 == _wcsicmp(arg1, arg2));
}

inline void deleteFile(const wchar_t* file)
{
	DeleteFile(file);
}

inline void printError(const primo::error::ErrorInfo* e)
{
	using namespace std;

	if (primo::error::ErrorFacility::Success == e->facility())
	{
		wcout << L"Success";
	}
	else
	{
		wcout << L"facility: " << e->facility() << L", error: " << e->code();

		if (e->message())
		{
			wcout << L", " << e->message();
		}

		if (e->hint())
		{
			wcout << L", " << e->hint();
		}
	}

	wcout << endl;
}

