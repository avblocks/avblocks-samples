/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
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

struct Options
{
	bool help;
    std::string input_file;
	std::string output_file;
	
	Options(): help(false) {}
};

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[]);