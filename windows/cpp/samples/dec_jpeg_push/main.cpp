/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "util.h"
#include "options.h"


using namespace std;
using namespace primo::codecs;
using namespace primo::avblocks;

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

primo::ref<MediaSocket> createInputSocket(int width, int height)
{
	auto socket = primo::make_ref(Library::createMediaSocket());
	socket->setStreamType(StreamType::JPEG);
	
	auto pin = primo::make_ref(Library::createMediaPin());
	socket->pins()->add(pin.get());

	auto vsi = primo::make_ref(Library::createVideoStreamInfo());
	pin->setStreamInfo(vsi.get());

	vsi->setStreamType(StreamType::JPEG);
	vsi->setScanType(ScanType::Progressive);

	vsi->setFrameWidth(width);
	vsi->setFrameHeight(height);

	return socket;
}

primo::ref<MediaSocket> createOutputSocket(const wchar_t* fileName, int width, int height)
{
	auto socket = primo::make_ref(Library::createMediaSocket());
	socket->setFile(fileName);
	socket->setStreamType(StreamType::UncompressedVideo);

	auto pin = primo::make_ref(Library::createMediaPin());
	socket->pins()->add(pin.get());

	auto vsi = primo::make_ref(Library::createVideoStreamInfo());
	pin->setStreamInfo(vsi.get());

	vsi->setStreamType(StreamType::UncompressedVideo);
	vsi->setScanType(ScanType::Progressive);
	vsi->setColorFormat(ColorFormat::YUV420);

	vsi->setFrameWidth(width);
	vsi->setFrameHeight(height);

	return socket;
}

bool decodeJpegPush(Options& opt)
{
	// program
	vector<uint8_t> inputData = readFileBytes(opt.inputFile.c_str());
	if (inputData.size() <= 0)
	{
		wcout << L"Could not load input file." << endl;
		return false;
	}
	
	// create input socket
	auto inSocket = createInputSocket(opt.frameSize.width, opt.frameSize.height);

	// create output socket
	auto outSocket = createOutputSocket(opt.outputFile.c_str(), opt.frameSize.width, opt.frameSize.height);

	// create transcoder
	auto transcoder = primo::make_ref(Library::createTranscoder());
	transcoder->setAllowDemoMode(TRUE);
	transcoder->inputs()->add(inSocket.get());
	transcoder->outputs()->add(outSocket.get());

	// transcoder will fail if output exists (by design)
	deleteFile(opt.outputFile.c_str());

	if (transcoder->open())
	{
		auto buffer = primo::make_ref(Library::createMediaBuffer());
		buffer->attach(inputData.data(), inputData.size(), TRUE);

		auto sample = primo::make_ref(Library::createMediaSample());
		sample->setBuffer(buffer.get());

		if (!transcoder->push(0, sample.get()))
		{
			printError(transcoder->error());
			return false;
		}

		transcoder->flush();
		transcoder->close();
	}
	else
	{
		printError(transcoder->error());
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
	
	// init AVBlocks
	Library::initialize();
	
	bool decodeResult = decodeJpegPush(opt);

	// cleanup AVBlocks
	Library::shutdown();

	return decodeResult ? 0 : 1;
}
