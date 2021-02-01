/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

// memmove in AVBlocks
#include <string.h>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <assert.h>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;	
using namespace primo::codecs;	

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: mux_mp4_avc_aac_file --input <file1> [--input <file2>] --output <output.mp4> [--fast-start]";
    cout << endl;
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    if (opt.output_file.empty())
    {
        opt.output_file = "mux_mp4_avc_aac_file.mp4";
    }

    if (opt.input_files.empty())
    {
        string input( getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.m4v");
        opt.input_files.push_back(input);
    }
}

bool validateOptions(Options& opt)
{    
    if (opt.input_files.empty())
    {
        return false;
    }

    if (opt.output_file.empty())
    {
        return false;
    }
    return true;
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        for (const auto input: opt.input_files)
        {
            cout << " --input " << input;
        }
        cout << " --output " << opt.output_file;
        cout << endl;
        return Parsed;
    }
 
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
            ("help,h",      opt.help,       "")
            ("input,i",     opt.input_files, std::vector<string>(), "input file (aac, h264, mp4)")
            ("output,o",    opt.output_file, string(), "output file")
            ("fast-start,f",opt.fast_start, "mp4 fast start");
    
    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<char> &ex)
    {
        cout << ex.message() << endl;
        help(optcfg);
        return Error;
    }
    
    if (opt.help)
    {
        help(optcfg);
        return Command;
    }

    if (!validateOptions(opt))
    {
        help(optcfg);
        return Error;
    }
    
    return Parsed;
}

std::istringstream &operator>>(std::istringstream &in, std::vector<string>& v)
{
	std::string arg;
	in >> arg;
        v.push_back(arg);
	return in;
}

