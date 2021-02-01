/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "stdafx.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;

wstring engineToString(primo::codecs::HwEngine::Enum engine)
{
    switch (engine)
    {
    case primo::codecs::HwEngine::None:
        return L"None";
    case primo::codecs::HwEngine::QuickSyncVideo:
        return L"QuickSyncVideo";
    case primo::codecs::HwEngine::Nvenc:
        return L"Nvenc";
    case primo::codecs::HwEngine::VideoCodingEngine:
        return L"VideoCodingEngine";
    default:
        return L"";
    }
}

wstring encoderTypeToString(primo::codecs::HwCodecType::Enum type)
{
    switch (type)
    {
    case primo::codecs::HwCodecType::None:
        return L"None";
    case primo::codecs::HwCodecType::H264Encoder:
        return L"H264Encoder";
    case primo::codecs::HwCodecType::H265Encoder:
        return L"H265Encoder";
    default:
        return L"";
    }
}

wstring apiToString(primo::codecs::HwApi::Enum api)
{
    switch (api)
    {
    case primo::codecs::HwApi::None:
        return L"None";
    case primo::codecs::HwApi::IntelMedia:
        return L"IntelMedia";
    case primo::codecs::HwApi::Nvenc:
        return L"Nvenc";
    case primo::codecs::HwApi::AmdOpenVideo:
        return L"AmdOpenVideo";
    case primo::codecs::HwApi::AmdMedia:
        return L"AmdMedia";
    case primo::codecs::HwApi::MediaFoundation:
        return L"MediaFoundation";
    default:
        return L"";
    }
}

wstring vendorToString(primo::codecs::HwVendor::Enum vendor)
{
    switch (vendor)
    {
    case primo::codecs::HwVendor::None:
        return L"None";
    case primo::codecs::HwVendor::Intel:
        return L"Intel";
    case primo::codecs::HwVendor::Nvidia:
        return L"Nvidia";
    case primo::codecs::HwVendor::Amd:
        return L"Amd";
    default:
        return L"";
    }
}

void printHardwareList()
{
    primo::ref<Hardware> hw(Library::createHardware());

    hw->refresh();

    for (int i = 0; i < hw->devices()->count(); ++i)
    {
        HwDevice* device = hw->devices()->at(i);


        wcout << std::left << setw(15) << L"Vendor" << vendorToString(device->vendor()) << endl;
        wcout << setw(15) << L"ID" << device->ID() << endl;
        wcout << setw(15) << L"Device name" << device->name() << endl;

        for (int j = 0; j < device->codecs()->count(); ++j)
        {
            HwCodec* codec = device->codecs()->at(j);
            wcout << setw(15) << L"Engine" << engineToString(codec->engine()) << endl;
            wcout << setw(15) << L"Type" << encoderTypeToString(codec->type()) << endl;
            wcout << setw(15) << L"API" << apiToString(codec->api()) << endl;
            wcout << setw(15) << L"Codec name" << codec->name() << endl;
        }
        wcout << endl;
    }
}

int _tmain(int argc, wchar_t* argv[])
{
	// set your license string
	// Library::setLicense("avblocks-license-xml");
	Library::initialize();

    printHardwareList();

	Library::shutdown();

    return 0;
}

