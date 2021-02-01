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
using namespace primo::codecs;

void setDefaultOptions(Options& opt)
{
    opt.inputFile  = getExeDir() + "/../assets/aud/equinox-48KHz.wav";
    opt.outputFile = getExeDir() + "/equinox-48KHz.adts.aac";
}

void help(OptionsConfig<char>& optcfg)
{
    cout << "Usage: enc_aac_adts_pull --input <wav file> --output <aac file>" << endl;
    doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
    if(opt.inputFile.empty())
    {
        cout << "Input file needed" << endl;
        return false;
    }
    else
    {
        cout << "Input file: " << opt.inputFile << endl;
    }
    
    if(opt.outputFile.empty())
    {
        cout << "Output file needed" << endl;
        return false;
    }
    else
    {
        cout << "Output file: " << opt.outputFile << endl;
    }
    
    return true;
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
    (("help,h"),    opt.help,                   (""))
    (("input,i"),   opt.inputFile,  string(),   ("input WAV file"))
    (("output,o"),  opt.outputFile, string(),   ("output AAC file"));
    
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
