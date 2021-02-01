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

class FrameSize
{
public:
	FrameSize():width(0), height(0){}
	int width;
	int height;
};

struct ColorDescriptor
{
	primo::codecs::ColorFormat::Enum Id;
	const wchar_t * name;
	const wchar_t * description;
};

ColorDescriptor*	getColorByName(const wchar_t* colorName);
ColorDescriptor*	getColorById(primo::codecs::ColorFormat::Enum Id);

struct Options
{
    Options() : fps(0.0), output_color(), help(false), list_colors(false)
    {}

    std::wstring input_dir;
    
    FrameSize frame_size;
    double fps;
    ColorDescriptor output_color;
    std::wstring output_file;

    bool help;
    bool list_colors;
};

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[]);
