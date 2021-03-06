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
    opt.inputFile = getExeDir() + "/../assets/vid/foreman_qcif.h264";
    opt.outputFile = getExeDir() + "/foreman_qcif.yuv";
}

void help(OptionsConfig<char>& optcfg)
{
    cout << "Usage: dec_avc_file -i <h264 file> -o <yuv file>" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    return !(opt.inputFile.empty() || opt.outputFile.empty());
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
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
    ("input, i", opt.inputFile, string(), "H264 input file.")
    ("output, o", opt.outputFile, string(), "YUV output file.");
    
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
