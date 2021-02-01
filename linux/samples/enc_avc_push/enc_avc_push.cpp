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

void printStatus(const primo::error::ErrorInfo* e)
{
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

bool encode_h264_stream(Options& opt)
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
            
        // simulate input stream
	ifstream inStream;
	inStream.open(opt.yuv_file.c_str(), ifstream::binary);
        
        if(!inStream.is_open())
            return false;

	auto mediaSample = primo::make_ref(Library::createMediaSample());
        
        // pushed data need to be a frame
	int videoBufferSize = mediaSample->videoBufferSizeInBytes(opt.frame_size.width_, opt.frame_size.height_, opt.yuv_color.Id);
        
        if (videoBufferSize <= 0)
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
				cout << "Transcoder push: ";
				printStatus(transcoder->error());
				return false;
			}
		}
		else
		{
                        if(!transcoder->flush())
                            return false;
                        
                        cout << "Transcoder flush: ";
                        printStatus(transcoder->error());
                        break;
		}
	}

	inStream.close();
                      
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
    
    bool encodeResult = encode_h264_stream(opt);    
    
    Library::shutdown();

    return encodeResult ? 0 : 1;
}
