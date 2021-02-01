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

void printError(const wchar_t* action, const primo::error::ErrorInfo* e)
{
	if (action)
	{
		wcout << action << L": ";
	}

	if (primo::error::ErrorFacility::Success == e->facility())
	{
		wcout << L"Success" << endl;
		return;
	}

	if (e->message())
	{
		wcout << e->message() << L", ";
	}

	wcout << L"facility:" << e->facility() << L", error:" << e->code() << endl;
}

primo::ref<MediaSocket> inputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::AVC);

    auto pin = primo::make_ref(Library::createMediaPin());
    socket->pins()->add(pin.get());

    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    pin->setStreamInfo(vsi.get());

    vsi->setStreamType(StreamType::AVC);
    vsi->setScanType(ScanType::Progressive);

    return socket;
}

primo::ref<MediaSocket> outputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setFile(opt.output_file.c_str());
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
    transcoder->setAllowDemoMode(TRUE);

    auto inSocket = inputSocket(opt);
    auto outSocket = outputSocket(opt);
    
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    // transcoder will fail if output exists (by design)
    deleteFile(opt.output_file.c_str());

    if (!transcoder->open())
    {
        printError(L"transcoder open: ", transcoder->error());
        return false;
    }

    for (int fileCount = 0; ;fileCount++)
    {
        WCHAR file[MAX_PATH];
        wstring pattern = L"au_%04d.h264";
        wstring filePath = opt.input_dir + L"\\" + pattern;
        wsprintf(file, filePath.c_str(), fileCount);

        // program
        vector<uint8_t> inputData = readFileBytes(file);
        if (inputData.size() <= 0)
        {
            wsprintf(file, pattern.c_str(), fileCount-1);
            wcout << L"Decoded " << fileCount << " files." << "(last decoded file: " << file << ")" << endl;
            break;
        }

        auto buffer = primo::make_ref(Library::createMediaBuffer());
        buffer->attach(inputData.data(), inputData.size(), TRUE);

        auto sample = primo::make_ref(Library::createMediaSample());
        sample->setBuffer(buffer.get());

        if (!transcoder->push(0, sample.get()))
        {
            printError(L"transcoder push: ", transcoder->error());
            return false;
        }
    }

    if (!transcoder->flush())
    {
        printError(L"transcoder flush: ", transcoder->error());
        return false;
    }

    transcoder->close();

    std::wcout << L"output file: " << opt.output_file << std::endl;

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

	// set your license string
	// Library::setLicense("avblocks-license-xml");
	Library::initialize();

    bool transcodeResult = transcode(opt);

	Library::shutdown();

    return transcodeResult ? 0 : 1;
}

