/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
// Slideshow.cpp : Defines the entry point for the console application.
//

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

MediaBuffer* createMediaBufferForFile(const wchar_t* filename)
{
	HANDLE h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == h)
		return NULL;

	int32_t imgsize = GetFileSize(h, NULL);

	MediaBuffer* mediaBuffer = Library::createMediaBuffer(imgsize);
	
	DWORD bytesRead;
	ReadFile(h, mediaBuffer->start(), imgsize, &bytesRead, NULL);
	
	mediaBuffer->setData(0, imgsize);
	
	CloseHandle(h);
	
	return mediaBuffer;
}

bool slideshow(Options& opt)
{
	const double inputFramerate = 25.0;
	const int imageCount = 250;
	bool_t res;
	
	wstring imgdir( getExeDir() );
	imgdir.append(L"\\..\\assets\\img");

	wstring outFilename (getExeDir());
	outFilename.append(L"\\cube.");
	outFilename.append(opt.preset.fileExtension);
	DeleteFile(outFilename.c_str());

	auto transcoder = primo::make_ref(Library::createTranscoder());

	// Transcoder demo mode must be enabled, 
	// in order to use the production release for testing (without a valid license)
	transcoder->setAllowDemoMode(TRUE);

	// Load image info
	auto info = primo::make_ref(Library::createMediaInfo());
	{
		wstring firstImage(imgdir);
		firstImage.append(L"\\cube0000.jpeg");

		info->inputs()->at(0)->setFile(firstImage.c_str());

		res = info->open();
		printError(L"Load Info", info->error());
		if (!res)
			return false;
	}

	// Configure input
	{
		auto vinfo = primo::make_ref((VideoStreamInfo*) info->outputs()->at(0)->pins()->at(0)->streamInfo()->clone());
		vinfo->setFrameRate(inputFramerate);

		auto pin = primo::make_ref(Library::createMediaPin());
		pin->setStreamInfo(vinfo.get());

		auto socket = primo::make_ref(Library::createMediaSocket());
		socket->pins()->add(pin.get());

		transcoder->inputs()->add(socket.get());
	}
	

	// Configure output
	{
		auto socket = primo::make_ref(Library::createMediaSocket(opt.preset.name));
		socket->setFile(outFilename.c_str());

		transcoder->outputs()->add(socket.get());
	}

	res = transcoder->open();
	printError(L"Open Transcoder", transcoder->error());
	if (!res)
		return false;

	// Encode images
	auto mediaSample = primo::make_ref(Library::createMediaSample());

	for(int i = 0; i < imageCount; i++)
	{
		WCHAR imgfile[MAX_PATH];
		wsprintf(imgfile, L"%s\\cube%04d.jpeg", imgdir.c_str(), i);
		
		auto buffer = primo::make_ref(createMediaBufferForFile(imgfile));

		mediaSample->setBuffer(buffer.get());
		
		// the correct start time is required by the transcoder
		mediaSample->setStartTime(i / inputFramerate);

		res = transcoder->push(0, mediaSample.get());
		if (!res)
		{
			printError(L"Push Transcoder", transcoder->error());
			return false;
		}
	}

	if(!transcoder->flush())
	{
		printError(L"Flush Transcoder", transcoder->error());
		return false;
	}

	transcoder->close();
	wcout << L"Output video: \"" << outFilename << L"\"" << endl;

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

	bool slideshowResult = slideshow(opt);

	primo::avblocks::Library::shutdown();

	return slideshowResult ? 0 : 1;
}

