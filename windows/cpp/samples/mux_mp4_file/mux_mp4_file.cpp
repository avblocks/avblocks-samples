/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
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

bool MP4Mux(const Options& opt)
{
    deleteFile(opt.output_file.c_str());

    primo::ref<Transcoder> transcoder (Library::createTranscoder());

    // Transcoder demo mode must be enabled, 
    // in order to use the production release for testing (without a valid license)
    transcoder->setAllowDemoMode(TRUE);

    primo::ref<MediaSocket> outputSocket(Library::createMediaSocket());
    outputSocket->setFile(opt.output_file.c_str());
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
        inputSocket->setFile(opt.input_audio[i].c_str());
        inputSocket->setStreamType(StreamType::MP4);
        transcoder->inputs()->add(inputSocket.get());

        wcout << "Muxing audio input: " << opt.input_audio[i] << endl;
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
        inputSocket->setFile(opt.input_video[i].c_str());
        inputSocket->setStreamType(StreamType::MP4);
        transcoder->inputs()->add(inputSocket.get());

        wcout << "Muxing video input: " << opt.input_video[i] << endl;
    }

    transcoder->outputs()->add(outputSocket.get());

    if (!transcoder->open())
    {
        printError(L"Open Transcoder", transcoder->error());
        return false;
    }
    
    if (!transcoder->run())
    {
        printError(L"Run Transcoder", transcoder->error());
        return false;
    }

    transcoder->close();

    wcout << "Output file: " << opt.output_file << endl;
        
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

    // set your license string
    // primo::avblocks::Library::setLicense("PRIMO-LICENSE");

    bool mp4MuxResult = MP4Mux(opt);

    primo::avblocks::Library::shutdown();

    return mp4MuxResult ? 0 : 1;
}
