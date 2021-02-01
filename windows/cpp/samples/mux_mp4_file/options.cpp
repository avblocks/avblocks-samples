/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "options.h"
#include "util.h"
#include <atlstr.h>
#include "program_options.h"

using namespace std;
using namespace primo::avblocks;	
using namespace primo::codecs;	
using namespace primo::program_options;

void help(OptionsConfig<wchar_t>& optionsConfig)
{
    wcout << endl;
    wcout << L"Usage: mux_mp4_file --audio <input_AAC> --video <input_AVC> --output <output.mp4>" << endl;
    doHelp(wcout, optionsConfig);
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
    opt.input_audio.push_back(getExeDir() + L"\\..\\assets\\aud\\big_buck_bunny_trailer_iphone.aud.mp4");
    opt.input_video.push_back(getExeDir() + L"\\..\\assets\\mov\\big_buck_bunny_trailer_iphone.vid.mp4");
    opt.output_file = getExeDir() + L"\\mux_mp4_file.mp4";
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        for(std::vector<wstring>::iterator input = opt.input_audio.begin(); input != opt.input_audio.end(); input++)
        {
            wcout << L" --audio " << *input;
        }
        for (std::vector<wstring>::iterator input = opt.input_video.begin(); input != opt.input_video.end(); input++)
        {
            wcout << L" --video " << *input;
        }
        wcout << L" --output " << opt.output_file;
        wcout << endl;
        return Parsed;
    }

    OptionsConfig<wchar_t> optionsConfig;
    optionsConfig.addOptions()
        (L"help,?",			opt.help,								L"")
        (L"audio,a",		opt.input_audio,	vector<wstring>(),	L"input AAC files. Can be used multiple times")
        (L"video,v",        opt.input_video,    vector<wstring>(),  L"input H264 files. Can be used multiple times")
        (L"output,o",		opt.output_file,	wstring(),			L"output file");

    try
    {
        scanArgv(optionsConfig, argc, argv);
    }
    catch (ParseFailure<wchar_t> &ex)
    {
        wcout << ex.message() << endl;
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
