/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

#include <string>

enum ErrorCodes
{ 
	Parsed = 0,			
	Error,
	Command,
};

struct Options
{
    Options(): help(false), overlay_count(0),
               output_file("overlay_yuv_jpegs.mp4") {}
    
    std::string input_file;
    std::string output_file;
    std::string overlay_image;
    int overlay_count;
    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);