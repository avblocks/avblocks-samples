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
    Options(): help(false), split_time(0), split_size(0)
    {}
    
    std::string in_file;
    int split_time; // seconds
    int64_t split_size; // bytes
    bool help;
};

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);