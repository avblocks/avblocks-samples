/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "AvbUtil.h"
#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

void printVideo(VideoStreamInfo* vsi)
{
	int32_t bitrate = vsi->bitrate();

	BitrateMode::Enum bitrateMode = (BitrateMode::Enum)vsi->bitrateMode();
	
	primo::codecs::ColorFormat::Enum color = vsi->colorFormat();
	
	int32_t dar_width  = vsi->displayRatioWidth();
	int32_t dar_height = vsi->displayRatioHeight();
	
	bool frameBottomUp = vsi->frameBottomUp() == TRUE;
	int32_t height = vsi->frameHeight();
	int32_t width = vsi->frameWidth();
	double rate = vsi->frameRate();
	
	ScanType::Enum scanType = vsi->scanType();

	wcout << L"bitrate: " << bitrate << L", mode: " << getBitrateModeName(bitrateMode) << endl;

	wcout << L"color format: " << getColorFormatName(color) << endl;

	wcout << L"display ratio: " <<  dar_width << L":" << dar_height << endl;

	wcout << L"frame bottom up: " << frameBottomUp << endl;
	wcout << L"frame size: " << width << L"x" << height << endl;
	wcout << L"frame rate: " << rate << endl;

	wcout << L"scan type: " << getScanTypeName(scanType) << endl;
}

void printAudio(AudioStreamInfo* asi)
{
	uint32_t bitrate = asi->bitrate();
	BitrateMode::Enum bitrateMode = (BitrateMode::Enum)asi->bitrateMode();

	uint32_t bitsPerSample = asi->bitsPerSample();
	uint32_t bytesPerFrame = asi->bytesPerFrame();
	
	uint32_t channelLayout = asi->channelLayout();
	uint32_t channels = asi->channels();
	
	uint32_t flags = asi->pcmFlags();
	
	uint32_t rate = asi->sampleRate();

	wcout << L"bitrate: " << bitrate << L", mode: " << getBitrateModeName(bitrateMode) << endl;

	wcout << L"bits per sample: " << bitsPerSample << endl;
	wcout << L"bytes per frame: " << bytesPerFrame << endl;
	
	wcout << L"channel layout: " << channelLayout << endl;
	wcout << L"channels: " << channels << endl;

	wcout << L"flags: " << flags << endl;
	
	wcout << L"sample rate: " << rate << endl;
}

void printStreams(MediaInfo* info)
{
	stdout_utf16 mode;
	
	wcout << L"file: " <<  info->inputs()->at(0)->file() << endl;

    for (int outSocketIndex = 0; outSocketIndex < info->outputs()->count(); outSocketIndex++)
    {
        MediaSocket * socket = info->outputs()->at(outSocketIndex);

        StreamType::Enum containerType = socket->streamType();
        wcout << L"container: " << getStreamTypeName(containerType) << endl;

        MediaPinList* pins = socket->pins();
        int32_t streamsCount = pins->count();
        wcout << L"streams: " << streamsCount << endl;
        wcout << endl;

        for (int i = 0; i < streamsCount; ++i)
        {
            StreamInfo* psi = pins->at(i)->streamInfo();

            MediaType::Enum mediaType = psi->mediaType();
            wcout << L"stream #" << i << " " << getMediaTypeName(mediaType) << endl;

            StreamType::Enum streamType = psi->streamType();
            wcout << L"type: " << getStreamTypeName(streamType);
            StreamSubType::Enum streamSubType = psi->streamSubType();
            wcout << L", subtype: " << getStreamSubTypeName(streamSubType) << endl;

            int32_t id = psi->ID();
            wcout << L"id: " << id << endl;

            double duration = psi->duration();
            wcout << L"duration: " << duration << endl;

            if (MediaType::Video == mediaType)
            {
                VideoStreamInfo* vsi = static_cast<VideoStreamInfo*>(psi);
                printVideo(vsi);
            }
            else if (MediaType::Audio == mediaType)
            {
                AudioStreamInfo* asi = static_cast<AudioStreamInfo*>(psi);
                printAudio(asi);
            }
            else
            {
                wcout << endl;
            }

            wcout << std::endl;
        }
    }
}

void printError(const primo::error::ErrorInfo* e)
{
	if (primo::error::ErrorFacility::Success == e->facility())
	{
		wcout << L"Success";
	}
	else
	{
		wcout << L"facility: " << e->facility() << L", error: " << e->code();

		if (e->message())
		{
			wcout << L", " << e->message();
		}

		if (e->hint())
		{
			wcout << L", " << e->hint();
		}
	}

	wcout << endl;
}

bool avInfo(Options& opt)
{
	auto info = primo::make_ref(Library::createMediaInfo());

	info->inputs()->at(0)->setFile(opt.avfile.c_str());

	if(info->open())
	{
		printStreams(info.get());
		return true;
	}
	else
	{
		printError(info->error());
		return false;
	}
}

int wmain(int argc, wchar_t* argv[])
{
	CoInitializeEx(0, COINIT_MULTITHREADED);

	Options opt;

	switch(prepareOptions(opt, argc, argv))
	{
		case Command: return 0;
		case Error:	return 1;
	}

	Library::initialize();

	bool avInfoResult = avInfo(opt);

	Library::shutdown();
	CoUninitialize();

	return avInfoResult ? 0 : 1;
}
