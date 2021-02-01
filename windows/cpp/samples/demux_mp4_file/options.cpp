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

void setDefaultOptions(Options& opt)
{
	opt.inputFile = getExeDir() + L"\\..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v";
	opt.outputFile = getExeDir() + L"\\big_buck_bunny_trailer_iphone";
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Usage: demux_mp4_file -i <input_mp4_file> -o <output_mp4_file_name_without_extension>\n";
    primo::program_options::doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{
    return !(opt.inputFile.empty() || opt.outputFile.empty());    
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
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
            (L"help,h", opt.help, L"")
            (L"input,i", opt.inputFile, wstring(), L"Input mp4 file.")
            (L"output,o", opt.outputFile, wstring(), L"Output mp4 file without extension.");
			
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