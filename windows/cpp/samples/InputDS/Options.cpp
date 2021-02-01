/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "stdafx.h"
#include "Options.h"
#include "Util.h"
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

void printPresets()
{
	wcout << L"\nPRESETS" << endl;
	wcout << L"-------------------------------------------------------" << endl;
	for (int i=0; i < avb_presets_len; ++i)
	{
		const PresetDescriptor& preset = avb_presets[i];
		wcout << left << setw(45) <<  preset.name << L" ." << preset.extension << endl;
	}
}

bool validateOptions(Options& opt)
{
    if(opt.input_file.empty())
	{
		wcout << L"Need input file" << endl;
		return false;
	}
	else
	{
		wcout << L"Input file: " << opt.input_file << endl;
	}

	if(opt.output_file.empty())
	{
		wcout << L"Need output file" << endl;
		return false;
	}
	else
	{
		wcout << L"Output file: " << opt.output_file << endl;
	}

	if(opt.preset.name == NULL)
	{
		wcout << L"Need preset" << endl;
		return false;
	}
	else
	{
		wcout << L"Preset: " << opt.preset.name << endl;
	}

	return true;
}

template<typename T>
PresetDescriptor* getPresetByName(const T* presetName)
{
	for (int i=0; i < avb_presets_len; ++i)
	{
		PresetDescriptor& preset = avb_presets[i];
		if (compareNoCase(preset.name, presetName))
			return &preset;
	}
	return NULL;
}

std::wistringstream &operator>>(std::wistringstream &in, PresetDescriptor &preset)
{
	std::wstring strPresetName;
	in >> strPresetName;

	PresetDescriptor* presetDesc = getPresetByName(strPresetName.c_str());
	if(!presetDesc)
		throw primo::program_options::ParseFailure<wchar_t>(L"", strPresetName, L"Parse error");

	preset = *presetDesc;
	return in;
}

void help(OptionsConfig<wchar_t>& optionsConfig)
{
	wcout << L"\nUsage: InputDS --preset encodingPreset --input inputFile --output outputFile\r\n" << 
				L"Example: InputDS --preset mp4.h264.aac  --input input.avi --output output.mp4" << endl;
	doHelp(wcout, optionsConfig);
}

void setDefaultOptions(Options& opt)
{
	opt.input_file = getExeDir() + L"\\..\\assets\\mov\\bars_100.avi";
	opt.output_file = getExeDir() + L"\\bars_100.mp4";
	opt.preset = *getPresetByName(Preset::Video::Generic::MP4::Base_H264_AAC);
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L"--input " << opt.input_file << endl;
		wcout << L"--output " << opt.output_file << endl;
		wcout << L"--preset " << opt.preset.name << endl;
        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",		opt.help,								L"")
		(L"input,i",	opt.input_file,		wstring(),			L"input file")
		(L"output,o",	opt.output_file,	wstring(),			L"output YUV file")
		(L"preset,p",	opt.preset,			PresetDescriptor(), L"AVBlocks preset")
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

	if (opt.list_presets)
	{
		printPresets();
		return Command;
	}

	if (!validateOptions(opt))
    {
        help(optionsConfig);
        return Error;
    }

	return Parsed;
}
