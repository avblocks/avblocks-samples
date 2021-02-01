/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */
#include <string>
#include <iostream>
#include <vector>

#include <AVBlocks.h>
#include <PrimoReference++.h>

#include "options.h"
#include "util.h"
#include "program_options.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;
using namespace primo::program_options;

void help(OptionsConfig<char>& optionsConfig)
{
    cout << endl;
    cout << "Usage: demux_mp4_avc_aac_file -i <input_mp4_file> -o <output_file_name_without_extension>\n" << endl;
    doHelp(cout, optionsConfig);
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

void setDefaultOptions(Options& opt)
{
    if(opt.input_file.empty())
        opt.input_file = getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.m4v";
    
    if (opt.output_file.empty())
        opt.output_file = getExeDir() + "/demux_mp4_avc_aac_file";
    
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input "  << opt.input_file;
        cout << " --output " << opt.output_file + ".h264";
        cout << " --output " << opt.output_file + ".aac";
        
        cout << endl;
        return Parsed;
    }
    
    OptionsConfig<char> optionsConfig;
    optionsConfig.addOptions()
    ("help,?",		opt.help,			"")
    ("input,i",		opt.input_file,    string(),    "input file (mp4)")
    ("output,o",		opt.output_file,   string(),    "output file");
    
    try
    {
        scanArgv(optionsConfig, argc, argv);
    }
    catch (ParseFailure<char> &ex)
    {
        cout << ex.message() << endl;
        help(optionsConfig);
        return Error;
    }
    
    if(opt.help)
    {
        help(optionsConfig);
        return Command;
    }
    
    if (!validateOptions(opt))
    {
        help(optionsConfig);
        return Error;
    }
    
    opt.input_file = getExeDir() + "/" + opt.input_file;
    opt.output_file = getExeDir() + "/" + opt.output_file;
    
    return Parsed;
}
