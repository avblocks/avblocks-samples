/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
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
	wcout << L"Usage: mux_mp4_avc_aac_file --input file1.aac --input file2.h264 --output output.mp4 [--fast-start]" << endl;
	doHelp(wcout, optionsConfig);
}

inline std::wistringstream &operator>>(std::wistringstream &in, vector<wstring> &arr)
{
	std::wstring next;
	in >> next;
	arr.push_back(next);
	return in;
}

bool validateOptions(Options& opt)
{    
    if (opt.input_files.empty())
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
	if(opt.input_files.empty())
		opt.input_files.push_back(getExeDir() + L"\\..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v");
	
	if(opt.output_file.empty())
		opt.output_file = getExeDir() + L"\\mux_mp4_avc_aac_file.mp4";
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        for(std::vector<wstring>::iterator input = opt.input_files.begin(); input != opt.input_files.end(); input++)
        {
            wcout << L" --input " << *input;
        }
        wcout << L" --output " << opt.output_file;
        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",			opt.help,								L"")
		(L"fast_start,f",	opt.fast_start,							L"mp4 fast start")
		(L"input,i",		opt.input_files,	vector<wstring>(),	L"input file (aac, h264, mp4)")
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
