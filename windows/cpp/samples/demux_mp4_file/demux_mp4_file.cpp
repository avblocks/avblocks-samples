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

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

primo::ref<Transcoder> configureTranscoder(Options& opt)
{
    primo::ref<MediaInfo> info(Library::createMediaInfo());
    info->inputs()->at(0)->setFile(opt.inputFile.c_str());

    if (!info->open())
    {
        printError(L"MediaInfo open", info->error());
        return primo::ref<Transcoder>();
    }

    primo::ref<MediaSocket> inSocket(Library::createMediaSocket(info.get()));

    info->close();

    primo::ref<Transcoder> transcoder(Library::createTranscoder());
    transcoder->setAllowDemoMode(TRUE);

    transcoder->inputs()->add(inSocket.get());

    bool audio = false;
    bool video = false;

    for (int i = 0; i < inSocket->pins()->count(); ++i)
    {
        wstring fileName;
        if (inSocket->pins()->at(i)->streamInfo()->mediaType() == MediaType::Audio && !audio)
        {
            audio = true;
            fileName = opt.outputFile + L".aud.mp4";
        }
        else if (inSocket->pins()->at(i)->streamInfo()->mediaType() == MediaType::Video && !video)
        {
            video = true;
            fileName = opt.outputFile + L".vid.mp4";
        }
        else
        {
            inSocket->pins()->at(i)->setConnection(PinConnection::Disabled);
            continue;
        }

        primo::ref<MediaSocket> outSocket(Library::createMediaSocket());
        outSocket->pins()->add(inSocket->pins()->at(i));
        deleteFile(fileName.c_str());
        outSocket->setFile(fileName.c_str());

        wcout << L"Output file: " << fileName << endl;

        transcoder->outputs()->add(outSocket.get());
    }

	return transcoder;
}

bool demuxMP4(Options& opt)
{
    auto transcoder = configureTranscoder(opt);

    if (!transcoder)
        return false;

    if (!transcoder->open())
    {
        printError(L"Transcoder open", transcoder->error());
        return false;
    }

    if (!transcoder->run())
    {
        printError(L"Transcoder run", transcoder->error());
        return false;
    }

    transcoder->close();
    return true;
}

int wmain(int argc, wchar_t* argv[])
{
	Options opt;

	switch(prepareOptions(opt, argc, argv))
	{
		case Command: return 0;
		case Error:	return 1;
	}

    Library::initialize();

	bool demuxResult = demuxMP4(opt);

    Library::shutdown();

	return demuxResult ? 0 : 1;
}
