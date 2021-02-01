/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include <memory.h>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>
#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;

void printError(const primo::error::ErrorInfo* e)
{
	using namespace std;

	if (primo::error::ErrorFacility::Success == e->facility())
	{
		cout << "Success";
	}
	else
	{
		cout << "facility: " << e->facility() << ", error: " << e->code();

		if (e->message())
		{
			cout << ", " << e->message();
		}

		if (e->hint())
		{
			cout << ", " << e->hint();
		}
	}

	cout << endl;
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

primo::ref<MediaSocket> createOutputSocket(primo::ustring fileName, int width, int height)
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
        vector<uint8_t> inputData = readFileBytes(opt.inputFile.c_str());
	if (inputData.size() <= 0)
	{
		std::cout << "Could not load input file." << std::endl;
		return false;
	}
        
        auto frameWidth = opt.frameSize.width;
        auto frameHeight = opt.frameSize.height;

        // create input socket
        auto inSocket = createInputSocket(frameWidth, frameHeight);

        // create output socket
        auto outSocket = createOutputSocket(opt.outputFile, frameWidth, frameHeight);

        // create transcoder
        auto transcoder = primo::make_ref(Library::createTranscoder());
        transcoder->setAllowDemoMode(true);
        transcoder->inputs()->add(inSocket.get());
        transcoder->outputs()->add(outSocket.get());

        // transcoder will fail if output exists (by design)
        deleteFile(opt.outputFile.c_str());

        if (transcoder->open())
        {
                auto buffer = primo::make_ref(Library::createMediaBuffer());
                buffer->attach(inputData.data(), inputData.size(), true);

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

int main(int argc, char* argv[])
{
	using namespace std;

	Options opt;
        switch(prepareOptions( opt, argc, argv))
	{
		case Command: return 0;
		case Error: return 1;
	}

	// init AVBlocks
	Library::initialize();
	
        bool decodeJpegPushResult = decodeJpegPush(opt);

	// cleanup AVBlocks
	Library::shutdown();

	return decodeJpegPushResult ? 0 : 1;
}
