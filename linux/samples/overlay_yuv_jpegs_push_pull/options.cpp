/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "options.h"
#include "util.h"
#include "program_options.h"

#include <string>

using namespace std;
using namespace primo::program_options;

void help(OptionsConfig<char> &optionsConfig)
{
	cout << "Usage: overlay_yuv_jpegs_push_pull --input <file.m4v> --output <file.mp4> --overlay_input <directory> --overlay_count <count>" << std::endl;
	doHelp(cout, optionsConfig);
}

bool validateOptions(Options& opt)
{
    return (!opt.input_file.empty() &&
            !opt.overlay_image.empty() &&
            opt.overlay_count > 0);    
}

void setDefaultOptions(Options& opt)
{
    opt.input_file    = getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.m4v";
    opt.output_file   = getExeDir() + "/overlay_yuv_jpeg.m4v";
    opt.overlay_image = getExeDir() + "/../assets/overlay/cube";
    opt.overlay_count = 250;
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input "         << opt.input_file;
        cout << " --output "        << opt.output_file;
        cout << " --overlay_input " << opt.overlay_image;
        cout << " --overlay_count " << opt.overlay_count;
        cout << endl;
        return Parsed;
    }

	OptionsConfig<char> optionsConfig;
	optionsConfig.addOptions()
	("help,?",           opt.help,                    "");
	("input,i",	     opt.input_file,	string(), "file; if no input is specified a default input file is used.");
        ("output,o",         opt.output_file,   string(), "file; if no output file name is specified a default one is used.");
        ("overlay_input,oi", opt.overlay_image, string(), "directory to overlay files");
        ("overlay_count,oc", opt.overlay_count, string(), "number; if no number is specified a default one is used.");

	try
	{
		primo::program_options::scanArgv(optionsConfig, argc, argv);
	}
	catch (ParseFailure<char> &ex)
	{
		cout << ex.message() << endl;
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