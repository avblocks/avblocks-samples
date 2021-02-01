/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include <iostream>
#include <string>
#include <unistd.h>

// memmove in AVBlocks
#include <string.h>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>

#include "options.h"
#include "util.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;
using namespace primo::error;

namespace av = primo::avblocks;
namespace pc = primo::codecs;

void printError(const char* action, const ErrorInfo* e)
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

	cout << "(facility:" << e->facility() << " error:" << e->code() << ")" << endl;
}

MediaSocket* outputSocket(Options& opt, MediaSocket* inSocket)
{
    auto outVsi = primo::make_ref(Library::createVideoStreamInfo());
    outVsi->setStreamType(StreamType::UncompressedVideo);
    outVsi->setScanType(ScanType::Progressive);
    outVsi->setColorFormat(opt.output_color.Id == ColorFormat::Unknown ? ColorFormat::YUV420 : opt.output_color.Id);
    
    auto outPin = primo::make_ref(Library::createMediaPin());
    outPin->setStreamInfo(outVsi.get());
    
    auto outSocket = primo::make_ref(Library::createMediaSocket());
    outSocket->pins()->add(outPin.get());

    // Options
    if (opt.frame_size.height > 0)
        outVsi->setFrameHeight(opt.frame_size.height);
    if (opt.frame_size.width > 0)
        outVsi->setFrameWidth(opt.frame_size.width);
    if (opt.fps > 0)
        outVsi->setFrameRate(opt.fps);

    // build output file name "path/decoded_WxH.yuv"
    if (opt.output_file.empty())
    {
        int yuv_width, yuv_height;
        if (opt.frame_size.height > 0 && opt.frame_size.width > 0)
        {
            yuv_width = opt.frame_size.width;
            yuv_height = opt.frame_size.height;
        }
        else
        {
            VideoStreamInfo* vsi = (VideoStreamInfo*)inSocket->pins()->at(0)->streamInfo();
            yuv_height = vsi->frameHeight();
            yuv_width = vsi->frameWidth();
        }

        ostringstream s; 
        s << getExeDir() << "/decoded_" << yuv_width << "x" << yuv_height << ".yuv";
        
        opt.output_file = s.str();
    }

    outSocket->setFile(primo::ustring(opt.output_file));
    outSocket->setStreamType(StreamType::UncompressedVideo);

    return outSocket.release();
}

bool setTranscoder(Transcoder* transcoder, string imgFile, Options& opt)
{
    auto info = primo::make_ref(Library::createMediaInfo());
    info->inputs()->at(0)->setFile(primo::ustring(imgFile));

    if (!info->open())
    {
        printError("MediaInfo open", info->error());
        return false;
    }

    // prepare input socket
    auto inSocket = primo::make_ref(Library::createMediaSocket(info.get()));
    inSocket->setFile(NULL);
    inSocket->setStream(NULL);

    // prepare output socket
    auto outSocket = primo::make_ref(outputSocket(opt, inSocket.get()));

    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    deleteFile(opt.output_file.c_str());

    if (!transcoder->open())
    {
        printError("Transcoder open", transcoder->error());
        return false;
    }
    
    return true;
}

bool transcode(Options& opt)
{
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);

    bool decoderInitialized = false;
    int32_t index = 0;

    for (int i = 0; ; i++)
    {
        char imgFile[PATH_MAX];
        string pattern = "au_%04d.h264";
        string imgPath = getExeDir() + "/" + opt.input_dir + pattern;
        sprintf(imgFile, imgPath.c_str(), i);

        if (!decoderInitialized)
        {
            if (!setTranscoder(transcoder.get(), imgFile, opt))
                return false;

            decoderInitialized = true;
        }

        auto sample = primo::make_ref(Library::createMediaSample());

        vector<uint8_t> inputData = readFileBytes(imgFile);
        if (inputData.size() <= 0)
            break;

        auto buffer = primo::make_ref(Library::createMediaBuffer());
        buffer->attach(inputData.data(), inputData.size(), true);
        sample->setBuffer(buffer.get());

        if (!transcoder->push(index, sample.get()))
        {
            printError("Transcoder push", transcoder->error());
            return false;
        }
    }

    if (!transcoder->flush())
    {
        printError("Transcoder flush", transcoder->error());
        return false;
    }

    cout << "Output file: " << opt.output_file << endl;

    transcoder->close();

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
	
	bool transcodeResult = transcode(opt);
	
	primo::avblocks::Library::shutdown();

	return transcodeResult ? 0 : 1;
}
