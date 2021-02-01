/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once
#include <string>
#include "util.h"

enum ErrorCodes
{ 
	Parsed = 0,			
	Error,
	Command,
};

struct YesNo
{
    YesNo(bool v): val(v) {}
    bool val;
    operator bool() { return val; }
};

struct Options
{
	Options() : help(false), 
                    reEncodeVideo(true),
                    reEncodeAudio(true)
	{
	}

	bool help;

	std::string inputFile;
	std::string outputFile;

        YesNo reEncodeVideo;
	YesNo reEncodeAudio;
};

inline std::istringstream &operator>>(std::istringstream &in, YesNo &yn)
{
    std::string s;
    in >> s;
    
    if (compareNoCase(s.c_str(), "yes") || compareNoCase(s.c_str(), "y"))
        yn.val = true;
    else if (compareNoCase(s.c_str(), "no") || compareNoCase(s.c_str(), "n"))
        yn.val = false;
    else
        throw std::exception();
    
    return in;
}

inline std::basic_ostream<char> &operator<<(std::basic_ostream<char> &out, YesNo &yn)
{
    if (yn.val)
        out << "yes";
    else
        out << "no";
        
    return out;
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[]);