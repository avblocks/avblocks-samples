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
#include "program_options.h"

using namespace std;
using namespace primo::avblocks;	
using namespace primo::codecs;	
using namespace primo::program_options;

void help(OptionsConfig<wchar_t>& optionsConfig)
{
	wcout << L"\nUsage: dec_avc_push --input <directory> [--output <file>]" << endl;
	doHelp(wcout, optionsConfig);
}

void setDefaultOptions(Options& opt)
{
    opt.input_dir         = getExeDir() + L"\\..\\assets\\vid\\foreman_qcif.h264.au";
    opt.output_file       = getExeDir() + L"\\decoded_file.yuv";
}

bool validateOptions(Options& opt)
{
    return !opt.input_dir.empty() && !opt.output_file.empty();
}


ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input "   << opt.input_dir;
        wcout << L" --output "  << opt.output_file;
        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
    optionsConfig.addOptions()
        (L"help,?", opt.help, L"")
        (L"input,i", opt.input_dir, wstring(), L"input directory (contains sequence of compressed files)")
        (L"output,o", opt.output_file, wstring(), L"output YUV file");

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







