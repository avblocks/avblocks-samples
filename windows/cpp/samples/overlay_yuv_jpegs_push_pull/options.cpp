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
	std::wcout << L"Usage: overlay_yuv_jpegs_push_pull --input <file.m4v> --output <file.mp4> --overlay_input <directory> --overlay_count <count>" << std::endl;
	doHelp(std::wcout, optionsConfig);
}

bool validateOptions(Options& opt)
{
    return (!opt.input_file.empty() &&
            !opt.overlay_image.empty() &&
            opt.overlay_count > 0);    
}

void setDefaultOptions(Options& opt)
{
	opt.input_file    = getExeDir() + L"\\..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v";
    opt.output_file   = getExeDir() + L"\\overlay_yuv_jpeg.m4v";
    opt.overlay_image = getExeDir() + L"\\..\\assets\\overlay\\cube";
    opt.overlay_count = 250;
}

ErrorCodes prepareOptions(Options& opt,int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        std::wcout << L"Using defaults:\n";
        std::wcout << L" --input "         << opt.input_file;
        std::wcout << L" --output "        << opt.output_file;
        std::wcout << L" --overlay_input " << opt.overlay_image;
        std::wcout << L" --overlay_count " << opt.overlay_count;
        std::wcout << std::endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",		      opt.help,				        L"")  
		(L"input,i",	      opt.input_file,	 wstring(), L"file; if no input is specified a default input file is used.");
        (L"output,o",         opt.output_file,   wstring(), L"file; if no output file name is specified a default one is used.");
        (L"overlay_input,oi", opt.overlay_image, wstring(), L"directory to overlay files.");
        (L"overlay_count,oc", opt.overlay_count, wstring(), L"number; if no number is specified a default one is used.");

	try
	{
		scanArgv(optionsConfig, argc, argv);
	}
	catch (ParseFailure<wchar_t> &ex)
	{
		std::wcout << ex.message() << std::endl;
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
