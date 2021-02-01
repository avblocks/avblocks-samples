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

    std::wstring input_file;
    std::wstring output_dir;
    bool help;
};

OptionResult prepareOptions(Options &opt, int argc, wchar_t* argv[]);
