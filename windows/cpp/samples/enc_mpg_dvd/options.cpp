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
using namespace primo::codecs;


void help(OptionsConfig<wchar_t>& optcfg)
{
	wcout << "enc_mpg_dvd --input <avfile> [--split-time <seconds>] [--split-size <MBs>]" << endl;
	doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
	if(opt.in_file.empty())
	{
		wcout << L"input file needed" << endl;
		return false;
	}
	else
	{
		wcout << L"input file: " << opt.in_file << endl;
	}

	if(opt.split_size > 0)
	{
		wcout << L"split size (MB): " << opt.split_size << endl;
		opt.split_size *= 1000*1000;
	}
	
	if(opt.split_time > 0)
	{
		wcout << L"split time (seconds): " << opt.split_time << endl;
	}

	return true;
}

void setDefaultOptions(Options& opt)
{
	opt.help = false;
	opt.in_file = getExeDir() + L"\\..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v";
	opt.split_size = 3000000; // in bytes
	opt.split_time = 0;
}

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
	{
		setDefaultOptions(opt);
		wcout << L"Using default options:\n";
		wcout << L" --input " << opt.in_file;
		wcout << L" --split-size " << opt.split_size / 1000000;
		wcout << endl;
		return Parsed;
	}

    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
            (L"help,h",			opt.help,						L"")
			(L"input,i",		opt.in_file, wstring(),			L"input video file")
			(L"split-time,t",	opt.split_time, 0,				L"split time (seconds)")
			(L"split-size,s",	opt.split_size, INT64_C(0),		L"split size (MB)");

    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<wchar_t> &ex)
    {
        wcout << ex.message() << endl;
        help(optcfg);
        return Error;
    }
    
    if (opt.help)
    {
        help(optcfg);
        return Command;
    }

    if (!validateOptions(opt))
    {
        help(optcfg);
        return Error;
    }

	return Parsed;
}
