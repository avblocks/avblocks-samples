/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */
#include <iostream>
#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: read_av_info_any_file -i <avfile>\n";
    primo::program_options::doHelp(cout, optcfg);
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        // default input file
        opt.avfile = getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.m4v";
        cout << "Using default input file.\n";
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h", opt.help, "")
    ("input,i", opt.avfile, string(), "file; if no input is specified a default input file is used.");
    
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
    
    return Parsed;
}