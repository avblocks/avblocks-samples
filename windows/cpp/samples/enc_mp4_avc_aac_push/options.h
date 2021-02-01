/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once 

struct Options
{
	std::wstring inputAudioFile;
	std::wstring inputVideoFile;
	std::wstring outputFile;
	bool parseError;
	bool help;
	Options() : inputVideoFile(L""), inputAudioFile(L""), outputFile(L""), parseError(false), help(false){}
};

bool prepareOptions(int argc, wchar_t* argv[], Options& opt);