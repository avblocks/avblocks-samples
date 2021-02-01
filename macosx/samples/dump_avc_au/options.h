/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

#include <string>

enum OptionResult
{
	NotParsed = 0,
	Parsed,			
	Error,
	Command,
};

struct Options
{
    Options() : help(false)
    {}

    std::string input_file;
    std::string output_dir;
    bool help;
};

OptionResult prepareOptions(Options &opt, int argc, char* argv[]);
