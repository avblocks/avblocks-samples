/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
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
using namespace primo::avblocks;

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

void printPreset(const PresetDescriptor& preset)
{
	wcout << left << setw(45) <<  preset.name << L" ." << preset.fileExtension << endl;
}

void printPresets()
{
	wcout << L"PRESETS" << endl;
	wcout << L"----------------------------------" << endl;
	for_each(begin(avb_presets), end(avb_presets), printPreset);
	wcout << endl;
}

template<typename T>
PresetDescriptor* getPresetByName(const T* presetName)
{
	auto preset = 
		find_if(begin(avb_presets), end(avb_presets), 
		[presetName] (const PresetDescriptor& preset) {
			return 0 == compareNoCase(preset.name, presetName);
		});

	if (preset != end(avb_presets))
		return preset;

	return NULL;
}

void help(OptionsConfig<wchar_t>& optionsConfig)
{
	wcout << endl << L"Usage: Slideshow -p PRESET\n" << endl;
	doHelp(wcout, optionsConfig);
	wcout << endl;
	printPresets();
}

inline std::wistringstream &operator>>(std::wistringstream &in, string &obj)
{
	wstring preset;
    in >> preset;
	
	obj.assign(preset.begin(), preset.end());

    return in;
}

bool validateOptions(Options& opt)
{
    return (opt.preset.name != NULL && opt.preset.fileExtension != NULL);    
}

void setDefaultOptions(Options& opt)
{
    opt.preset = *getPresetByName(Preset::Video::iPad::H264_720p);
}

ErrorCodes prepareOptions(Options& opt, int argc, wchar_t* argv[])
{
	if (argc < 2)
    {
        setDefaultOptions(opt);
        wcout << L"Using defaults:\n";
        wcout << L"--preset " << opt.preset.name;
        wcout << endl;
        return Parsed;
    }

	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(L"help,?",		opt.help,						L"")
		(L"preset,p",	opt.preset,	PresetDescriptor(), L"preset");

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

	if (!validateOptions(opt))
    {
        help(optionsConfig);
        return Error;
    }

	return Parsed;
}

std::wistringstream &operator>>(std::wistringstream &in, PresetDescriptor &preset)
{
	std::wstring presetName;
	in >> presetName;

	PresetDescriptor* presetDesc = getPresetByName(presetName.c_str());
	if(!presetDesc)
		throw primo::program_options::ParseFailure<wchar_t>(L"", presetName, L"Parse error");

	preset = *presetDesc;
	return in;
}