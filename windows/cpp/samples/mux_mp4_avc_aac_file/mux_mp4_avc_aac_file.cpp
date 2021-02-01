/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
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

bool MP4Mux(const Options& opt)
{
	deleteFile(opt.output_file.c_str());

	auto transcoder = primo::make_ref(Library::createTranscoder());

	// Transcoder demo mode must be enabled, 
	// in order to use the production release for testing (without a valid license)
	transcoder->setAllowDemoMode(TRUE);

	bool audioStreamDetected = false;
	bool videoStreamDetected = false;

    for(int i = 0; i < (int)opt.input_files.size(); i++)
    {
		auto mediaInfo = primo::make_ref(Library::createMediaInfo());
		mediaInfo->inputs()->at(0)->setFile(opt.input_files[i].c_str());

        if (!mediaInfo->open())
        {
            printError(L"mediaInfo.Open", mediaInfo->error());
            return false;
        }

		auto inputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));

		for(int j = 0; j < inputSocket->pins()->count(); j++)
		{
			MediaPin *pin = inputSocket->pins()->at(j);

			if(pin->streamInfo()->streamType() == StreamType::H264)
			{
				if(videoStreamDetected)
				{
					pin->setConnection(PinConnection::Disabled);
				}
				else
				{
					videoStreamDetected = true;
					wcout << "Muxing video input: " << opt.input_files[i] << std::endl;
				}
			}
			else if(pin->streamInfo()->streamType() == StreamType::AAC)
			{
				if(audioStreamDetected)
				{
					pin->setConnection(PinConnection::Disabled);
				}
				else
				{
					audioStreamDetected = true;
					wcout << "Muxing audio input: " << opt.input_files[i] << std::endl;
				}
			}
			else
			{
				pin->setConnection(PinConnection::Disabled);
			}
		}

        transcoder->inputs()->add(inputSocket.get());
    }

	// Configure output
	{
		auto socket = primo::make_ref(Library::createMediaSocket());
		socket->setFile(opt.output_file.c_str());
		socket->setStreamType(StreamType::MP4);

		if(videoStreamDetected)
		{
			auto streamInfo = primo::make_ref(Library::createVideoStreamInfo());
			streamInfo->setStreamType(StreamType::H264);
			streamInfo->setStreamSubType(StreamSubType::AVC1);

			auto pin = primo::make_ref(Library::createMediaPin());
			pin->setStreamInfo(streamInfo.get());
			socket->pins()->add(pin.get());
		}

		if(audioStreamDetected)
		{
			auto streamInfo = primo::make_ref(Library::createAudioStreamInfo());
			streamInfo->setStreamType(StreamType::AAC);
			streamInfo->setStreamSubType(StreamSubType::AAC_MP4);

			auto pin = primo::make_ref(Library::createMediaPin());
			pin->setStreamInfo(streamInfo.get());
			socket->pins()->add(pin.get());
		}

		if(opt.fast_start)
		{
			socket->params()->addInt(Param::Muxer::MP4::FastStart, 1);
		}

		transcoder->outputs()->add(socket.get());
	}

	bool_t res = transcoder->open();
	printError(L"Open Transcoder", transcoder->error());
	if (!res)
		return false;

	res = transcoder->run();
	printError(L"Run Transcoder", transcoder->error());

	transcoder->close();

	return res ? true : false;
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

	// set your license string
	// primo::avblocks::Library::setLicense("PRIMO-LICENSE");

	bool mp4MuxResult = MP4Mux(opt);

	primo::avblocks::Library::shutdown();

	return mp4MuxResult ? 0 : 1;
}
