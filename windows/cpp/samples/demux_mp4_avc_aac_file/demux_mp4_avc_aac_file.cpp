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

wstring GenerateOutputFileName(const wstring &fileName, int fileIndex, const wstring type)
{
    wostringstream ws;
    ws << fileName << L"." << setw(3) << setfill(L'0') << fileIndex << L"." << type;
    wstring newFileName = ws.str();

    wcout << L"Output file: " << newFileName << endl;

    return newFileName;
}

primo::ref<Transcoder> createTranscoder(Options& opt)
{
    primo::ref<MediaInfo> info(Library::createMediaInfo());
    info->inputs()->at(0)->setFile(opt.input_file.c_str());

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

    int vIndex = 0;
    int aIndex = 0;

    for (int i = 0; i < inSocket->pins()->count(); ++i)
    {
        int fileIndex;
        wstring type;
        if (inSocket->pins()->at(i)->streamInfo()->mediaType() == MediaType::Audio)
        {
            fileIndex = ++aIndex;
            type = L"aac";
        }
        else if (inSocket->pins()->at(i)->streamInfo()->mediaType() == MediaType::Video)
        {
            fileIndex = ++vIndex;
            type = L"h264";
        }
        else
            continue;

        primo::ref<MediaSocket> outSocket(Library::createMediaSocket());
        primo::ref<MediaPin> pin(inSocket->pins()->at(i)->clone());
        outSocket->pins()->add(pin.get());

        wstring fileName = GenerateOutputFileName(opt.output_file, fileIndex, type);
        deleteFile(fileName.c_str());
        
        outSocket->setFile(fileName.c_str());
        transcoder->outputs()->add(outSocket.get());
    }

    return transcoder;
}

bool demuxMP4(Options& opt)
{
    auto transcoder = createTranscoder(opt);

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

int _tmain(int argc, wchar_t* argv[])
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

	bool mp4MuxResult = demuxMP4(opt);

	primo::avblocks::Library::shutdown();

	return mp4MuxResult ? 0 : 1;
}
