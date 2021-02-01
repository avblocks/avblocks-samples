/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
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

inline void deleteFile(const wchar_t* file)
{
	DeleteFile(file);
}

inline void printError(const wchar_t* action, const primo::error::ErrorInfo* e)
{
    if (action)
    {
        std::wcout << action << L": ";
    }

    if (primo::error::ErrorFacility::Success == e->facility())
    {
        std::wcout << L"Success" << std::endl;
        return;
    }

    if (e->message())
    {
        std::wcout << e->message() << L", ";
    }

    std::wcout << L"facility:" << e->facility() << L", error:" << e->code() << std::endl;
}

inline std::vector<uint8_t> readFileBytes(const wchar_t *name)
{
    std::ifstream f(name, std::ios::binary);
    std::vector<uint8_t> bytes;
    if (f)
    {
        f.seekg(0, std::ios::end);
        size_t filesize = f.tellg();
        bytes.resize(filesize);
        f.seekg(0, std::ios::beg);
        f.read((char*)&bytes[0], filesize);
    }

    return bytes;
}
