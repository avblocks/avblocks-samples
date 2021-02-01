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

MediaSocket* outputSocket(Options& opt, MediaSocket* inSocket)
{
    auto outSocket = primo::make_ref(Library::createMediaSocket());
    auto outPin = primo::make_ref(Library::createMediaPin());
    auto outVsi = primo::make_ref(Library::createVideoStreamInfo());
    outVsi->setColorFormat(opt.output_color.Id == ColorFormat::Unknown ? ColorFormat::YUV420 : opt.output_color.Id);
    if (opt.frame_size.height > 0)
        outVsi->setFrameHeight(opt.frame_size.height);
    if (opt.frame_size.width > 0)
        outVsi->setFrameWidth(opt.frame_size.width);
    if (opt.fps > 0)
        outVsi->setFrameRate(opt.fps);
    outVsi->setStreamType(StreamType::UncompressedVideo);
    outVsi->setScanType(ScanType::Progressive);
    outPin->setStreamInfo(outVsi.get());
    outSocket->pins()->add(outPin.get());

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
        wostringstream s; // build output file name "path\decoded_wxh.yuv"
        s << getExeDir() << L"\\decoded_" << yuv_width << L"x" << yuv_height << L".yuv";
        opt.output_file = s.str();
    }

    outSocket->setFile(opt.output_file.c_str());
    outSocket->setStreamType(StreamType::UncompressedVideo);

    return outSocket.release();
}

bool setTranscoder(Transcoder* transcoder, wstring imgFile, Options& opt)
{
    auto info = primo::make_ref(Library::createMediaInfo());
    info->inputs()->at(0)->setFile(imgFile.c_str());

    if (!info->open())
    {
        printError(L"MediaInfo open", info->error());
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
        printError(L"Transcoder open", transcoder->error());
        return false;
    }

    return true;
}

bool transcode(Options& opt)
{
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(TRUE);

    bool decoderInitialized = false;
    int32_t index = 0;

    for (int i = 0; ; i++)
    {
        WCHAR imgFile[MAX_PATH];
        wstring pattern = L"au_%04d.h264";
        wstring imgPath = getExeDir() + L"\\" + opt.input_dir + pattern;
        wsprintf(imgFile, imgPath.c_str(), i);

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
        buffer->attach(inputData.data(), inputData.size(), TRUE);
        sample->setBuffer(buffer.get());

        if (!transcoder->push(index, sample.get()))
        {
            printError(L"Transcoder push", transcoder->error());
            return false;
        }
    }

    if (!transcoder->flush())
    {
        printError(L"Transcoder flush", transcoder->error());
        return false;
    }

    wcout << L"Output file: " << opt.output_file << endl;

    transcoder->close();

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

