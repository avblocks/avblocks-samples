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


void setDefaultOptions(Options& opt)
{
	opt.inputFile = getExeDir() + L"\\..\\assets\\aud\\equinox-48KHz.wav";
	opt.outputFile = getExeDir() + L"\\equinox-48KHz.adif.aac";
}

void help(OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Using: enc_aac_adif_file --input <wav file> --output <aac file>" << endl;
	doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
	if(opt.inputFile.empty())
	{
		wcout << L"Input file needed" << endl;
		return false;
	}
	else
	{
		wcout << L"Input file: " << opt.inputFile << endl;
	}

	if(opt.outputFile.empty())
	{
		wcout << L"Output file needed" << endl;
		return false;
	}
	else
	{
		wcout << L"Output file: " << opt.outputFile << endl;
	}

	return true;
}

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.inputFile;
		wcout << L" --output " << opt.outputFile;
		wcout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",     opt.help,                   L"")
        (L"input,i",    opt.inputFile,  wstring(),  L"input WAV file")
        (L"output,o",   opt.outputFile, wstring(),  L"output AAC file");
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