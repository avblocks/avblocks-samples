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

const wstring inputFile = L"..\\assets\\aud\\Hydrate-Kenny_Beltrey.adts.aac";
const wstring outputFile = L"Hydrate-Kenny_Beltrey.wav";

void setDefaultOptions(Options& opt)
{
	opt.inputFile = getExeDir() + L"\\" + inputFile;
	opt.outputFile = getExeDir() + L"\\" + outputFile;
}

void help(OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Using: dec_aac_adts_file --input <aac file> --output <wav file>" << endl;
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
        wcout << L"Using default options:" << endl;
        wcout << L" --input " << inputFile << endl;
		wcout << L" --output " << outputFile << endl;
		wcout << endl;

        return Parsed;
    }
    
    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
        (L"help,h",     opt.help,                   L"")
        (L"input,i",    opt.inputFile,  wstring(),  L"input AAC file")
        (L"output,o",   opt.outputFile, wstring(),  L"output WAV file");
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