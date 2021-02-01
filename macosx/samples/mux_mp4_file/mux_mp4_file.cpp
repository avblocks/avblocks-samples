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

bool MP4Mux(const Options& opt)
{
    deleteFile(primo::ustring(opt.output_file));

    primo::ref<Transcoder> transcoder (Library::createTranscoder());

    // Transcoder demo mode must be enabled, 
    // in order to use the production release for testing (without a valid license)
    transcoder->setAllowDemoMode(true);

    primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
    outputSocket->setFile(primo::ustring(opt.output_file));
    outputSocket->setStreamType(StreamType::MP4);

    // audio
    for(int i = 0; i < (int)opt.input_audio.size(); i++)
    {
        primo::ref<MediaPin> outputPin(Library::createMediaPin());
        primo::ref<AudioStreamInfo> asi(Library::createAudioStreamInfo());
        asi->setStreamType(StreamType::AAC);
        outputPin->setStreamInfo(asi.get());

        outputSocket->pins()->add(outputPin.get());

        primo::ref<MediaSocket> inputSocket(Library::createMediaSocket());
        inputSocket->setFile(primo::ustring(opt.input_audio[i]));
        inputSocket->setStreamType(StreamType::MP4);
        transcoder->inputs()->add(inputSocket.get());

        cout << "Muxing audio input: " << opt.input_audio[i] << endl;
    }

    // video
    for (int i = 0; i < (int)opt.input_video.size(); i++)
    {
        primo::ref<MediaPin> outputPin(Library::createMediaPin());
        primo::ref<VideoStreamInfo> vsi(Library::createVideoStreamInfo());
        vsi->setStreamType(StreamType::H264);
        outputPin->setStreamInfo(vsi.get());

        outputSocket->pins()->add(outputPin.get());

        primo::ref<MediaSocket> inputSocket(Library::createMediaSocket());
        inputSocket->setFile(primo::ustring(opt.input_video[i]));
        inputSocket->setStreamType(StreamType::MP4);
        transcoder->inputs()->add(inputSocket.get());

        cout << "Muxing video input: " << opt.input_video[i] << endl;
    }

    transcoder->outputs()->add(outputSocket.get());

    if (!transcoder->open())
    {
        printError("Open Transcoder", transcoder->error());
        return false;
    }

    if (!transcoder->run())
    {
        printError("Run Transcoder", transcoder->error());
        return false;
    }

    transcoder->close();
        
    cout << "Output file: " << opt.output_file << endl;
        
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

    primo::avblocks::Library::initialize();

    // set your license string
    // primo::avblocks::Library::setLicense("PRIMO-LICENSE");

    bool mp4MuxResult = MP4Mux(opt);

    primo::avblocks::Library::shutdown();

    return mp4MuxResult ? 0 : 1;
}

