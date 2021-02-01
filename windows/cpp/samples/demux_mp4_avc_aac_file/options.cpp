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
	wcout << L"Usage: demux_mp4_avc_aac_file -i <input_mp4_file> -o <output_file_name_without_extension>\n" << endl;
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
	
    if (opt.output_file.empty())
        opt.output_file = getExeDir() + L"\\demux_mp4_avc_aac_file";
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.input_file;
        wcout << L" --output " << opt.output_file + L".h264";
        wcout << L" --output " << opt.output_file + L".aac";

        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",			opt.help,						 L"")
		(L"input,i",		opt.input_file,    wstring(),    L"input file (mp4)")
		(L"output,o",		opt.output_file,   wstring(),    L"output file");

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

    opt.input_file = getExeDir() + L"\\" + opt.input_file;
    opt.output_file = getExeDir() + L"\\" + opt.output_file;

	return Parsed;
}
