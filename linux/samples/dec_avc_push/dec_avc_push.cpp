/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include <iostream>
#include <string>

#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>

#include "options.h"
#include "util.h"

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
		cout << primo::ustring(e->message()) << ", ";
	}

	cout << "facility:" << e->facility() << ", error:" << e->code() << endl;
}

primo::ref<MediaSocket> inputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::H264);

    auto pin = primo::make_ref(Library::createMediaPin());
    socket->pins()->add(pin.get());

    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    pin->setStreamInfo(vsi.get());

    vsi->setStreamType(StreamType::H264);
    vsi->setScanType(ScanType::Progressive);

    return socket;
}

primo::ref<MediaSocket> outputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setFile(primo::ustring(opt.output_file));
    socket->setStreamType(StreamType::UncompressedVideo);

    auto pin = primo::make_ref(Library::createMediaPin());
    socket->pins()->add(pin.get());

    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    pin->setStreamInfo(vsi.get());

    vsi->setStreamType(StreamType::UncompressedVideo);
    vsi->setScanType(ScanType::Progressive);
    vsi->setColorFormat(ColorFormat::YUV420);

    return socket;
}

bool transcode(Options& opt)
{
    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);

    auto inSocket = inputSocket(opt);
    auto outSocket = outputSocket(opt);
    
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    // transcoder will fail if output exists (by design)
    deleteFile(opt.output_file.c_str());

    if (!transcoder->open())
    {
        printError("transcoder open: ", transcoder->error());
        return false;
    }

    for (int fileCount = 0; ;fileCount++)
    {
        char file[PATH_MAX];
        string pattern = "au_%04d.h264";
        string filePath = opt.input_dir + "/" + pattern;
        sprintf(file, filePath.c_str(), fileCount);

        // program
        vector<uint8_t> inputData = readFileBytes(file);
        if (inputData.size() <= 0)
        {
            sprintf(file, pattern.c_str(), fileCount-1);
            cout << "Decoded " << fileCount << " files." << "(last decoded file: " << file << ")" << endl;
            break;
        }

        auto buffer = primo::make_ref(Library::createMediaBuffer());
        buffer->attach(inputData.data(), inputData.size(), true);

        auto sample = primo::make_ref(Library::createMediaSample());
        sample->setBuffer(buffer.get());

        if (!transcoder->push(0, sample.get()))
        {
            printError("transcoder push: ", transcoder->error());
            return false;
        }
    }

    if (!transcoder->flush())
    {
        printError("transcoder flush: ", transcoder->error());
        return false;
    }

    transcoder->close();

    std::cout << "output file: " << opt.output_file << std::endl;

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

	// set your license string
	// Library::setLicense("avblocks-license-xml");
	Library::initialize();

    bool transcodeResult = transcode(opt);

	Library::shutdown();

    return transcodeResult ? 0 : 1;
}

