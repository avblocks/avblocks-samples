/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/

#include <AVBlocks.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;


ColorDescriptor color_formats[] = {
    
    { ColorFormat::YV12,	"yv12",     "Planar Y, V, U (4:2:0) (note V,U order!)" },
    { ColorFormat::NV12,	"nv12",     "Planar Y, merged U->V (4:2:0)" },
    { ColorFormat::YUY2,	"yuy2",     "Composite Y->U->Y->V (4:2:2)" },
    { ColorFormat::UYVY,	"uyvy",     "Composite U->Y->V->Y (4:2:2)" },
    { ColorFormat::YUV411,	"yuv411",   "Planar Y, U, V (4:1:1)" },
    { ColorFormat::YUV420,	"yuv420",   "Planar Y, U, V (4:2:0)" },
    { ColorFormat::YUV422,	"yuv422",   "Planar Y, U, V (4:2:2)" },
    { ColorFormat::Y41P,	"y41p",     "Composite Y, U, V (4:1:1)" },
    { ColorFormat::BGR32,	"bgr32",    "Composite B->G->R" },
	{ ColorFormat::BGRA32,	"bgra32",   "Composite B->G->R->A" },
    { ColorFormat::BGR24,	"bgr24",    "Composite B->G->R" },
    { ColorFormat::BGR565,	"bgr565",   "Composite B->G->R, 5 bit per B & R, 6 bit per G" },
    { ColorFormat::BGR555,	"bgr555",   "Composite B->G->R->A, 5 bit per component, 1 bit per A" },
    { ColorFormat::BGR444,	"bgr444",   "Composite B->G->R->A, 4 bit per component" },
};

const int color_formats_len = sizeof(color_formats) / sizeof(ColorDescriptor);


ColorDescriptor* getColorByName(const char* colorName)
{
    for (int i=0; i < color_formats_len; ++i)
    {
        ColorDescriptor& color = color_formats[i];
        if (compareNoCase(color.name, colorName))
            return &color;
    }
    return NULL;
}

ColorDescriptor* getColorById(primo::codecs::ColorFormat::Enum Id)
{
    for (int i=0; i < color_formats_len; ++i)
    {
        ColorDescriptor& color = color_formats[i];
        if (color.Id == Id)
            return &color;
    }
    return NULL;
}

void listColors()
{
    cout << "\nCOLORS:\n";
    cout << "-------\n";
    for (int i=0; i < color_formats_len; ++i)
    {
        const ColorDescriptor& color = color_formats[i];
        cout << left << setw(20) <<  color.name << color.description << endl;
    }
    cout << endl;
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: dec_avc_au --input <directory> [--output <file>] [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>]";
    cout << " [--colors]";
    cout << endl;
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.input_dir = "../assets/vid/foreman_qcif.h264.au/";
    opt.output_color = *getColorById(primo::codecs::ColorFormat::YUV420);
}

bool validateOptions(Options& opt)
{
    return opt.input_dir.empty() ? false : true;
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.input_dir;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
    ("help,h",  opt.help,       "")
    ("input,i", opt.input_dir, string(),           "input directory (contains sequence of compressed file)")
    ("output,o",opt.output_file,string(),           "output YUV file")
    ("rate,r",  opt.fps,        0.0,                "frame rate")
    ("color,c", opt.output_color,ColorDescriptor(),
     "output color format. Use --colors to list all supported color formats")
    ("colors",  opt.list_colors, "list COLOR formats");
    
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
    
    if (opt.list_colors)
    {
        listColors();
        return Command;
    }
    
    if(!validateOptions(opt))
    {
        help(optcfg);
        return Error;
    }
    
    return Parsed;
}

std::istringstream &operator>>(std::istringstream &in, ColorDescriptor &color)
{
    std::string strColorName;
    in >> strColorName;
    
    ColorDescriptor* colorDesc = getColorByName(strColorName.c_str());
    if(!colorDesc)
        throw primo::program_options::ParseFailure<char>("", strColorName, "Parse error");
    
    color = *colorDesc;
    return in;
}

std::istringstream &operator>>(std::istringstream &in, FrameSize &frameSize)
{
    in >> frameSize.width;
    char ch;
    in >> ch;
    in >> frameSize.height;
    return in;
}