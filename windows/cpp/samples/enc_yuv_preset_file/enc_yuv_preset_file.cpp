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

	auto transcoder = primo::make_ref(Library::createTranscoder());
	
	// Transcoder demo mode must be enabled, 
	// in order to use the production release for testing (without a valid license)
	transcoder->setAllowDemoMode(TRUE);

	// Configure input
	// The input stream frame rate determines the playback speed
	{
		auto instream = primo::make_ref(Library::createVideoStreamInfo());
		instream->setStreamType(StreamType::UncompressedVideo);
		instream->setFrameRate(opt.yuv_fps);
		instream->setFrameWidth(opt.yuv_frame.width);
		instream->setFrameHeight(opt.yuv_frame.height);
		instream->setColorFormat(opt.yuv_color.Id);
		instream->setScanType(ScanType::Progressive);

		auto inpin = primo::make_ref(Library::createMediaPin());
		inpin->setStreamInfo(instream.get());

		auto insocket = primo::make_ref(Library::createMediaSocket());
		insocket->setStreamType(StreamType::UncompressedVideo);
		insocket->setFile(opt.yuv_file.c_str());
		insocket->pins()->add(inpin.get());

		transcoder->inputs()->add(insocket.get());
	}

	// Configure output
	{
		auto outsocket = primo::make_ref(Library::createMediaSocket(opt.preset.name));
		outsocket->setFile(opt.output_file.c_str());

		transcoder->outputs()->add(outsocket.get());
	}

	bool_t res = transcoder->open();
	printError(L"Open Transcoder", transcoder->error());
	if (!res)
		return false;

	res = transcoder->run();
	printError(L"Run Transcoder", transcoder->error());
	if (!res)
	{
		transcoder->close();
		return false;
	}

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
