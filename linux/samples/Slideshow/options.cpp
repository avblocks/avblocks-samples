/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

// needed for memmove in AVBlocks
#include <string.h>
#include <AVBlocks.h>
#include <PrimoReference++.h>
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

PresetDescriptor avb_presets[] = {
	
	// video presets
	{ Preset::Video::DVD::PAL_4x3_MP2,                                                  "mpg" },
	{ Preset::Video::DVD::PAL_16x9_MP2,                                                 "mpg" },
	{ Preset::Video::DVD::NTSC_4x3_MP2,                                                 "mpg" },
	{ Preset::Video::DVD::NTSC_16x9_MP2,                                                "mpg" },
	{ Preset::Video::AppleTV::H264_480p,                                                "mp4" },
	{ Preset::Video::AppleTV::H264_720p,                                                "mp4" },
	{ Preset::Video::AppleTV::MPEG4_480p,                                               "mp4" },
	{ Preset::Video::AppleLiveStreaming::WiFi_H264_640x480_30p_1200K_AAC_96K,           "ts" },
	{ Preset::Video::AppleLiveStreaming::WiFi_Wide_H264_1280x720_30p_4500K_AAC_128K,    "ts" },
	{ Preset::Video::Generic::MP4::Base_H264_AAC,                                       "mp4" },
	{ Preset::Video::iPad::H264_576p,                                                   "mp4" },
	{ Preset::Video::iPad::H264_720p,                                                   "mp4" },
	{ Preset::Video::iPad::MPEG4_480p,                                                  "mp4" },
	{ Preset::Video::iPhone::H264_480p,                                                 "mp4" },
	{ Preset::Video::iPhone::MPEG4_480p,                                                "mp4" },
	{ Preset::Video::iPod::H264_240p,                                                   "mp4" },
	{ Preset::Video::iPod::MPEG4_240p,                                                  "mp4" },
	{ Preset::Video::AndroidPhone::H264_360p,                                           "mp4" },
	{ Preset::Video::AndroidPhone::H264_720p,                                           "mp4" },
	{ Preset::Video::AndroidTablet::H264_720p,                                          "mp4" },
	{ Preset::Video::AndroidTablet::WebM_VP8_720p,                                      "webm" },
	{ Preset::Video::VCD::PAL,                                                          "mpg" },
	{ Preset::Video::VCD::NTSC,                                                         "mpg" },
	{ Preset::Video::Generic::WebM::Base_VP8_Vorbis,                                    "webm" }
};

const int preset_count = sizeof(avb_presets) / sizeof(PresetDescriptor);

void printPreset(const PresetDescriptor& preset)
{
	cout << left << setw(30) <<  preset.name << " ." << preset.fileExtension << endl;
}

void printPresets()
{
	cout << "PRESETS" << endl;
	cout << "-----------------" << endl;
	for_each(avb_presets, avb_presets+preset_count, printPreset);
	cout << endl;
}

PresetDescriptor* getPresetByName(const char* presetName)
{
    for (int i=0; i<preset_count; ++i)
    {
        PresetDescriptor* preset = &avb_presets[i];
        if (0 == strcasecmp(preset->name, presetName))
            return preset;
    }
    return NULL;
}

void help(primo::program_options::OptionsConfig<char>& optcfg)
{
    cout << "Usage: Slideshow [-p PRESET]\n" << endl;
    printPresets();
    //primo::program_options::doHelp(cout, optcfg);
}

void setDefaultOptions(Options& opt)
{
    opt.preset = *getPresetByName(Preset::Video::iPad::H264_720p);
}

bool validateOptions(Options& opt)
{
    return (opt.preset.name != NULL && opt.preset.fileExtension != NULL);    
}

ErrorCodes prepareOptions(Options &opt, int argc, char* argv[])
{
    if (argc < 2)
    {
        setDefaultOptions(opt);
        cout << "Using defaults:\n";
        cout << " --preset " << opt.preset.name;
        cout << endl;
        return Parsed;
    }
    
    primo::program_options::OptionsConfig<char> optcfg;
    optcfg.addOptions()
            ("help,h",  opt.help,       "")
            ("preset,p", opt.preset,PresetDescriptor(), "output preset.");
       
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
    
    if (opt.help )
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

std::istringstream &operator>>(std::istringstream &in, PresetDescriptor &preset)
{
	std::string presetName;
	in >> presetName;

	PresetDescriptor* presetDesc = getPresetByName(presetName.c_str());
	if(!presetDesc)
		throw primo::program_options::ParseFailure<char>("", presetName, "Parse error");

	preset = *presetDesc;
	return in;
}
