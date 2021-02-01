/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

enum ErrorCodes
{ 
	Parsed = 0,			
	Error,
	Command,
};

struct PresetDescriptor
{
	const char* name;
	const wchar_t * extension;
};

struct Options
{
	std::wstring input_file;
	std::wstring output_file;
	PresetDescriptor preset;
	bool help;
	bool list_presets;
	Options() : help(false), list_presets(), preset() {}
};

extern PresetDescriptor avb_presets[];

void printPresets();
ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);
