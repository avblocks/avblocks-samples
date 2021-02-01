/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include <cassert>
#include <fstream>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::program_options;
using namespace primo::codecs;

void help(OptionsConfig<char>& optcfg)
{
	cout << "enc_mpg_dvd --input <avfile> [--split-time <seconds>] [--split-size <MBs>]" << endl;
	doHelp(cout, optcfg);
}

bool validateOptions(Options& opt)
{
	if(opt.in_file.empty())
	{
		cout << "input file needed" << endl;
		return false;
	}
	else
	{
		cout << "input file: " << opt.in_file << endl;
	}

	if(opt.split_size > 0)
	{
		cout << "split size (MB): " << opt.split_size << endl;
		opt.split_size *= 1000*1000;
	}
	
	if(opt.split_time > 0)
	{
		cout << "split time (seconds): " << opt.split_time << endl;
	}

	return true;
}

void setDefaultOptions(Options& opt)
{
	opt.help = false;
	opt.in_file = getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.m4v";
	opt.split_size = 3000000; // in bytes
	opt.split_time = 0;
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using default options:\n";
        cout << " --input " << opt.in_file;
        cout << " --split-size " << opt.split_size / 1000000;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
            ("help,h",		opt.help,			"")
            ("input,i",	opt.in_file, string(),		"input video file")
            ("split-time,t",	opt.split_time, 0,		"split time (seconds)")
            ("split-size,s",	opt.split_size, INT64_C(0),	"split size (MB)");

    try
    {
        primo::program_options::scanArgv(optcfg, argc, argv);
    }
    catch (primo::program_options::ParseFailure<char> &ex)
    {
        cout << ex.message() << endl;
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
