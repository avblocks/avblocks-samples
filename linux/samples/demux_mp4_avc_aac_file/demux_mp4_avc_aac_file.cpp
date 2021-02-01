/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include <string>
#include <iostream>
#include <iomanip>

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

string GenerateOutputFileName(const string &fileName, int fileIndex, const string type)
{
    ostringstream s;
    s << fileName << "." << setw(3) << setfill('0') << fileIndex << "." << type;
    string newFileName = s.str();

    cout << "Output file: " << newFileName << endl;

    return newFileName;
}

primo::ref<Transcoder> createTranscoder(Options& opt)
{
    primo::ref<MediaInfo> info(Library::createMediaInfo());
    info->inputs()->at(0)->setFile(primo::ustring(opt.input_file));

    if (!info->open())
    {
        printError("MediaInfo open", info->error());
        return primo::ref<Transcoder>();
    }

    primo::ref<MediaSocket> inSocket(Library::createMediaSocket(info.get()));
    info->close();

    primo::ref<Transcoder> transcoder(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);
    transcoder->inputs()->add(inSocket.get());

    int vIndex = 0;
    int aIndex = 0;

    for (int i = 0; i < inSocket->pins()->count(); ++i)
    {
        int fileIndex;
        string type;
        if (inSocket->pins()->at(i)->streamInfo()->mediaType() == MediaType::Audio)
        {
            fileIndex = ++aIndex;
            type = "aac";
        }
        else if (inSocket->pins()->at(i)->streamInfo()->mediaType() == MediaType::Video)
        {
            fileIndex = ++vIndex;
            type = "h264";
        }
        else
            continue;

        primo::ref<MediaSocket> outSocket(Library::createMediaSocket());
        primo::ref<MediaPin> pin(inSocket->pins()->at(i)->clone());
        outSocket->pins()->add(pin.get());
        
        string fileName = GenerateOutputFileName(opt.output_file, fileIndex, type);
        deleteFile(fileName.c_str());
        
        outSocket->setFile(primo::ustring(fileName));
        transcoder->outputs()->add(outSocket.get());
    }

    return transcoder;
}

bool demuxMP4(Options& opt)
{
    auto transcoder = createTranscoder(opt);

    if (!transcoder->open())
    {
        printError("Transcoder open", transcoder->error());
        return false;
    }

    if (!transcoder->run())
    {
        printError("Transcoder run", transcoder->error());
        return false;
    }
        
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

	bool mp4MuxResult = demuxMP4(opt);

	primo::avblocks::Library::shutdown();

	return mp4MuxResult ? 0 : 1;
}
