/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
// ReEncode.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include "options.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;
using namespace primo::error;

void printError(const wchar_t* action, const primo::error::ErrorInfo* e)
{
	if (action)
	{
		wcout << action << L": ";
	}

	if (ErrorFacility::Success == e->facility())
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

bool reEncode(Options opt)
{
	wcout << L"Input file: " << opt.inputFile << endl;
	wcout << L"Output file: " << opt.outputFile << endl;
	wcout << L"Re-encode audio forced: " << opt.reEncodeAudio << endl;
	wcout << L"Re-encode video forced: " << opt.reEncodeVideo << endl;

	DeleteFile(opt.outputFile.c_str());

	auto transcoder = primo::make_ref(Library::createTranscoder());

	// In order to use the production release for testing (without a valid license), 
	// the demo mode must be enabled.
	transcoder->setAllowDemoMode(1);

    // configure input
    {
        auto mediaInfo = primo::make_ref(Library::createMediaInfo());

        mediaInfo->inputs()->at(0)->setFile(opt.inputFile.c_str());

        if (!mediaInfo->open())
        {
            printError(L"Open Info", mediaInfo->error());
            return false;
        }

        // Add Inputs
        {
            auto socket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));
            transcoder->inputs()->add(socket.get());
        }
    }

	// Add Outputs
	{
		// Add output socket
		auto socket = primo::make_ref(Library::createMediaSocket());
		socket->setStreamType(transcoder->inputs()->at(0)->streamType());
		socket->setFile(opt.outputFile.c_str());

		// Add output pins and set the ReEncode parameter to Use::On
        for (int i = 0; i < transcoder->inputs()->count(); i++)
        {
            auto inSocket = transcoder->inputs()->at(i);
            for (int j = 0; j < inSocket->pins()->count(); j++)
            {
                auto psi = primo::make_ref(inSocket->pins()->at(j)->streamInfo()->clone());

                auto pin = primo::make_ref(Library::createMediaPin());
                pin->setStreamInfo(psi.get());

                if ((MediaType::Video == psi->mediaType()) && opt.reEncodeVideo)
                {
                    pin->params()->addInt(Param::ReEncode, Use::On);
                }

                if ((MediaType::Audio == psi->mediaType()) && opt.reEncodeAudio)
                {
                    pin->params()->addInt(Param::ReEncode, Use::On);
                }

                socket->pins()->add(pin.get());
            }
        }

		transcoder->outputs()->add(socket.get());
	}

	bool_t res = transcoder->open();
	printError(L"Open Transcoder", transcoder->error());
	if (!res)
		return false;

	res = transcoder->run();
	if (!res)
		return false;

	printError(L"Run Transcoder", transcoder->error());

	transcoder->close();

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

	CoInitialize(NULL);
	primo::avblocks::Library::initialize();

	// set your license string
	// primo::avblocks::Library::setLicense("PRIMO-LICENSE");

	bool reEncodeResult = reEncode(opt);

	primo::avblocks::Library::shutdown();
	CoUninitialize();

	return reEncodeResult ? 0 : 1;
}
