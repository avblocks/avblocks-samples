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
    cout << "Usage: split_mp4_pull_push -i <file.mp4>\n";
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    return (!opt.input_file.empty());    
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        opt.input_file = getExeDir() + "/../assets/mov/avsync_2min.mp4";
        cout << "Using defaults:\n";
        cout << " --input " << opt.input_file;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
            ("help,h", opt.help, "")
            ("input,i", opt.input_file, string(), "file; if no input is specified a default input file is used.");
       
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