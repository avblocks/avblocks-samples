/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "program_options.h"
#include "util.h"
#include "options.h"

using namespace std;
using namespace primo::program_options;

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Usage: dec_jpeg_push --input <file.jpeg> --frame <width>x<height> --output <file.yuv>\n";
    primo::program_options::doHelp(wcout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + L"\\..\\assets\\img\\cube0000.jpeg";
    opt.outputFile = getExeDir() + L"\\cube0000.yuv";
    opt.frameSize.width = 640;
    opt.frameSize.height = 480;
}

bool validateOptions(Options& opt)
{
    return (!opt.inputFile.empty() && 
            !opt.outputFile.empty() &&
            opt.frameSize.width > 0 &&
            opt.frameSize.height > 0);    
}

ErrorCodes prepareOptions(Options &opt, int argc, wchar_t* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.inputFile;
        wcout << L" --frame " << opt.frameSize.width << "x" << opt.frameSize.height;
        wcout << L" --output " << opt.outputFile;
        wcout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
            (L"help,h", opt.help, L"")
            (L"input,i", opt.inputFile, wstring(), L"input jpeg file")
            (L"frame,f", opt.frameSize, FrameSize(), L"frame size, <width>x<height>")
            (L"output,o", opt.outputFile, wstring(), L"output yuv file");
       
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

std::wistringstream &operator>>(std::wistringstream &in, FrameSize &frameSize)
{
	in >> frameSize.width;
	wchar_t ch;

	in >> ch;

	in >> frameSize.height;
	return in;
}