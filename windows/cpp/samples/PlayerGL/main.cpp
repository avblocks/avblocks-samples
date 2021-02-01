/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "Player.h"
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <Windows.h>
#include <Shlwapi.h>


#define SAMPLE_TITLE L"AVBlocks SDK - PlayerGL sample"


static std::wstring getExeDir()
{
	WCHAR exedir[MAX_PATH];
	GetModuleFileName(NULL, exedir, MAX_PATH);
	PathRemoveFileSpec(exedir);
	return std::wstring(exedir);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	int argc;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	std::wstring inputFile;

	if (argc > 1)
	{
		inputFile = argv[1];
	}
	else
	{
		inputFile = getExeDir();
		inputFile.append(L"\\..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v");
	}

	CoInitialize(NULL);

	primo::avblocks::Library::initialize();

	// set your license string
	// primo::avblocks::Library::setLicense("PRIMO-LICENSE");

	Player player(hInstance, SAMPLE_TITLE);

	if (player.Open(inputFile.c_str()))
	{
		player.EventLoop();
	}
	else
	{
		wchar_t buffer[2048] = { 0 };
		wsprintf(buffer, L"Cannot open the input file: %s\r\n", inputFile.c_str());
		MessageBox(NULL, buffer, SAMPLE_TITLE, MB_OK);
	}

	primo::avblocks::Library::shutdown();

	CoUninitialize();
	
	return 0;
}
