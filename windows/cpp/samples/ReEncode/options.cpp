/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "stdafx.h"
#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::program_options;

void help(OptionsConfig<wchar_t>& optionsConfig)
{
	wcout << L"Usage: ReEncode [--input inputFile.mp4] [--output outputFile.mp4] [--reEncodeAudio yes|no] [--reEncodeVideo yes|no]\n" << endl;
	doHelp(wcout, optionsConfig);
}

bool validateOptions(Options& opt)
{
    return (!opt.inputFile.empty() && 
            !opt.outputFile.empty());    
}

void setDefaultOptions(Options& opt)
{
	//reEncoder audio and video flags are enabled by default
	opt.inputFile = getExeDir() + L"\\..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v";
	opt.outputFile = getExeDir() + L"\\reencode_output.mp4";
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L"--input " << opt.inputFile;
        wcout << L" --output " << opt.outputFile;
        wcout << L" --reEncodeVideo " << opt.reEncodeVideo;
        wcout << L" --reEncodeAudio " << opt.reEncodeAudio;
        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",				opt.help,							L"")
		(L"input,i",			opt.inputFile,		wstring(),		L"input file")
		(L"output,o",			opt.outputFile,		wstring(),		L"output file")
		(L"reEncodeAudio,a",	opt.reEncodeAudio,	YesNo(TRUE),	L"re-encode audio with yes|no")
		(L"reEncodeVideo,v",	opt.reEncodeVideo,	YesNo(TRUE),	L"re-encode video with yes|no");

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