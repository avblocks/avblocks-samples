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

struct Position
{
    int x;
    int y;
};

struct Options
{
    Options(): help(false) {} 
    
    std::string inputFile;
    std::string overlayImage;
    Position position;
    double alpha;
    std::string outputFile;

    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);