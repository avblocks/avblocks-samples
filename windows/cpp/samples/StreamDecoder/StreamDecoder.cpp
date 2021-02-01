/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"

#include "options.h"
#include "util.h"
#include "FileStream.h"

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

bool addFrameSizeToOutputFileName(Options& opt)
{
	int width = 0;
	int height = 0;

	if((opt.yuv_frame.width > 0) && (opt.yuv_frame.height > 0))
	{
		width = opt.yuv_frame.width;
		height = opt.yuv_frame.height;
	}
	else
	{
		auto mediaInfo = primo::make_ref(Library::createMediaInfo());

		mediaInfo->inputs()->at(0)->setFile(opt.input_file.c_str());
		mediaInfo->inputs()->at(0)->setStreamType(opt.input_stream_type.Id);

		if(!mediaInfo->open())
		{
			printError(L"MediaInfo open", mediaInfo->error());
			return false;
		}

        for (int i = 0; i < mediaInfo->outputs()->count(); ++i)
        {
            auto socket = mediaInfo->outputs()->at(i);
            for (int j = 0; j < socket->pins()->count(); j++)
            {
                auto psi = socket->pins()->at(j)->streamInfo();

                if (MediaType::Video == psi->mediaType())
                {
                    VideoStreamInfo* vsi = static_cast<VideoStreamInfo*>(psi);

                    width = vsi->frameWidth();
                    height = vsi->frameHeight();
                    break;
                }
            }
        }
	}

	if((width <= 0) || (height <= 0))
		return false;

	wstring::size_type pos = opt.output_file.rfind(L'.');
	if (wstring::npos != pos)
	{
		wstring ext = opt.output_file.substr(pos);
		wstring name = opt.output_file.substr(0, pos);

		std::wstringstream newName;
		newName << name << L"_" << width << L"x" << height << ext;
		opt.output_file = newName.str();
	}

	return true;
}

bool decode(Options& opt)
{
	if(!addFrameSizeToOutputFileName(opt))
		return false;

	deleteFile(opt.output_file.c_str());

	wcout << L"Input file: " << opt.input_file << endl;
	wcout << L"YUV output file: " << opt.output_file << endl;

	auto inputStream = primo::make_ref(FileStream::create());
	auto outputStream = primo::make_ref(FileStream::create());

	inputStream->setFile(opt.input_file.c_str(), FileStream::OpenModeRead);
	outputStream->setFile(opt.output_file.c_str(), FileStream::OpenModeWrite);

	auto transcoder = primo::make_ref(Library::createTranscoder());
	// In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
	transcoder->setAllowDemoMode(TRUE);

	// Configure input
	{
		auto socket = primo::make_ref(Library::createMediaSocket());
		socket->setStream(inputStream.get());
		socket->setStreamType(opt.input_stream_type.Id);
		transcoder->inputs()->add(socket.get());
	}

	// Configure output
	{
		auto streamInfo = primo::make_ref(Library::createVideoStreamInfo());

		if(opt.yuv_fps > 0)
		{
			streamInfo->setFrameRate(opt.yuv_fps);
		}

		if((opt.yuv_frame.width > 0) && (opt.yuv_frame.height > 0))
		{
			streamInfo->setFrameWidth(opt.yuv_frame.width);
			streamInfo->setFrameHeight(opt.yuv_frame.height);
		}

		if(opt.yuv_color.Id != primo::codecs::ColorFormat::Unknown)
		{
			streamInfo->setColorFormat(opt.yuv_color.Id);
		}

		streamInfo->setStreamType(StreamType::UncompressedVideo);
		streamInfo->setScanType(ScanType::Progressive);

		auto pin = primo::make_ref(Library::createMediaPin());
		pin->setStreamInfo(streamInfo.get());

		auto socket = primo::make_ref(Library::createMediaSocket());
		socket->setStream(outputStream.get());
		socket->setStreamType(StreamType::UncompressedVideo);
		socket->pins()->add(pin.get());

		transcoder->outputs()->add(socket.get());
	}

	bool_t res = transcoder->open();
	printError(L"Open Transcoder", transcoder->error());
	if (!res)
		return false;

	res = transcoder->run();
	printError(L"Run Transcoder", transcoder->error());
	if (!res)
		return false;

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

	primo::avblocks::Library::initialize();

	bool decodeResult = decode(opt);

	primo::avblocks::Library::shutdown();

	return decodeResult ? 0 : 1;
}

