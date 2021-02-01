/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
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

/*
command line options:

long format:
	dump_avc_au --input <h264-file> --output <folder>

short format:
	dump_avc_au -i <h264-file> -o <folder>

*/

void help(OptionsConfig<wchar_t>& optionsConfig)
{
	wcout << L"dump_avc_au --input <h264-file> --output <folder>" << endl;
	doHelp(wcout, optionsConfig);
}

void setDefaultOptions(Options& opt)
{
	opt.input_file = getExeDir() + L"\\..\\assets\\vid\\foreman_qcif.h264";
	opt.output_dir = getExeDir() + L"\\foreman_qcif.h264.au";
}

bool validateOptions(Options& opt)
{
	return !opt.input_file.empty() && !opt.output_dir.empty();
}

OptionResult prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.input_file;
		wcout << L" --output " << opt.output_dir;
        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?", opt.help, L"")
		(L"input,i", opt.input_file, wstring(), L"input file (AVC/H.264)")
		(L"output,o", opt.output_dir, wstring(), L"output directory");

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







