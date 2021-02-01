/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include <string.h>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <string>
#include <iostream>
#include <iomanip>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;	
using namespace primo::codecs;	
using namespace primo::program_options;

void help(OptionsConfig<char>& optionsConfig)
{
	cout << "\nUsage: dec_avc_push --input <directory> [--output <file>]" << endl;
	doHelp(cout, optionsConfig);
}

void setDefaultOptions(Options& opt)
{
    opt.input_dir         = getExeDir() + "/../assets/vid/foreman_qcif.h264.au";
    opt.output_file       = getExeDir() + "/decoded_file.yuv";
}

bool validateOptions(Options& opt)
{  
    return !opt.input_dir.empty() && !opt.output_file.empty();       
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input "   << opt.input_dir;
        cout << " --output "  << opt.output_file;
        cout << endl;
        return Parsed;
    }

    OptionsConfig<char> optionsConfig;
    optionsConfig.addOptions()
        ("help,?",     opt.help,                           "")
        ("input,i",    opt.input_dir,   string(),          "input directory (contains sequence of compressed files)")
        ("output,o",   opt.output_file, string(),          "output YUV file");

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

    return Parsed;
}







