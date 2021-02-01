/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include <PrimoUString.h>

#include "options.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

MediaBuffer* createMediaBufferWithFileBytes(const string &imagePath)
{
    vector<uint8_t> imageData = readFileBytes(imagePath.c_str());
    int imageSize = imageData.size();

    if (imageSize <= 0)
    {
        cout << "Could not load the overlay image." << endl;
        return NULL;
    }

    auto mediaBuffer = primo::make_ref(Library::createMediaBuffer());
    mediaBuffer->append(&imageData[0], imageSize);

    return mediaBuffer.release();
}

MediaSocket* configureOutputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setFile(primo::ustring(opt.outputFile));

    auto outputVideoPin = primo::make_ref(Library::createMediaPin());
    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    outputVideoPin->setStreamInfo(vsi.get());
    socket->pins()->add(outputVideoPin.get());

    // Overlay
    auto overlayBuffer = primo::make_ref(createMediaBufferWithFileBytes(opt.overlayImage));

    if (!overlayBuffer)
        return NULL;

    auto overlayVsi = primo::make_ref(Library::createVideoStreamInfo());
    overlayVsi->setStreamType(primo::codecs::StreamType::PNG);

    outputVideoPin->params()->addInt(Param::Video::Overlay::Mode, primo::codecs::AlphaCompositingMode::Atop);
    outputVideoPin->params()->addInt(Param::Video::Overlay::LocationX, opt.position.x);
    outputVideoPin->params()->addInt(Param::Video::Overlay::LocationY, opt.position.y);
    outputVideoPin->params()->addFloat(Param::Video::Overlay::BackgroundAlpha, 1.0);
    outputVideoPin->params()->addFloat(Param::Video::Overlay::ForegroundAlpha, opt.alpha);

    outputVideoPin->params()->addMediaBuffer(Param::Video::Overlay::ForegroundBuffer, overlayBuffer.get());
    outputVideoPin->params()->addVideoStreamInfo(Param::Video::Overlay::ForegroundBufferFormat, overlayVsi.get());

    return socket.release();
}

bool applyOverlay(Options& opt)
{
    deleteFile(opt.outputFile.c_str());

    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(TRUE);

    auto inputSocket = primo::make_ref(Library::createMediaSocket());
    inputSocket->setFile(primo::ustring(opt.inputFile));
    transcoder->inputs()->add(inputSocket.get());

    auto outputSocket = primo::make_ref(configureOutputSocket(opt));

    if (!outputSocket)
        return false;

    transcoder->outputs()->add(outputSocket.get());

    if (!transcoder->open())
    {
        printError("transcoder open", transcoder->error());
        return false;
    }

    if (!transcoder->run())
    {
        printError("transcoder run", transcoder->error());
        return false;
    }

    transcoder->close();

    cout << "\nOutput file: " << opt.outputFile << endl;

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

    bool overlayed = applyOverlay(opt);

	Library::shutdown();

	return overlayed ? 0 : 1;
}
