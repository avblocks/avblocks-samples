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

struct ColorDescriptor
{
    primo::codecs::ColorFormat::Enum Id;
    const char* name;
    const char* description;
};

struct StreamTypeDescriptor
{
    primo::codecs::StreamType::Enum Id;
    const char* name;
    const char* extension;
};

struct FrameSize
{
    int width;
    int height;
};

struct Options
{
    Options() :
    input_stream_type(), yuv_frame(), yuv_fps(-1.0), yuv_color(),
    help(false), list_colors(false), list_stream_types(false)
    {
    }
    
    // input options
    StreamTypeDescriptor input_stream_type;
    std::string input_file;
    
    // output options
    FrameSize yuv_frame;
    double	yuv_fps;
    ColorDescriptor yuv_color;
    std::string output_file;
    
    bool help;
    bool list_colors;
    bool list_stream_types;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);