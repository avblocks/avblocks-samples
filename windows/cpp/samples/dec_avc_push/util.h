/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

// Windows specific

inline bool compareNoCase(const wchar_t* arg1, const wchar_t* arg2)
{
	return (0 == _wcsicmp(arg1, arg2));
}

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