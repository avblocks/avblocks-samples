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

using namespace std;
using namespace primo::avblocks;	
using namespace primo::codecs;	
using namespace primo::program_options;

PresetDescriptor avb_presets[] = {
	
	// video presets
	{ Preset::Video::DVD::PAL_4x3_MP2,													L"mpg" },
	{ Preset::Video::DVD::PAL_16x9_MP2,													L"mpg" },
	{ Preset::Video::DVD::NTSC_4x3_MP2,													L"mpg" },
	{ Preset::Video::DVD::NTSC_16x9_MP2,												L"mpg" },
	{ Preset::Video::AppleTV::H264_480p,												L"mp4" },
	{ Preset::Video::AppleTV::H264_720p,												L"mp4" },
	{ Preset::Video::AppleTV::MPEG4_480p,												L"mp4" },
	{ Preset::Video::AppleLiveStreaming::WiFi_H264_640x480_30p_1200K_AAC_96K,			L"ts" },
	{ Preset::Video::AppleLiveStreaming::WiFi_Wide_H264_1280x720_30p_4500K_AAC_128K,	L"ts" },
	{ Preset::Video::Generic::MP4::Base_H264_AAC,										L"mp4" },
	{ Preset::Video::iPad::H264_576p,													L"mp4" },
	{ Preset::Video::iPad::H264_720p,													L"mp4" },
	{ Preset::Video::iPad::MPEG4_480p,													L"mp4" },
	{ Preset::Video::iPhone::H264_480p,													L"mp4" },
	{ Preset::Video::iPhone::MPEG4_480p,												L"mp4" },
	{ Preset::Video::iPod::H264_240p,													L"mp4" },
	{ Preset::Video::iPod::MPEG4_240p,													L"mp4" },
	{ Preset::Video::AndroidPhone::H264_360p,											L"mp4" },
	{ Preset::Video::AndroidPhone::H264_720p,											L"mp4" },
	{ Preset::Video::AndroidTablet::H264_720p,											L"mp4" },
	{ Preset::Video::AndroidTablet::WebM_VP8_720p,										L"webm" },
	{ Preset::Video::VCD::PAL,															L"mpg" },
	{ Preset::Video::VCD::NTSC,															L"mpg" },
	{ Preset::Video::Generic::WebM::Base_VP8_Vorbis,									L"webm" }
};

const int avb_presets_len = sizeof(avb_presets) / sizeof(PresetDescriptor);

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

PresetDescriptor* getPresetByName(const char* presetName)
{
	for (int i=0; i < avb_presets_len; ++i)
	{
		PresetDescriptor& preset = avb_presets[i];
		if (compareNoCase(preset.name, presetName))
			return &preset;
	}
	return NULL;
}

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

void listPresets()
{
	wcout << L"\nPRESETS (default extension):" << endl;
	wcout << L"----------------------------" << endl;
	for (int i=0; i < avb_presets_len; ++i)
	{
		const PresetDescriptor& preset = avb_presets[i];
		wcout << left << setw(45) <<  preset.name << L" ." << preset.extension << endl;
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

void help(OptionsConfig<wchar_t>& optionsConfig)
{
	wcout << L"\nUsage: streamencoder --frame <width>x<height> --rate <fps> --color <COLOR> --input <file> --output <file> [--preset <PRESET>]";
	wcout << L" [--colors] [--presets]" << endl;
	doHelp(wcout, optionsConfig);
}

void setDefaultOptions(Options& opt)
{
	opt.yuv_file = getExeDir() + L"\\..\\assets\\vid\\foreman_qcif.yuv";
	opt.yuv_color = *getColorById(primo::codecs::ColorFormat::YUV420);
	opt.output_file = getExeDir() + L"\\foreman_qcif.mp4";
	opt.preset = *getPresetByName(primo::avblocks::Preset::Video::Generic::MP4::Base_H264_AAC);
	opt.yuv_fps = 30.0;
	opt.yuv_frame.height = 144;
	opt.yuv_frame.width = 176;
}

bool validateOptions(Options& opt)
{
        // fix output: append file extension
    if (!opt.output_file.empty() && opt.preset.extension)
	{
		wstring::size_type pos = opt.output_file.rfind('.');
		
		if (string::npos == pos)
		{
			opt.output_file.append(L".");
		}
		else
		{
			opt.output_file = opt.output_file.substr(0, pos+1);
		}
        
		opt.output_file.append(opt.preset.extension);
	}
    
	if (opt.yuv_file.empty())
	{
            return false;
	}
	
	if (opt.yuv_frame.width == 0 || opt.yuv_frame.height == 0)
	{
            return false;
	}

	if (opt.yuv_color.Id == ColorFormat::Unknown)
	{
            return false;
	}
	
	if (opt.yuv_fps == 0.0)
	{
            return false;
	}
	
	if (opt.output_file.empty())
	{
            return false;
	}
	
	if (!opt.preset.name)
	{
            return false;
	}
	
        return true;
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{	
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L" --input " << opt.yuv_file;
        wcout << L" --color " << opt.yuv_color.name;
        wcout << L" --rate " << opt.yuv_fps;
        wcout << L" --frame " << opt.yuv_frame.width << L"x" << opt.yuv_frame.height;   
        wcout << L" --output " << opt.output_file;
        wcout << L" --preset " << opt.preset.name;
        wcout << endl;
        
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",		opt.help,								L"")
		(L"input,i",	opt.yuv_file,		wstring(),			L"input YUV file")
		(L"frame,f",	opt.yuv_frame,		FrameSize(),		L"input frame size, <width>x<height>")
		(L"rate,r",		opt.yuv_fps,		0.0,				L"input frame rate")
		(L"color,c",	opt.yuv_color,		ColorDescriptor(),	L"input color format. Use --colors to list all supported color formats")
		(L"output,o",	opt.output_file,	wstring(),			L"output file")
		(L"preset,p",	opt.preset,			PresetDescriptor(),	L"output preset. Use --presets to list all supported presets")
		(L"colors",		opt.list_colors,						L"list COLOR constants")
		(L"presets",	opt.list_presets,						L"list PRESET constants");

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

	if(opt.list_presets)
	{
		listPresets();
		return Command;
	}

	if (!validateOptions(opt))
    {
        help(optionsConfig);
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

std::wistringstream &operator>>(std::wistringstream &in, PresetDescriptor &preset)
{
	wstring presetName;
    in >> presetName;
	std::string presetNameS( presetName.begin(), presetName.end());
	bool validatePreset = (getPresetByName(presetNameS.c_str())) ? true : false;
	if(!validatePreset)
		throw ParseFailure<TCHAR>(_T(""), presetName, _T("Parse error"));
	else
		preset = *getPresetByName(presetNameS.c_str());

    return in;
}