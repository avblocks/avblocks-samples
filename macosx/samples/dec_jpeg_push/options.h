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

struct Options
{
    Options() : help(false)  {}
    
    std::string inputFile;
    std::string outputFile;
    FrameSize frameSize;
    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);