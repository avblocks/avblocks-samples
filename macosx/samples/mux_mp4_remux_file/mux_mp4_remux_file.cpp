/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include <iostream>
#include <string>
#include <unistd.h>

#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>

#include "options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;
using namespace primo::error;

namespace pe = primo::error;

void printError(const char* action, const pe::ErrorInfo* e)
{
    if (action)
    {
        cout << action << ": ";
    }

    if (pe::ErrorFacility::Success == e->facility())
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

bool MP4Remux(const Options& opt)
{
    deleteFile(opt.output_file.c_str());
    
    auto transcoder = primo::make_ref(Library::createTranscoder());

    // Transcoder demo mode must be enabled, 
    // in order to use the production release for testing (without a valid license)
    transcoder->setAllowDemoMode(1);

    bool audioStreamDetected = false;
    bool videoStreamDetected = false;

    auto mediaInfo = primo::make_ref(Library::createMediaInfo());
    mediaInfo->inputs()->at(0)->setFile(primo::ustring(opt.input_file));

    if (!mediaInfo->open())
    {
        printError("mediaInfo.Open", mediaInfo->error());
        return false;
    }

    auto inputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));
    mediaInfo->close();

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
                cout << "Muxing video input: " << opt.input_file << std::endl;
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
                cout << "Muxing audio input: " << opt.input_file << std::endl;
            }
        }
        else
        {
            pin->setConnection(PinConnection::Disabled);
        }
    }

    transcoder->inputs()->add(inputSocket.get());

    // Configure output
    {
        auto socket = primo::make_ref(Library::createMediaSocket());
        socket->setFile(primo::ustring(opt.output_file));
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
    printError("Open Transcoder", transcoder->error());
    if (!res)
        return false;

    res = transcoder->run();
    printError("Run Transcoder", transcoder->error());

    transcoder->close();

    return res ? true : false;
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

    bool mp4MuxResult = MP4Remux(opt);

    primo::avblocks::Library::shutdown();

    return mp4MuxResult ? 0 : 1;
}

