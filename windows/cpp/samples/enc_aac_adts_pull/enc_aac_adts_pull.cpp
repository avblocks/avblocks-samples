/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
    primo::ref<MediaSocket> socket (Library::createMediaSocket());

    socket->setStreamType(StreamType::AAC);
    socket->setStreamSubType(StreamSubType::AAC_ADTS);

    primo::ref<MediaPin> pin (Library::createMediaPin());
    socket->pins()->add(pin.get());

    primo::ref<AudioStreamInfo> asi (Library::createAudioStreamInfo());
    pin->setStreamInfo(asi.get());

    asi->setStreamType(StreamType::AAC);
    asi->setStreamSubType(StreamSubType::AAC_ADTS);

    // You can change the sampling rate and the number of the channels
    //asi->setChannels(1);
    //asi->setSampleRate(44100);

    return socket;
}

bool encode(Options& opt)
{
    primo::ref<MediaSocket> inSocket(Library::createMediaSocket());
    inSocket->setFile(opt.inputFile.c_str());

    // create output socket
    primo::ref<MediaSocket> outSocket (createOutputSocket(opt));

    // create transcoder
    primo::ref<Transcoder> transcoder (Library::createTranscoder());
    transcoder->setAllowDemoMode(TRUE);
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    // transcoder will fail if output exists (by design)
    deleteFile(opt.outputFile.c_str());

    if (!transcoder->open())
    {
        printError(L"Transcoder open", transcoder->error());
        return false;
    }

    int outputIndex = 0;
    primo::ref<MediaSample> sample(Library::createMediaSample());
    std::ofstream outfile(opt.outputFile, ios_base::binary);

    if (!outfile.is_open())
    {
        wcout << L"Could not open file " << opt.outputFile << endl;
        return false;
    }

    while (transcoder->pull(outputIndex, sample.get()))
    {
        outfile.write((const char*)sample->buffer()->data(), sample->buffer()->dataSize());
    }

    bool success = false;
    const primo::error::ErrorInfo* error = transcoder->error();
    printError(L"Transcoder pull", error);

    if ((error->facility() == primo::error::ErrorFacility::Codec) &&
        (error->code() == primo::codecs::CodecError::EOS))
    {
        // ok
        success = true;
    }

    transcoder->close();

    return success;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch(prepareOptions( opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
    }

    Library::initialize();

    bool result = encode(opt);

    Library::shutdown();

    return result ? 0 : 1;
}