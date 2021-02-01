/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
	opt.inputFile = getExeDir() + "/../assets/mov/big_buck_bunny_trailer_iphone.m4v";
    opt.overlayImage = getExeDir() + "/../assets/overlay/smile_icon.png";
    opt.position.x = 50;
    opt.position.y = 50;
    opt.alpha = 0.5;
	opt.outputFile = getExeDir() + "/overlay_mp4_png_file.m4v";
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: overlay_mp4_png_file -i <input video file> -w <PNG file> -p <x>:<y> -a <transparency> -o <output video file>\n";
    primo::program_options::doHelp(cout, optcfg);
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

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input "       << opt.inputFile;
        cout << " --watermark "   << opt.overlayImage;
        cout << " --position " << opt.position.x 
            << ":" << opt.position.y;
        cout << " --alpha " << opt.alpha;
        cout << " --output " << opt.outputFile;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
            ("help,h",         opt.help,           "")
            ("input,i",        opt.inputFile,      string(),      "input video file.")
            ("watermark,w",    opt.overlayImage,   string(),      "overlay PNG image.")
            ("position,p",     opt.position,       Position(),    "the top left position of the watermark in the video frame. The top left position of the video frame is origin for this coordinate system.")
            ("alpha,a",        opt.alpha,          0.0,           "watermark transparency from 0(transparent) to 1(opaque).")
            ("output,o",       opt.outputFile,     string(),      "overlayed output video.");

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

std::istringstream &operator>>(std::istringstream &in, Position &position)
{
    in >> position.x;

    char ch;
    in >> ch;

    in >> position.y;
    return in;
}