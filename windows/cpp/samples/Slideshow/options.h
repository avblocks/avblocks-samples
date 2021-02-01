/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
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
	wchar_t * fileExtension;
};

struct Options
{
	Options() : help(false) {}

	PresetDescriptor preset;
	bool help;
};

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[]);

