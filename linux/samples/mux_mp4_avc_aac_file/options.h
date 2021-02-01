/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#pragma once

#include <vector>
#include <string>

enum ErrorCodes
{ 
	Parsed = 0,			
	Error,
	Command,
};

struct Options
{
	Options(): help(false), fast_start(false)
	{
	}

	std::string output_file;
	std::vector<std::string> input_files;

	bool help;
	bool fast_start;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);