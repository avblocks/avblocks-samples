/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace primo::error;
using namespace std;

bool decodeH264Stream(Options& opt)
{
    // Create an input socket from file
    primo::ref<MediaSocket> inSocket(Library::createMediaSocket());
    inSocket->setFile(primo::ustring(opt.inputFile));

    // Create an output socket with one YUV 4:2:0 video pin
    primo::ref<VideoStreamInfo> outStreamInfo(Library::createVideoStreamInfo());
    outStreamInfo->setStreamType(StreamType::UncompressedVideo);
    outStreamInfo->setColorFormat(ColorFormat::YUV420);
    outStreamInfo->setScanType(ScanType::Progressive);

    primo::ref<MediaPin> outPin(Library::createMediaPin());
    outPin->setStreamInfo(outStreamInfo.get());

    primo::ref<MediaSocket> outSocket(Library::createMediaSocket());
    outSocket->setStreamType(StreamType::UncompressedVideo);

    outSocket->pins()->add(outPin.get());

    // Create Transcoder
    primo::ref<Transcoder> transcoder(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    if (!transcoder->open())
    {
        printError("Transcoder open", transcoder->error());
        return false;
    }
    
    deleteFile(opt.outputFile.c_str());

    int32_t outputIndex = 0;
    primo::ref<MediaSample> yuvFrame(Library::createMediaSample());
    std::ofstream outfile(opt.outputFile, ios_base::binary);

    if (!outfile.is_open())
    {
        cout << "Could not open file " << opt.outputFile << endl;
        return false;
    }

    int frameCounter = 0;
    while (transcoder->pull(outputIndex, yuvFrame.get()))
    {
        // Each call to Transcoder::pull returns a raw YUV 4:2:0 frame. 
        outfile.write((const char*)yuvFrame->buffer()->data(), yuvFrame->buffer()->dataSize());
        ++frameCounter;
    }

    printError("Transcoder pull", transcoder->error());

    cout << "Frames decoded: " << frameCounter << endl;
    cout << "Output file: " << opt.outputFile << endl;

    outfile.close();
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

    Library::initialize();
    
	bool decode = decodeH264Stream(opt);

    Library::shutdown();
    
	return decode ? 0 : 1;
}
