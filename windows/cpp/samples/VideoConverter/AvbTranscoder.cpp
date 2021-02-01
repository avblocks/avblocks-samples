/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "stdafx.h"
#include "AvbTranscoder.h"
#include "AvbUtil.h"

#include <sstream>

using namespace primo::avblocks;
using namespace primo;

PresetDescriptor avb_presets[] = {
	
	// custom presets
    {"custom-mp4-h264-704x576-25fps-aac",	false,	"mp4"},
    {"custom-mp4-h264-704x576-12fps-aac",	false,	"mp4"},
    {"custom-mp4-h264-352x288-25fps-aac",	false,	"mp4"},
    {"custom-mp4-h264-352x288-12fps-aac",	false,	"mp4"},

	// video presets
	{ Preset::Video::DVD::PAL_4x3_MP2,		false,	"mpg" },
	{ Preset::Video::DVD::PAL_16x9_MP2,		false,	"mpg" },
	{ Preset::Video::DVD::NTSC_4x3_PCM,		false,	"mpg" },
	{ Preset::Video::DVD::NTSC_4x3_MP2,		false,	"mpg" },
	{ Preset::Video::DVD::NTSC_16x9_PCM,	false,	"mpg" },
	{ Preset::Video::DVD::NTSC_16x9_MP2,	false,	"mpg" },
	{ Preset::Video::AppleTV::H264_480p,	false,	"mp4" },
	{ Preset::Video::AppleTV::H264_720p,	false,	"mp4" },
	{ Preset::Video::AppleTV::MPEG4_480p,	false,	"mp4" },
	{ Preset::Video::AppleLiveStreaming::WiFi_H264_640x480_30p_1200K_AAC_96K, false,	"ts" },
	{ Preset::Video::AppleLiveStreaming::WiFi_Wide_H264_1280x720_30p_4500K_AAC_128K, false,	"ts" },
	{ Preset::Video::Generic::MP4::Base_H264_AAC, false,	"mp4" },
	{ Preset::Video::Fast::MP4::H264_AAC,	false,	"mp4" },
	{ Preset::Video::iPad::H264_576p,		false,	"mp4" },
	{ Preset::Video::iPad::H264_720p,		false,	"mp4" },
	{ Preset::Video::iPad::MPEG4_480p,		false,	"mp4" },
	{ Preset::Video::iPhone::H264_480p,		false,	"mp4" },
	{ Preset::Video::iPhone::MPEG4_480p,	false,	"mp4" },
	{ Preset::Video::iPod::H264_240p,		false,	"mp4" },
	{ Preset::Video::iPod::MPEG4_240p,		false,	"mp4" },
	{ Preset::Video::VCD::PAL,				false,	"mpg" },
	{ Preset::Video::VCD::NTSC,				false,	"mpg" },
	{ Preset::Video::Generic::WebM::Base_VP8_Vorbis, false,	"webm" },
	{ Preset::Video::AndroidPhone::H264_360p, false,	"mp4" },
	{ Preset::Video::AndroidPhone::H264_720p, false,	"mp4" },
	{ Preset::Video::AndroidTablet::H264_720p, false,	"mp4" },
	{ Preset::Video::AndroidTablet::WebM_VP8_720p, false,	"webm" },
			
	// audio presets
	{ Preset::Audio::AudioCD::WAV,							true,	"wav" },
	{ Preset::Audio::Generic::AAC,							true,	"aac" },
	{ Preset::Audio::Generic::M4A::CBR_128kbps,				true,	"m4a" },
	{ Preset::Audio::Generic::M4A::CBR_256kbps,				true,	"m4a" },
	{ Preset::Audio::DVD::MP2,								true,	"mp2" },
	{ Preset::Audio::Generic::MP3::CBR_128kbps,				true,	"mp3" },
	{ Preset::Audio::Generic::MP3::CBR_256kbps,				true,	"mp3" },
	{ Preset::Audio::Generic::WMA::Standard::CBR_128kbps,	true,	"wma" },
	{ Preset::Audio::Generic::WMA::Professional::VBR_Q75,	true,	"wma" },
	{ Preset::Audio::Generic::WMA::Professional::VBR_Q90,	true,	"wma" },
	{ Preset::Audio::Generic::WMA::Lossless::CD,			true,	"wma" },
	{ Preset::Audio::Generic::OggVorbis::VBR_Q4,			true,	"ogg" },
	{ Preset::Audio::Generic::OggVorbis::VBR_Q8,			true,	"ogg" },

	{ NULL,								false,	NULL },
};

bool AvbTranscoder::Convert()
{
	m_errorMessage.clear();

	struct LocalContext {
		MediaInfo*		mediaInfo;
		MediaSocket*	inSocket;
		MediaSocket*	outSocket;
		Transcoder*		avbTranscoder;

		LocalContext() : mediaInfo(NULL), inSocket(NULL), outSocket(NULL), avbTranscoder(NULL) {}

		// cleanup
		~LocalContext() {
			if (mediaInfo)		mediaInfo->release();
			if (inSocket)		inSocket->release();
			if (outSocket)		outSocket->release();
			if (avbTranscoder)	avbTranscoder->release();
		};

	} lc;

	lc.mediaInfo = Library::createMediaInfo();
	lc.mediaInfo->inputs()->at(0)->setFile(m_inputFile.c_str());
	if (!lc.mediaInfo->open())
	{
		FormatErrorMessage(lc.mediaInfo->error());
		return false;
	}

	lc.avbTranscoder = Library::createTranscoder();
	// In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
	lc.avbTranscoder->setAllowDemoMode(TRUE);
	if (m_callback)
	{
		lc.avbTranscoder->setCallback(m_callback);
	}

	lc.inSocket = Library::createMediaSocket(lc.mediaInfo);
	lc.avbTranscoder->inputs()->add( lc.inSocket );

	lc.outSocket = Library::createMediaSocket(m_outputPreset.c_str());
	if (!lc.outSocket)
	{
		m_errorMessage = L"Unknown output preset";
		return false;
	}

	lc.outSocket->setFile(m_outputFile.c_str());

	lc.avbTranscoder->outputs()->add( lc.outSocket );

	if (!lc.avbTranscoder->open())
	{
		FormatErrorMessage(lc.avbTranscoder->error());
		return false;
	}

	if (!lc.avbTranscoder->run())
	{
		FormatErrorMessage(lc.avbTranscoder->error());
		return false;
	}

	return true;
}

void AvbTranscoder::FormatErrorMessage(const primo::error::ErrorInfo* e)
{
	if (primo::error::ErrorFacility::Success == e->facility())
	{
		m_errorMessage = L"Success";
		return;
	}

	std::wstringstream msg;

	if (e->message())
	{
		msg << e->message() << L", ";
	}

	msg << L"(facility:" << e->facility() << L" error:" << e->code() << L")";
	
	m_errorMessage = msg.str();
}