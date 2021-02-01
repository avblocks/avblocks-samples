/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

class stdout_utf16
{
public:
    stdout_utf16()
    {
        // change stdout to Unicode. Cyrillic and Ideographic characters will appear in the console (console font is unicode).
        _setmode(_fileno(stdout), _O_U16TEXT);
    }

    ~stdout_utf16()
    {
        // restore ANSI mode
        _setmode(_fileno(stdout), _O_TEXT); 
    }
};

void printStatus(const wchar_t* action, const primo::error::ErrorInfo* e)
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

bool isHardwareEncoderAvailable(primo::codecs::HwVendor::Enum vendor, primo::codecs::HwCodecType::Enum type)
{
    primo::ref<Hardware> hw(Library::createHardware());

    hw->refresh();

    for (int i = 0; i < hw->devices()->count(); ++i)
    {
        HwDevice* device = hw->devices()->at(i);
        if (device->vendor() == vendor)
        {
            for (int j = 0; j < device->codecs()->count(); ++j)
            {
                if (device->codecs()->at(j)->type() == type)
                    return true;
            }
        }
    }

    return false;
}

primo::ref<MediaSocket> createInputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setStreamType(StreamType::UncompressedVideo);
    socket->setFile(opt.yuv_file.c_str());
    
    auto pin = primo::make_ref(Library::createMediaPin());
    socket->pins()->add(pin.get());

    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    pin->setStreamInfo(vsi.get());

    vsi->setStreamType(StreamType::UncompressedVideo);
    vsi->setFrameWidth(opt.frame_size.width_);
    vsi->setFrameHeight(opt.frame_size.height_);
    vsi->setColorFormat(opt.yuv_color.Id);
    vsi->setFrameRate(opt.fps);
    vsi->setScanType(ScanType::Progressive);

    return socket;
}

primo::ref<MediaSocket> createOutputSocket(Options& opt)
{
    auto socket = primo::make_ref(Library::createMediaSocket());
    socket->setFile(opt.h264_file.c_str());
    socket->setStreamType(StreamType::H264);
    socket->setStreamSubType(StreamSubType::AVC_Annex_B);

    auto pin = primo::make_ref(Library::createMediaPin());
    socket->pins()->add(pin.get());

    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    pin->setStreamInfo(vsi.get());

    pin->params()->addInt(Param::HardwareEncoder, HardwareEncoder::Intel);

    vsi->setStreamType(StreamType::H264);
    vsi->setStreamSubType(StreamSubType::AVC_Annex_B);

    return socket;
}

bool encode(Options& opt)
{
    auto inSocket = createInputSocket(opt);

    // create output socket
    auto outSocket = createOutputSocket(opt);

    // create transcoder
    auto transcoder = primo::make_ref(Library::createTranscoder());
    transcoder->setAllowDemoMode(TRUE);
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());

    // transcoder will fail if output exists (by design)
    deleteFile(opt.h264_file.c_str());

    bool_t res = transcoder->open();
    printStatus(L"Transcoder open:", transcoder->error());
    if(!res)
        return false;

    res = transcoder->run();
    printStatus(L"Transcoder run:", transcoder->error());
    if(!res)
        return false;

    transcoder->close();
    printStatus(L"Transcoder close:", transcoder->error());

    return true;
}

int wmain(int argc, wchar_t* argv[])
{
    Options opt;

    switch(prepareOptions( opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
    }

    Library::initialize();

    if (!isHardwareEncoderAvailable(primo::codecs::HwVendor::Intel, primo::codecs::HwCodecType::H264Encoder))
    {
        wcout << "Intel H.264 hardware encoder is not available on your system" << endl;
        return 0;
    }

    bool result = encode(opt);

    Library::shutdown();

    return result ? 0 : 1;
}
