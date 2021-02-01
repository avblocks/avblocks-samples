/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "stdafx.h"
#include "options.h"
#include "util.h"
#include "program_options.h"
#include <regex>

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;
using namespace primo::program_options;

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
	{ ColorFormat::BGR32,	L"bgr32",	L"Composite B->G->R" },
	{ ColorFormat::BGRA32,	L"bgra32",	L"Composite B->G->R->A" },
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

StreamTypeDescriptor stream_types[] = 
{
	{ primo::codecs::StreamType::MP4 , L"MP4" , L"mp4"},
	{ primo::codecs::StreamType::MP4 , L"MP4" , L"mov"},
	{ primo::codecs::StreamType::MP4 , L"MP4" , L"m4v"},

	{ primo::codecs::StreamType::AVI , L"AVI" , L"avi"},
	{ primo::codecs::StreamType::MPEG_PS , L"MPEG_PS" , L"mpg"},
	{ primo::codecs::StreamType::MPEG_TS , L"MPEG_TS" , L"ts"},

	{ primo::codecs::StreamType::MPEG2_Video, L"MPEG2_Video" , L"m2v"},
	{ primo::codecs::StreamType::MPEG2_Video, L"MPEG2_Video" , L"mpv"},

	{ primo::codecs::StreamType::ASF, L"ASF" , L"asf"},
	{ primo::codecs::StreamType::ASF, L"ASF" , L"wmv"},

	{ primo::codecs::StreamType::H264, L"H264" , L"h264"},
	{ primo::codecs::StreamType::WebM, L"WebM" , L"webm"},
};

const int stream_types_len = sizeof(stream_types) / sizeof(StreamTypeDescriptor);

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

StreamTypeDescriptor* getStreamTypeByName(const wchar_t* streamTypeName)
{
	for (int i=0; i < stream_types_len; ++i)
	{
		StreamTypeDescriptor& stream = stream_types[i];
		if (compareNoCase(stream.name, streamTypeName))
			return &stream;
	}
	return NULL;
}

StreamTypeDescriptor* getStreamTypeByExtension(const wchar_t* extension)
{
	for (int i=0; i < stream_types_len; ++i)
	{
		StreamTypeDescriptor& stream = stream_types[i];
		if (compareNoCase(stream.extension, extension))
			return &stream;
	}
	return NULL;
}

StreamTypeDescriptor* getStreamTypeById(primo::codecs::StreamType::Enum stream_id)
{
	for (int i=0; i < stream_types_len; ++i)
	{
		StreamTypeDescriptor& stream = stream_types[i];
		if (stream.Id == stream_id)
			return &stream;
	}
	return NULL;
}

void listStreamTypes()
{
	wcout << L"\nSTREAM TYPE (default extension):" << endl;
	wcout << L"----------------------------" << endl;

	for (int i=0; i < stream_types_len; ++i)
	{
		StreamTypeDescriptor& stream = stream_types[i];
		wcout << left << setw(20) <<  stream.name << L" (." << stream.extension << L")" << endl;
	}
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

void help(OptionsConfig<wchar_t> &optConf)
{
	wcout << L"\nUsage: streamdecoder --input <file> [--streamtype <STREAM TYPE>] --output <file> [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>]";
	wcout << L" [--colors] [--streamtypes]" << endl;
	doHelp(wcout, optConf);
	wcout << endl;
}

std::wistringstream &operator>>(std::wistringstream &in, FrameSize &frameSize)
{
    in >> frameSize.width;

    wchar_t ch;
    
	in >> ch;
    
    in >> frameSize.height;
    return in;
}

std::wistringstream &operator>>(std::wistringstream &in, StreamTypeDescriptor &streamType)
{
	wstring streamTypeName;
    in >> streamTypeName;
	bool validateStreamType = (getStreamTypeByName(streamTypeName.c_str())) ? true : false;
	if(!validateStreamType)
		throw ParseFailure<TCHAR>(_T(""), streamTypeName, _T("Parse error"));
	else
		streamType = *getStreamTypeByName( streamTypeName.c_str() );
	
    return in;
}

std::wistringstream &operator>>(std::wistringstream &in, ColorDescriptor &color)
{
	wstring colorName;
    in >> colorName;
	bool validateColor = (getColorByName(colorName.c_str())) ? true : false;
	if(!validateColor)
		throw ParseFailure<TCHAR>(_T(""), colorName, _T("Parse error"));
	else
		color = *getColorByName(colorName.c_str());

    return in;
}

void setDefaultOptions(Options& opt)
{
	opt.input_stream_type = *getStreamTypeById(primo::codecs::StreamType::H264);
	opt.yuv_color = *getColorById(primo::codecs::ColorFormat::YUV420);
	opt.output_file = getExeDir() + L"\\foreman_qcif.yuv";
	opt.input_file = getExeDir() + L"\\..\\assets\\vid\\foreman_qcif.h264";
}

bool validateOptions(Options& opt)
{
    if (opt.input_file.empty() || opt.output_file.empty())
        return false;

    if (opt.input_stream_type.Id == primo::codecs::StreamType::Unknown)
    {
            string::size_type pos = opt.input_file.rfind('.');
            if (string::npos != pos)
            {
                    wstring ext = opt.input_file.substr(pos + 1);
                    StreamTypeDescriptor* streamType = getStreamTypeByExtension(ext.c_str());

                    if(streamType)
                    {
                        opt.input_stream_type.Id = streamType->Id;
                    }
                    else
                    {
                        cout << "Input stream type cannot be detected by file extension." << endl;
                        return false;
                    }
            }
    }
    return true;    
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.input_file;
        wcout << L" --output " << opt.output_file;
        wcout << L" --color " << opt.yuv_color.name;
        wcout << L" --streamtype " << opt.input_stream_type.name;
        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",			opt.help,										L"")
		(L"input,i",		opt.input_file,			wstring(),				L"input file (compressed elementary stream)")
		(L"streamtype,s",	opt.input_stream_type,	StreamTypeDescriptor(),	L"input stream type. Use --streamtypes to list all supported stream types.")
		(L"frame,f",		opt.yuv_frame,			FrameSize(),			L"frame size, <width>x<height>")
		(L"rate,r",			opt.yuv_fps,			0.0,					L"frame rate")
		(L"color,c",		opt.yuv_color,			ColorDescriptor(),		L"output color format. Use --colors to list all supported color formats.")
		(L"output,o",		opt.output_file,		wstring(),				L"output YUV file")
		(L"colors",			opt.list_colors,								L"list COLOR constants")
		(L"streamtypes",	opt.list_stream_types,							L"list STREAM TYPE constants");

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

	if(opt.list_colors)
	{
		listColors();
		return Command;
	}

	if(opt.list_stream_types)
	{
		listStreamTypes();
		return Command;
	}

	if (!validateOptions(opt))
    {
        help(optionsConfig);
        return Error;
    }

	return Parsed;
}