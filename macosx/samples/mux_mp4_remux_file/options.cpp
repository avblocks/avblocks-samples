/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <assert.h>

#include <AVBlocks.h>
#include <PrimoReference++.h>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: mux_mp4_remux_file --input <file.mp4> --output <output.mp4> [--fast-start]";
    cout << endl;
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    if (opt.output_file.empty())
    {
        opt.output_file = getExeDir() + "/mux_mp4_remux_file.mp4";
    }
    
    if (opt.input_file.empty())
    {
        opt.input_file = getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.m4v";
    }
}

bool validateOptions(Options& opt)
{
    if (opt.input_file.empty())
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
        cout << " --input " << opt.input_file;

        cout << " --output " << opt.output_file;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h",      opt.help,       "")
    ("input,i",     opt.input_file,  string(), "input file (mp4)")
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
