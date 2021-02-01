/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
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
using namespace primo::error;
using namespace std;

void printStatus(const ErrorInfo* e)
{
	if (ErrorFacility::Success == e->facility())
	{
		cout << "Success";
	}
	else
	{
                if (e->message())
                {
                    cout << primo::ustring(e->message()) << " ";
                }
         
                cout << "(facility:" << e->facility() << " error:" << e->code() << ")" << endl;
	}

	cout << endl;
}

primo::ref<MediaSocket> createInputSocket(Options& opt)
{
	auto socket = primo::make_ref(Library::createMediaSocket());
	socket->setStreamType(StreamType::UncompressedVideo);
	socket->setFile(primo::ustring(opt.yuv_file));
	
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
	socket->setFile(primo::ustring(opt.h264_file));
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

bool encode(Options& opt)
{
    // create input socket
    auto inSocket = createInputSocket(opt);

    // create output socket
    auto outSocket = createOutputSocket(opt);

    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    // transcoder will fail if output exists (by design)
    deleteFile(primo::ustring(opt.h264_file));

    cout << "Transcoder open: ";
    if(transcoder->open())
    {
            printStatus(transcoder->error());
            if(!transcoder->run())
                return false;
            
            cout << "Transcoder run: ";
            printStatus(transcoder->error());
            
            transcoder->close();
            cout << "Transcoder close: ";
            printStatus(transcoder->error());
    }
    else
    {
            printStatus(transcoder->error());
            return false;
    }   
    
    return true;
}

int main(int argc, char* argv[])
{
    Options opt;

    switch(prepareOptions( opt, argc, argv))
    {
            case Command: return 0;
            case Error: return 1;
    }

    Library::initialize();
    
    bool encodeResult = encode(opt);

    Library::shutdown();

    return encodeResult ? 0 : 1;
}
