/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

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
        cout << primo::ustring(e->message()) << " ";
    }
    
    cout << "(facility:" << e->facility() << " error:" << e->code() << ")" << endl;
}

MediaBuffer* createMediaBufferForFile(const char* filename)
{
    FILE* h = fopen(filename, "rb");
    if (!h)
    {
        perror("open image file:");
        return NULL;
    }
    
    int res;
    
    res = fseek(h, 0, SEEK_END);
    long imgsize = ftell(h);
    res = fseek(h, 0, SEEK_SET);
    MediaBuffer* mediaBuffer = Library::createMediaBuffer(imgsize);
    size_t bytesRead = fread(mediaBuffer->start(), 1, imgsize, h);
    mediaBuffer->setData(0, imgsize);
    fclose(h);
    return mediaBuffer;
}

bool slideshow(Options& opt)
{
    cout << "preset: " << opt.preset.name << endl;
    
    const double inputFramerate = 25.0;
    const int imageCount = 250;
    bool_t res;
    
    string imgdir( getExeDir() );
    imgdir.append("/../assets/img");
    
    string outFilename (getExeDir());
    outFilename.append("/cube.");
    outFilename.append(opt.preset.fileExtension);
    remove(outFilename.c_str());
    
    auto transcoder = primo::make_ref(Library::createTranscoder());
    // In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
    transcoder->setAllowDemoMode(1);
    
    auto info = primo::make_ref(Library::createMediaInfo());
    
    // Add Inputs
    {
        string firstImage(imgdir);
        firstImage.append("/cube0000.jpeg");
        info->inputs()->at(0)->setFile(primo::ustring(firstImage));
        res = info->open();
        printError("Open Info", info->error());
        if (!res)
            return false;
        
        auto socket = primo::make_ref(Library::createMediaSocket());
        auto pin = primo::make_ref(Library::createMediaPin());
        
        auto vinfo = primo::make_ref((VideoStreamInfo*)info->outputs()->at(0)->pins()->at(0)->streamInfo()->clone());
        vinfo->setFrameRate(inputFramerate);

        pin->setStreamInfo(vinfo.get());
        socket->pins()->add(pin.get());
        transcoder->inputs()->add(socket.get());
    }
    
    
    // Add Outputs
    {
        auto socket = primo::make_ref(Library::createMediaSocket(opt.preset.name));
        socket->setFile(primo::ustring(outFilename));
        transcoder->outputs()->add(socket.get());
    }
    
    res = transcoder->open();
    printError("Open Transcoder", transcoder->error());
    if (!res)
        return false;
    
    auto mediaSample = primo::make_ref(Library::createMediaSample());
    
    for(int i = 0; i < imageCount; i++)
    {
        char imgfile[PATH_MAX];
        sprintf(imgfile, "%s/cube%04d.jpeg", imgdir.c_str(), i);
        
        auto buffer = primo::make_ref(createMediaBufferForFile(imgfile));
        
        mediaSample->setBuffer(buffer.get());
        
        // the correct start time is required by the transcoder
        mediaSample->setStartTime(i / inputFramerate);
        
        res = transcoder->push(0, mediaSample.get());
        if (!res)
        {
            printError("Push Transcoder", transcoder->error());
            return false;
        }
    }
    
    if(!transcoder->flush())
    {
        printError("Flush Transcoder", transcoder->error());
        return false;
    }
    transcoder->close();
    cout << "Output video: \"" << outFilename << "\"" << endl;
    
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
    
    primo::avblocks::Library::initialize();
    
    bool slideshowResult = slideshow(opt);
    
    primo::avblocks::Library::shutdown();
    
    return slideshowResult ? 0 : 1;
}

