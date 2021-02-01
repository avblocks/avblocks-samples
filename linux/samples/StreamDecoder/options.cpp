/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

// memmove in AVBlocks
#include <string.h>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <assert.h>

#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;	

ColorDescriptor color_formats[] = {
	
	{ ColorFormat::YV12,	"yv12",         "Planar Y, V, U (4:2:0) (note V,U order!)" },
	{ ColorFormat::NV12,	"nv12",         "Planar Y, merged U->V (4:2:0)" },
	{ ColorFormat::YUY2,	"yuy2",         "Composite Y->U->Y->V (4:2:2)" },
	{ ColorFormat::UYVY,	"uyvy",         "Composite U->Y->V->Y (4:2:2)" },
	{ ColorFormat::YUV411,	"yuv411",	"Planar Y, U, V (4:1:1)" },
	{ ColorFormat::YUV420,	"yuv420",	"Planar Y, U, V (4:2:0)" },
	{ ColorFormat::YUV422,	"yuv422",	"Planar Y, U, V (4:2:2)" },
	{ ColorFormat::YUV444,	"yuv444",	"Planar Y, U, V (4:4:4)" },
	{ ColorFormat::Y411,	"y411",         "Composite Y, U, V (4:1:1)" },
	{ ColorFormat::Y41P,	"y41p",         "Composite Y, U, V (4:1:1)" },
	{ ColorFormat::BGR32,	"bgr32",	"Composite B->G->R" },
	{ ColorFormat::BGRA32,	"bgra32",	"Composite B->G->R->A" },
	{ ColorFormat::BGR24,	"bgr24",	"Composite B->G->R" },
	{ ColorFormat::BGR565,	"bgr565",	"Composite B->G->R, 5 bit per B & R, 6 bit per G" },
	{ ColorFormat::BGR555,	"bgr555",	"Composite B->G->R->A, 5 bit per component, 1 bit per A" },
	{ ColorFormat::BGR444,	"bgr444",	"Composite B->G->R->A, 4 bit per component" },
	{ ColorFormat::GRAY,	"gray",         "Luminance component only" },
	{ ColorFormat::YUV420A,	"yuv420a",	"Planar Y, U, V, Alpha (4:2:0)" },
	{ ColorFormat::YUV422A,	"yuv422a",	"Planar Y, U, V, Alpha (4:2:2)" },
	{ ColorFormat::YUV444A,	"yuv444a",	"Planar Y, U, V, Alpha (4:4:4)" },
	{ ColorFormat::YVU9,	"yvu9",         "Planar Y, V, U, 9 bits per sample" },
};

const int color_formats_len = sizeof(color_formats) / sizeof(ColorDescriptor);


StreamTypeDescriptor stream_types[] = 
{
	{ primo::codecs::StreamType::MP4 , "MP4" , "mp4"},
	{ primo::codecs::StreamType::MP4 , "MP4" , "mov"},
	{ primo::codecs::StreamType::MP4 , "MP4" , "m4v"},

	{ primo::codecs::StreamType::AVI , "AVI" , "avi"},
	{ primo::codecs::StreamType::MPEG_PS , "MPEG_PS" , "mpg"},
	{ primo::codecs::StreamType::MPEG_TS , "MPEG_TS" , "ts"},

	{ primo::codecs::StreamType::MPEG2_Video, "MPEG2_Video" , "m2v"},
	{ primo::codecs::StreamType::MPEG2_Video, "MPEG2_Video" , "mpv"},

	{ primo::codecs::StreamType::ASF, "ASF" , "asf"},
	{ primo::codecs::StreamType::ASF, "ASF" , "wmv"},

	{ primo::codecs::StreamType::H264, "H264" , "h264"},
	{ primo::codecs::StreamType::WebM, "WebM" , "webm"},
};

const int stream_types_len = sizeof(stream_types) / sizeof(StreamTypeDescriptor);

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

StreamTypeDescriptor* getStreamTypeByName(const char* streamTypeName)
{
	for (int i=0; i < stream_types_len; ++i)
	{
		StreamTypeDescriptor& stream = stream_types[i];
		if (compareNoCase(stream.name, streamTypeName))
			return &stream;
	}
	return NULL;
}

StreamTypeDescriptor* getStreamTypeByExtension(const char* extension)
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
	cout << "\nSTREAM TYPE (default extension):" << endl;
	cout << "----------------------------" << endl;

	for (int i=0; i < stream_types_len; ++i)
	{
		StreamTypeDescriptor& stream = stream_types[i];
		cout << left << setw(20) <<  stream.name << " (." << stream.extension << ")" << endl;
	}
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "\nUsage: StreamDecoder --input <file> [--streamtype <STREAM_TYPE>] --output <file> [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>]";
    cout << " [--colors] [--streams]\n";
    primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.input_file = getExeDir() + "/../assets/vid/foreman_qcif.h264";
    opt.output_file = getExeDir() + "/foreman_qcif.yuv";
    opt.yuv_color = *getColorById(primo::codecs::ColorFormat::YUV420);
    opt.input_stream_type = *getStreamTypeById(primo::codecs::StreamType::H264);
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
                    string ext = opt.input_file.substr(pos + 1);
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

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --input " << opt.input_file;
        cout << " --output " << opt.output_file;
        cout << " --color " << opt.yuv_color.name;
        cout << " --streamtype " << opt.input_stream_type.name;
        cout << endl;
        return Parsed;
    }
 
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
            ("help,h",      opt.help,       "")
            ("input,i",     opt.input_file,         string(), "input file (compressed elementary stream)")
            ("streamtype,s",opt.input_stream_type,  StreamTypeDescriptor(),
                "input stream type. Use --streamtypes to list all supported stream types.")
            ("frame,f",     opt.yuv_frame,          FrameSize(), "frame size, <width>x<height>")
            ("output,o",    opt.output_file,        string(), "output YUV file")
            ("rate,r",      opt.yuv_fps,            0.0,      "frame rate")
            ("color,c",     opt.yuv_color,          ColorDescriptor(),
                "output color format. Use --colors to list all supported color formats.")
            ("colors",      opt.list_colors, "list color formats")
            ("streamtypes", opt.list_stream_types, "list stream types");
       
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
    
    if (opt.list_stream_types)
    {
        listStreamTypes();
        return Command;
    }
    
    if (!validateOptions(opt))
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

std::istringstream &operator>>(std::istringstream &in, StreamTypeDescriptor &streamType)
{
	std::string streamTypeName;
	in >> streamTypeName;

	StreamTypeDescriptor* streamTypeDesc = getStreamTypeByName(streamTypeName.c_str());
	if(!streamTypeDesc)
		throw primo::program_options::ParseFailure<char>("", streamTypeName, "Parse error");

	streamType = *streamTypeDesc;
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