/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;


class stdout_utf16
{
public:
	stdout_utf16()
	{
		// change stdout to Unicode. Cyrillic and Ideographic characters will appear in the console (console font is unicode).
		_setmode(_fileno(stdout), _O_U16TEXT);
	}

	~stdout_utf16()
	{
		// restore ANSI mode
		_setmode(_fileno(stdout), _O_TEXT); 
	}
};

void printStatus(const primo::error::ErrorInfo* e)
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

primo::ref<MediaSocket> createInputSocket(Options& opt)
{
	auto socket = primo::make_ref(Library::createMediaSocket());
	socket->setStreamType(StreamType::UncompressedVideo);
	socket->setFile(NULL);
	socket->setStream(NULL);
	
	auto pin = primo::make_ref(Library::createMediaPin());
	socket->pins()->add(pin.get());

	auto vsi = primo::make_ref(Library::createVideoStreamInfo());
	pin->setStreamInfo(vsi.get());

	vsi->setStreamType(StreamType::UncompressedVideo);
	vsi->setFrameWidth(opt.frame_size.width_);
	vsi->setFrameHeight(opt.frame_size.height_);
	vsi->setColorFormat(opt.yuv_color.Id);
	vsi->setFrameRate(opt.fps);
	vsi->setScanType(ScanType::Progressive);

	return socket;
}

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
	auto socket = primo::make_ref(Library::createMediaSocket());
	socket->setFile(opt.h264_file.c_str());
	socket->setStreamType(StreamType::H264);
	socket->setStreamSubType(StreamSubType::AVC_Annex_B);

	auto pin = primo::make_ref(Library::createMediaPin());
	socket->pins()->add(pin.get());

	auto vsi = primo::make_ref(Library::createVideoStreamInfo());
	pin->setStreamInfo(vsi.get());

	vsi->setStreamType(StreamType::H264);
	vsi->setStreamSubType(StreamSubType::AVC_Annex_B);

	return socket;
}

bool encode_h264_stream(Options& opt)
{
	// create input socket
	auto inSocket = createInputSocket(opt);

	// create output socket
	auto outSocket = createOutputSocket(opt);

	// create transcoder
	auto transcoder = primo::make_ref(Library::createTranscoder());
	transcoder->setAllowDemoMode(TRUE);
	transcoder->inputs()->add(inSocket.get());
	transcoder->outputs()->add(outSocket.get());

	// transcoder will fail if output exists (by design)
	deleteFile(opt.h264_file.c_str());

	wcout << L"Transcoder open: ";
	if(transcoder->open())
	{
		printStatus(transcoder->error());
		
		// simulate input stream
		ifstream inStream;
		inStream.open(opt.yuv_file.c_str(), ifstream::binary);

		if(!inStream.is_open())
			return false;

		auto mediaSample = primo::make_ref(Library::createMediaSample());

		// pushed data need to be a frame
		int videoBufferSize = mediaSample->videoBufferSizeInBytes(opt.frame_size.width_, opt.frame_size.height_, opt.yuv_color.Id);

		if(videoBufferSize <= 0)
			return false;

		auto mediaBuffer = primo::make_ref(Library::createMediaBuffer(videoBufferSize));
		mediaSample->setBuffer(mediaBuffer.get());
		int inputIndex = 0;

		while(true)
		{
			if (inStream.read((char *)mediaBuffer->start(), videoBufferSize))
			{
				mediaBuffer->setData( 0, videoBufferSize);

				if(!transcoder->push(inputIndex, mediaSample.get()))
				{
					wcout << L"Transcoder push: ";
					printStatus(transcoder->error());
					return false;
				}
			}
			else
			{
				if(!transcoder->flush())
					return false;

				wcout << L"Transcoder flush: ";
				printStatus(transcoder->error());
				break;
			}
		}

		inStream.close();

		transcoder->close();
		wcout << L"Transcoder close: ";
		printStatus(transcoder->error());
	}
	else
	{
		printStatus(transcoder->error());
		return false;
	}

    return true;
}

int wmain(int argc, wchar_t* argv[])
{
	Options opt;

	switch(prepareOptions( opt, argc, argv))
	{
		case Command: return 0;
		case Error: return 1;
	}

	Library::initialize();

	bool encodeResult = encode_h264_stream(opt);

	Library::shutdown();

	return encodeResult ? 0 : 1;
}
