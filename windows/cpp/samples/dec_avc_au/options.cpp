/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "stdafx.h"
#include "options.h"
#include "util.h"
#include "program_options.h"

using namespace std;
using namespace primo::avblocks;	
using namespace primo::codecs;	
using namespace primo::program_options;

/*
command line options:

long format:
	dec_avc_au --input input.h264 --output file.yuv --frame 352x288 --rate 30 --color yuv420 

short format:
	dec_avc_au -i input.h264 -o file.yuv -f 352x288 -r 30 -c yuv420 

TODO: separate syntax from semantic processing.
1. Command Line Parser: get all options and validate syntax (get name->value; or just name; or just value)
2. enc_yuv_preset_file: translate command options to encoder options (parse names and values).
*/

ColorDescriptor color_formats[] = {
	
	{ ColorFormat::YV12,	L"yv12",	L"Planar Y, V, U (4:2:0) (note V,U order!)" },
	{ ColorFormat::NV12,	L"nv12",	L"Planar Y, merged U->V (4:2:0)" },
	{ ColorFormat::YUY2,	L"yuy2",	L"Composite Y->U->Y->V (4:2:2)" },
	{ ColorFormat::UYVY,	L"uyvy",	L"Composite U->Y->V->Y (4:2:2)" },
	{ ColorFormat::YUV411,	L"yuv411",	L"Planar Y, U, V (4:1:1)" },
	{ ColorFormat::YUV420,	L"yuv420",	L"Planar Y, U, V (4:2:0)" },
	{ ColorFormat::YUV422,	L"yuv422",	L"Planar Y, U, V (4:2:2)" },
	{ ColorFormat::YUV444,	L"yuv444",	L"Planar Y, U, V (4:4:4)" },
	{ ColorFormat::Y411,	L"y411",	L"Composite Y, U, V (4:1:1)" },
	{ ColorFormat::Y41P,	L"y41p",	L"Composite Y, U, V (4:1:1)" },
	{ ColorFormat::BGRA32,	L"bgra32",	L"Composite B->G->R" },
	{ ColorFormat::BGR32,	L"bgr32",	L"Composite B->G->R->A" },
	{ ColorFormat::BGR24,	L"bgr24",	L"Composite B->G->R" },
	{ ColorFormat::BGR565,	L"bgr565",	L"Composite B->G->R, 5 bit per B & R, 6 bit per G" },
	{ ColorFormat::BGR555,	L"bgr555",	L"Composite B->G->R->A, 5 bit per component, 1 bit per A" },
	{ ColorFormat::BGR444,	L"bgr444",	L"Composite B->G->R->A, 4 bit per component" },
	{ ColorFormat::GRAY,	L"gray",	L"Luminance component only" },
	{ ColorFormat::YUV420A,	L"yuv420a",	L"Planar Y, U, V, Alpha (4:2:0)" },
	{ ColorFormat::YUV422A,	L"yuv422a",	L"Planar Y, U, V, Alpha (4:2:2)" },
	{ ColorFormat::YUV444A,	L"yuv444a",	L"Planar Y, U, V, Alpha (4:4:4)" },
	{ ColorFormat::YVU9,	L"yvu9",	L"Planar Y, V, U, 9 bits per sample" },
};

const int color_formats_len = sizeof(color_formats) / sizeof(ColorDescriptor);

ColorDescriptor* getColorByName(const wchar_t* colorName)
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
	wcout << L"\nCOLORS:" << endl;
	wcout << L"-------" << endl;
	for (int i=0; i < color_formats_len; ++i)
	{
		const ColorDescriptor& color = color_formats[i];
		wcout << left << setw(20) <<  color.name << color.description << endl;
	}
}

void help(OptionsConfig<wchar_t>& optionsConfig)
{
	wcout << L"\nUsage: dec_avc_au --input <directory> [--output <file>] [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>]";
	wcout << L" [--colors]" << endl;
	doHelp(wcout, optionsConfig);
}

void setDefaultOptions(Options& opt)
{
	opt.input_dir = L"\\..\\assets\\vid\\foreman_qcif.h264.au\\";
	opt.output_color = *getColorById(primo::codecs::ColorFormat::YUV420);
}

std::wistringstream &operator>>(std::wistringstream &in, FrameSize &frameSize)
{
    in >> frameSize.width;

    wchar_t ch;
    
	in >> ch;
    
	in >> frameSize.height;
    return in;
}

std::wistringstream &operator>>(std::wistringstream &in, ColorDescriptor &color)
{
	std::wstring strColorName;
	in >> strColorName;

	ColorDescriptor* colorDesc = getColorByName(strColorName.c_str());
	if(!colorDesc)
		throw primo::program_options::ParseFailure<wchar_t>(L"", strColorName, L"Parse error");

	color = *colorDesc;
	return in;
}

bool validateOptions(Options& opt)
{
    return opt.input_dir.empty() ? false : true;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.input_dir;
        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",		opt.help,								L"")
		(L"input,i",	opt.input_dir,		wstring(),			L"input directory (contains sequence of compressed file)")
		(L"output,o",	opt.output_file,	wstring(),			L"output YUV file")
		(L"frame,f",	opt.frame_size,		FrameSize(),		L"frame size, <width>x<height>")
		(L"rate,r",		opt.fps,			0.0,				L"frame rate")
		(L"color,c",	opt.output_color,	ColorDescriptor(),	L"output color format. Use --colors to list all supported color formats")
		(L"colors",		opt.list_colors,						L"list COLOR constants");

	try
	{
		scanArgv(optionsConfig, argc, argv);
	}
	catch (ParseFailure<wchar_t> &ex)
	{
		wcout << ex.message() << endl;
		help(optionsConfig);
		return Error;
	}

	if(opt.help)
	{
		help(optionsConfig);
		return Command;
	}

	if (opt.list_colors)
	{
		listColors();
		return Command;
	}

	if (!validateOptions(opt))
    {
        help(optionsConfig);
        return Error;
    }

	return Parsed;
}







