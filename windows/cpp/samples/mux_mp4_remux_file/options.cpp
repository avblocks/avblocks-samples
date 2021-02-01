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
    wcout << L"Usage: mux_mp4_remux_file --input file.mp4 --output output.mp4 [--fast-start]" << endl;
    doHelp(wcout, optionsConfig);
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
        opt.input_file = getExeDir() + L"\\..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v";
    
    if(opt.output_file.empty())
        opt.output_file = getExeDir() + L"\\mux_mp4_remux_file.mp4";
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.input_file;
        wcout << L" --output " << opt.output_file;
        wcout << endl;
        return Parsed;
    }

    OptionsConfig<wchar_t> optionsConfig;
    optionsConfig.addOptions()
        (L"help,?",			opt.help,						 L"")
        (L"fast_start,f",	opt.fast_start,					 L"mp4 fast start")
        (L"input,i",		opt.input_file,	    wstring(),	 L"input file (mp4)")
        (L"output,o",		opt.output_file,	wstring(),	 L"output file");

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
