/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
// ReEncode.cpp : Defines the entry point for the console application.


#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <libgen.h>
#include <AVBlocks.h>

#include <PrimoReference++.h>
#include <PrimoUString.h>
#include "options.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

void printError(const char* action, const primo::error::ErrorInfo* e)
{
	if (action)
	{
		cout << action << ": ";
	}

    if (primo::error::ErrorFacility::Success == e->facility())
	{
		cout << "Success" << endl;
		return;
	}
	
	if (e->message())
	{
		cout << primo::ustring(e->message()) << " ";
	}

	cout << "facility:" << e->facility() << " error:" << e->code() << "" << endl;
}

bool reEncode(Options opt)
{
    cout << "Input file: " << opt.inputFile << endl;
    cout << "Output file: " << opt.outputFile << endl;
    cout << "Re-encode audio forced: " << opt.reEncodeAudio << endl;
    cout << "Re-encode video forced: " << opt.reEncodeVideo << endl;
    
	remove(opt.outputFile.c_str());

	auto transcoder = primo::make_ref(Library::createTranscoder());
	// In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
	transcoder->setAllowDemoMode(1);

    // configure input
    {
        auto mediaInfo = primo::make_ref(Library::createMediaInfo());

        mediaInfo->inputs()->at(0)->setFile(primo::ustring(opt.inputFile));

        if (!mediaInfo->open())
        {
            printError("Open Info", mediaInfo->error());
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
        socket->setFile(primo::ustring(opt.outputFile));

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
	printError("Open Transcoder", transcoder->error());
	if (!res)
		return false;

	res = transcoder->run();
	if (!res)
		return false;

	printError("Run Transcoder", transcoder->error());

	transcoder->close();

	return true;
}

int main(int argc, char* argv[])
{
    Options opt;
    
    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:	return 1;
    }

	primo::avblocks::Library::initialize();

	// set your license string
	// primo::avblocks::Library::setLicense("PRIMO-LICENSE");
    
    bool reEncodeResult = reEncode(opt);
    
	primo::avblocks::Library::shutdown();

    return reEncodeResult ? 0 : 1;
}
