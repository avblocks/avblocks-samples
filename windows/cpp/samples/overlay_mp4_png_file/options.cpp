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
    opt.overlayImage = getExeDir() + L"\\..\\assets\\overlay\\smile_icon.png";
    opt.position.x = 50;
    opt.position.y = 50;
    opt.alpha = 0.5;
	opt.outputFile = getExeDir() + L"\\overlay_mp4_png_file.m4v";
}

void help(primo::program_options::OptionsConfig<wchar_t>& optcfg)
{
    wcout << L"Usage: -i <input video file> -w <PNG file> -p <x>:<y> -a <transparency> -o <output video file>\n";
    primo::program_options::doHelp(wcout, optcfg);
}

bool validateOptions(Options& opt)
{

    if (opt.inputFile.empty() || opt.outputFile.empty() || opt.overlayImage.empty())
        return false;

    if (opt.alpha > 1 && opt.alpha < 0)
        return false;

    if (opt.position.x < 0 || opt.position.y < 0)
        return false;

    return true;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input "       << opt.inputFile;
        wcout << L" --watermark "   << opt.overlayImage;
        wcout << L" --position " << opt.position.x 
            << L":" << opt.position.y;
        wcout << L" --alpha " << opt.alpha;
        wcout << L" --output " << opt.outputFile;
        wcout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<wchar_t> optcfg;
    optcfg.addOptions()
            (L"help,h",         opt.help,           L"")
            (L"input,i",        opt.inputFile,      wstring(),      L"input video file.")
            (L"watermark,w",    opt.overlayImage,   wstring(),      L"overlay PNG image.")
            (L"position,p",     opt.position,       Position(),     L"the top left position of the watermark in the video frame. The top left position of the video frame is origin for this coordinate system.")
            (L"alpha,a",        opt.alpha,          0.0,            L"watermark transparency from 0(transparent) to 1(opaque)")
            (L"output,o",       opt.outputFile,     wstring(),      L"overlayed output video.");

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

std::wistringstream &operator>>(std::wistringstream &in, Position &position)
{
    in >> position.x;

    wchar_t ch;
    in >> ch;

    in >> position.y;
    return in;
}