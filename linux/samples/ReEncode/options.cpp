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
    cout << "Usage: ReEncode --input inputFile.mp4 --output outputFile.mp4 [--reEncodeAudio yes|no] [--reEncodeVideo yes|no]\n" << endl;
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.m4v";
    opt.outputFile = getExeDir() + "/reencode_output.mp4";
}

bool validateOptions(Options& opt)
{
    return (!opt.inputFile.empty() && 
            !opt.outputFile.empty());    
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.inputFile;
        cout << " --output " << opt.outputFile;
        cout << " --reEncodeVideo " << opt.reEncodeVideo;
        cout << " --reEncodeAudio " << opt.reEncodeAudio;
        
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
            ("help,h", opt.help, "")
            ("input,i", opt.inputFile, string(), "input mp4 file")
            ("output,o", opt.outputFile, string(), "output mp4 file")
            ("reEncodeAudio,a", opt.reEncodeAudio, YesNo(true), "re-encode audio, yes|no")
            ("reEncodeVideo,v", opt.reEncodeVideo, YesNo(true), "re-encode video, yes|no");
       
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
