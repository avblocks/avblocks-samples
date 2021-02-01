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
#include "program_options.h"

using namespace std;
using namespace primo::program_options;

void help(OptionsConfig<wchar_t> &optionsConfig)
{
	wcout << L"Usage: VideoSpliter --input <file.mp4>" << endl;
	doHelp(wcout, optionsConfig);
}

bool validateOptions(Options& opt)
{
    return (!opt.input_file.empty());    
}

void setDefaultOptions(Options& opt)
{
	opt.input_file = getExeDir() + L"\\..\\assets\\mov\\avsync_2min.mp4";
}

ErrorCodes prepareOptions(Options& opt,int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L"--input " << opt.input_file;
        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",		opt.help,					L"")
		(L"input,i",	opt.input_file,	wstring(),	L"file; if no input is specified a default input file is used.");

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
