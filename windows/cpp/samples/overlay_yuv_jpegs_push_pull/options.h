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

struct Options
{
    Options(): help(false), overlay_count(0),
               output_file(L"overlay_yuv_jpegs.mp4") {}
    
    std::wstring input_file;
    std::wstring output_file;
    std::wstring overlay_image;
    int overlay_count;
    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[]);