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

void help(primo::program_options::OptionsConfig<char>& optionsConfig)
{
    cout << endl;
    cout << "Usage: mux_mp4_file --audio <input_AAC> --video <input_AVC> --output <output.mp4>" << endl;
    primo::program_options::doHelp(cout, optionsConfig);
}

bool validateOptions(Options& opt)
{    
    if (opt.input_audio.empty() && opt.input_video.empty())
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
    opt.input_audio.push_back(getExeDir() + "/../assets/aud/big_buck_bunny_trailer_iphone.aud.mp4");
    opt.input_video.push_back(getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.vid.mp4");
    opt.output_file = getExeDir() + "/mux_mp4_file.mp4";
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        for(std::vector<string>::iterator input = opt.input_audio.begin(); input != opt.input_audio.end(); input++)
        {
            cout << " --audio " << *input;
        }
        for (std::vector<string>::iterator input = opt.input_video.begin(); input != opt.input_video.end(); input++)
        {
            cout << " --video " << *input;
        }
        cout << " --output " << opt.output_file;
        cout << endl;
        return Parsed;
    }

    primo::program_options::OptionsConfig<char> optionsConfig;
    optionsConfig.addOptions()
        ("help,?",			opt.help,								"")
        ("audio,a",         opt.input_audio,	vector<string>(),	"input AAC files. Can be used multiple times")
        ("video,v",         opt.input_video,    vector<string>(),   "input H264 files. Can be used multiple times")
        ("output,o",		opt.output_file,	string(),			"output file");

    try
    {
        primo::program_options::scanArgv(optionsConfig, argc, argv);
    }
    catch (primo::program_options::ParseFailure<char> &ex)
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

