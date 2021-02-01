/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
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
	bool help;
	bool fast_start;
	std::vector<std::string> input_video;
    std::vector<std::string> input_audio;
	std::string output_file;
	
	Options(): help(false), fast_start(false){}
};

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[]);

