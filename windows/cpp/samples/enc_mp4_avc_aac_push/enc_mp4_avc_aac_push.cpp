/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

bool_t av_encode(	MediaSocket* vinput,	// uncompressed video input format
			const wstring& vfile,	// uncompressed video input file
			MediaSocket* ainput,	// uncompressed audio input format
			const wstring& afile,	// uncompressed audio input file
			MediaSocket* output		// output av format
			);

bool_t mp4Mix()
{
	wstring videoInputFile ( getExeDir() + L"\\..\\assets\\vid\\avsync_240x180_29.97fps.yuv" );
	wstring audioInputFile ( getExeDir() + L"\\..\\assets\\aud\\avsync_44100_s16_2ch.pcm" );
	wstring outputFile ( L"avsync.mp4" );

	deleteFile(outputFile.c_str());

	// Video Input
	auto vinput = primo::make_ref(primo::avblocks::Library::createMediaSocket());

	{
		auto pin = primo::make_ref(primo::avblocks::Library::createMediaPin());
		auto vinformat = primo::make_ref(primo::avblocks::Library::createVideoStreamInfo());
		vinformat->setFrameRate(29.97);
		vinformat->setFrameWidth(240);
		vinformat->setFrameHeight(180);
		vinformat->setColorFormat(ColorFormat::YUV420);
		vinformat->setScanType(ScanType::Progressive);
		vinformat->setStreamType(StreamType::UncompressedVideo);
		pin->setStreamInfo(vinformat.get());
		vinput->pins()->add(pin.get());
	}

	// Audio Input
	auto ainput = primo::make_ref(primo::avblocks::Library::createMediaSocket());
	{
		auto pin = primo::make_ref(primo::avblocks::Library::createMediaPin());
		auto ainformat = primo::make_ref(primo::avblocks::Library::createAudioStreamInfo());
		ainformat->setStreamType(StreamType::LPCM);
		ainformat->setBitsPerSample(16);
		ainformat->setChannels(2);
		ainformat->setSampleRate(44100);
		ainformat->setBytesPerFrame(4);
		pin->setStreamInfo(ainformat.get());
		ainput->pins()->add(pin.get());
	}

	// Output
	auto output = primo::make_ref(primo::avblocks::Library::createMediaSocket());
	{
		// Video Pin
		{
			auto pin = primo::make_ref(primo::avblocks::Library::createMediaPin());
			auto voutformat = primo::make_ref(primo::avblocks::Library::createVideoStreamInfo());
			voutformat->setBitrate(4 * 1000 * 1000);
			voutformat->setStreamType(StreamType::H264);
			voutformat->setStreamSubType(StreamSubType::AVC1);
			pin->setStreamInfo(voutformat.get());
			output->pins()->add(pin.get());
		}

		// Audio Pin
		{
			auto pin = primo::make_ref(primo::avblocks::Library::createMediaPin());
			auto aoutformat = primo::make_ref(primo::avblocks::Library::createAudioStreamInfo());
			aoutformat->setStreamType(StreamType::AAC);
			aoutformat->setSampleRate(48000);
			aoutformat->setBitrate(128000);
			pin->setStreamInfo(aoutformat.get());
			output->pins()->add(pin.get());
		}

		output->setStreamType(StreamType::MP4);
		output->setFile(outputFile.c_str());
	}

	return av_encode(vinput.get(), videoInputFile, ainput.get(), audioInputFile, output.get());
}

int _tmain(int argc, wchar_t* argv[])
{
	primo::avblocks::Library::initialize();
	
	bool_t mp4MixResult = mp4Mix();

	primo::avblocks::Library::shutdown();

	return mp4MixResult ? 0 : 1;
}
