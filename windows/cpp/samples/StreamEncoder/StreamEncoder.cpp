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
#include "FileStream.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

void printError(const wchar_t* action, const primo::error::ErrorInfo* e)
{
	if (action)
	{
		wcout << action << L": ";
	}

	if (primo::error::ErrorFacility::Success == e->facility())
	{
		wcout << L"Success" << endl;
		return;
	}

	if (e->message())
	{
		wcout << e->message() << L", ";
	}

	wcout << L"facility:" << e->facility() << L", error:" << e->code() << endl;
}

bool encode(const Options& opt)
{
	deleteFile(opt.output_file.c_str());

	auto inputStream = primo::make_ref(FileStream::create());
	auto outputStream = primo::make_ref(FileStream::create());

	inputStream->setFile(opt.yuv_file.c_str(), FileStream::OpenModeRead);
	outputStream->setFile(opt.output_file.c_str(), FileStream::OpenModeWrite);

	auto transcoder = primo::make_ref(Library::createTranscoder());
	// In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
	transcoder->setAllowDemoMode(TRUE);

	// Configure input
	{
		auto instream = primo::make_ref(Library::createVideoStreamInfo());
		/*
			The input frame rate determines how fast the video is played.
		*/
		instream->setFrameRate(opt.yuv_fps);
		instream->setFrameWidth(opt.yuv_frame.width);
		instream->setFrameHeight(opt.yuv_frame.height);
		instream->setColorFormat(opt.yuv_color.Id);
		instream->setScanType(ScanType::Progressive);
		instream->setStreamType(StreamType::UncompressedVideo);

		auto inpin = primo::make_ref(Library::createMediaPin());
		inpin->setStreamInfo(instream.get());

		auto insocket = primo::make_ref(Library::createMediaSocket());
		insocket->setStream(inputStream.get());
		insocket->setStreamType(StreamType::UncompressedVideo);
		insocket->pins()->add(inpin.get());

		transcoder->inputs()->add(insocket.get());
	}

	// Configure output
	{
		auto outsocket = primo::make_ref(Library::createMediaSocket(opt.preset.name));
		outsocket->setStream(outputStream.get());

		transcoder->outputs()->add(outsocket.get());
	}

	bool_t res = transcoder->open();
	printError(L"Open Transcoder", transcoder->error());
	if (!res)
		return false;

	res = transcoder->run();
	printError(L"Run Transcoder", transcoder->error());
	if (!res)
		return false;

	transcoder->close();

	wcout << L"created video: " << opt.output_file << endl;
	
	return true;
}

int _tmain(int argc, wchar_t* argv[])
{
	Options opt;

	switch(prepareOptions( opt, argc, argv))
	{
		case Command: return 0;
		case Error: return 1;
	}

	primo::avblocks::Library::initialize();
		
	bool encodeResult = encode(opt);

	primo::avblocks::Library::shutdown();

	return encodeResult ? 0 : 1;
}

