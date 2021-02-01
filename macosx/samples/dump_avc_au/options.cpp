/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "options.h"
#include "util.h"
#include "program_options.h"
#include <iostream>

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

void help(OptionsConfig<char>& optionsConfig)
{
	cout << "dump_avc_au --input <h264-file> --output <folder>" << endl;
	doHelp(cout, optionsConfig);
}

void setDefaultOptions(Options& opt)
{
	opt.input_file = getExeDir() + "/../assets/vid/foreman_qcif.h264";
	opt.output_dir = getExeDir() + "/foreman_qcif.h264.au";
}

bool validateOptions(Options& opt)
{
	return !opt.input_file.empty() && !opt.output_dir.empty();
}

OptionResult prepareOptions(Options& opt, int argc, char* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.input_file;
		cout << " --output " << opt.output_dir;
        cout << endl;
        return Parsed;
    }

    OptionsConfig<char> optionsConfig;
	optionsConfig.addOptions()
		("help,?", opt.help, "")
		("input,i", opt.input_file, string(), "input file (AVC/H.264)")
		("output,o", opt.output_dir, string(),"output directory");

	try
	{
		scanArgv(optionsConfig, argc, argv);
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







