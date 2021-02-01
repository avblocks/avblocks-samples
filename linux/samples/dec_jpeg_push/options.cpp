/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include <iostream>
#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: dec_jpeg_push --input <file.jpeg> --frame <width>x<height> --output <file.yuv>\n";
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../assets/img/cube0000.jpeg";
    opt.outputFile = getExeDir() + "/cube0000.yuv";
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

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.inputFile;
        cout << " --frame " << opt.frameSize.width << "x" << opt.frameSize.height;
        cout << " --output " << opt.outputFile;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
            ("help,h", opt.help, "")
            ("input,i", opt.inputFile, string(), "input jpeg file")
            ("frame,f", opt.frameSize, FrameSize(), "frame size, <width>x<height>")
            ("output,o", opt.outputFile, string(), "output yuv file");
       
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

std::istringstream &operator>>(std::istringstream &in, FrameSize &frameSize)
{
	in >> frameSize.width;
	char ch;
	in >> ch;
	in >> frameSize.height;
	return in;
}

