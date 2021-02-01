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
using namespace std;

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
	auto socket = primo::make_ref(Library::createMediaSocket());
	socket->setFile(primo::ustring(opt.outputFile));
	socket->setStreamType(StreamType::UncompressedVideo);

	auto pin = primo::make_ref(Library::createMediaPin());
	socket->pins()->add(pin.get());

	auto vsi = primo::make_ref(Library::createVideoStreamInfo());
	pin->setStreamInfo(vsi.get());

	vsi->setStreamType(StreamType::UncompressedVideo);
	vsi->setColorFormat(ColorFormat::YUV420);

	return socket;
}

bool decodeAvcFile(Options& opt)
{
	deleteFile(opt.outputFile.c_str());

	auto mediaInfo = primo::make_ref(Library::createMediaInfo());
	mediaInfo->inputs()->at(0)->setFile(primo::ustring(opt.inputFile));

	if(!mediaInfo->open())
	{
		printError("MediaInfo open", mediaInfo->error());
		return false;
	}

	// input socekt
	auto inputSocket = primo::make_ref(Library::createMediaSocket(mediaInfo.get()));
	
	// output socket
	auto outputSocket = createOutputSocket(opt);

	// create transcoder
	auto transcoder = primo::make_ref(Library::createTranscoder());
	transcoder->setAllowDemoMode(true);
	transcoder->inputs()->add(inputSocket.get());
	transcoder->outputs()->add(outputSocket.get());

	if(!transcoder->open())
	{
		printError("Transcoder open", transcoder->error());
		return false;
	}

	if(!transcoder->run())
	{
		printError("Transcoder run", transcoder->error());
		return false;
	}

	transcoder->close();

	cout << "Output: " << opt.outputFile << endl;

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

	bool decode = decodeAvcFile(opt);

	Library::shutdown();

	return decode ? 0 : 1;
}