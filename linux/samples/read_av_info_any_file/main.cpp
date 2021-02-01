/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include <iostream>
#include <string>

//#include <memory.h>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>
#include "AvbUtil.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::error;
using namespace std;

void printVideo(VideoStreamInfo* vsi)
{
	int32_t bitrate = vsi->bitrate();
	BitrateMode::Enum bitrateMode = (BitrateMode::Enum)vsi->bitrateMode();
	primo::codecs::ColorFormat::Enum color = vsi->colorFormat();
	int32_t dar_width  = vsi->displayRatioWidth();
	int32_t dar_height = vsi->displayRatioHeight();
	bool frameBottomUp = vsi->frameBottomUp() == 1;
	int32_t height = vsi->frameHeight();
	int32_t width = vsi->frameWidth();
	double rate = vsi->frameRate();
	ScanType::Enum scanType = vsi->scanType();

	cout << "frame size: " << width << "x" << height << endl;
	cout << "display ratio: " <<  dar_width << ":" << dar_height << endl;
	cout << "frame rate: " << rate << endl;
	cout << "color format: " << getColorFormatName(color) << endl;
	cout << "bitrate: " << bitrate << ", mode: " << getBitrateModeName(bitrateMode) << endl;
	cout << "scan type: " << getScanTypeName(scanType) << endl;
	cout << "frame bottom up: " << frameBottomUp << endl;
	
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

	cout << "sample rate: " << rate << endl;
	cout << "channels: " << channels << endl;
	cout << "bits per sample: " << bitsPerSample << endl;
	cout << "bytes per frame: " << bytesPerFrame << endl;
	cout << "bitrate: " << bitrate << ", mode: " << getBitrateModeName(bitrateMode) << endl;
	cout << "channel layout: " << channelLayout << endl;
	cout << "flags: " << flags << endl;
	
}

void printStreams(MediaInfo * info)
{
	cout << "file: " <<  primo::ustring(info->inputs()->at(0)->file()) << endl;
        
        for (int outSocketIndex = 0; outSocketIndex < info->outputs()->count(); outSocketIndex++)
        {
            MediaSocket * socket = info->outputs()->at(outSocketIndex);

            StreamType::Enum containerType = socket->streamType();
            cout << "container: " << getStreamTypeName(containerType) << endl;

            MediaPinList* pins = socket->pins();
            int32_t streamsCount = pins->count();
            cout << "streams: " << streamsCount << endl;
            cout << endl;

            for (int i=0; i < streamsCount; ++i)
            {
                    StreamInfo* psi = pins->at(i)->streamInfo();

                    MediaType::Enum mediaType = psi->mediaType();
                    cout << "stream #" << i << " " << getMediaTypeName(mediaType) << endl;

                    StreamType::Enum streamType = psi->streamType();
                    cout << "type: " << getStreamTypeName(streamType);
                    StreamSubType::Enum streamSubType = psi->streamSubType();
                    cout << ", subtype: " << getStreamSubTypeName(streamSubType) << endl;

                    int32_t id = psi->ID();
                    cout << "id: " << id << endl;

                    double duration = psi->duration();
                    cout << "duration: " << duration << endl;

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
                            cout << endl;
                    }

                    cout << std::endl;
            }
        }
}

void printError(const primo::error::ErrorInfo* e)
{
	if (ErrorFacility::Success == e->facility())
	{
		cout << "Success";
		
	}
	else
	{
                if (e->message())
                {
                    cout << primo::ustring(e->message()) << " ";
                }
         
                cout << "(facility:" << e->facility() << " error:" << e->code() << ")" << endl;
	}

	cout << endl;
}

bool avInfo(Options& opt)
{
        auto info = primo::make_ref(Library::createMediaInfo());
        
        info->inputs()->at(0)->setFile(primo::ustring(opt.avfile));

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

int main(int argc, char* argv[])
{
        Options opt;
        
        switch(prepareOptions(opt, argc, argv))
	{
		case Command:   return 0;
		case Error:	return 1;
	}
        
        //Library::setLicense("<license-string>");
        Library::initialize();

        bool avInfoResult = avInfo(opt);

	Library::shutdown();

	return avInfoResult ? 0 : 1;
}
