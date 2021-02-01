/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

#include <string.h>
#include <iomanip>
#include "program_options.h"
#include "util.h"
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include "options.h"

using namespace std;
using namespace primo::program_options;


void setDefaultOptions(Options& opt)
{
	opt.inputFile = getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.m4v";
	opt.outputFile = getExeDir() + "/big_buck_bunny_trailer_iphone";
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: demux_mp4_file -i <input_mp4_file> -o <output_mp4_file_name_without_extension>\n";
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    return !(opt.inputFile.empty() || opt.outputFile.empty());    
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.inputFile;
        cout << " --output " << opt.outputFile;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
            ("help,h", opt.help, "")
            ("input,i", opt.inputFile, string(), "Input mp4 file.")
            ("output,o", opt.outputFile, string(), "Output mp4 file without extension.");
			
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
