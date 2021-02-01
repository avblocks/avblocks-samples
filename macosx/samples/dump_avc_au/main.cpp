/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
//#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <memory.h>

#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>
#include "util.h"
#include "options.h"


using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

/*
 Access Unit: one picture (parameters + all slices)
 The elementary stream may contain optional access unit delimiters (AUD).
 A picture may consist of one or more slices.
 
 */

// Network Abstraction Layer Unit Definitions per H.264/AVC spec
enum NALUType
{
    UNSPEC    = 0,  // Unspecified
    SLICE     = 1,  // Coded slice of a non-IDR picture
    DPA       = 2,  // Coded slice data partition A
    DPB       = 3,  // Coded slice data partition B
    DPC       = 4,  // Coded slice data partition C
    IDR       = 5,  // Coded slice of an IDR picture
    SEI       = 6,  // Supplemental enhancement information
    SPS       = 7,  // Sequence parameter set
    PPS       = 8,  // Picture parameter set
    AUD       = 9,  // Access unit delimiter
    EOSEQ     = 10, // End of sequence
    EOSTREAM  = 11, // End of stream
    FILL      = 12  // Filler data
};

enum NALUPriority
{
    DISPOSABLE  = 0,
    LOW         = 1,
    HIGH        = 2,
    HIGHEST     = 3,
};


NALUType nal_unit_type(uint8_t byte)
{
    return static_cast<NALUType>(byte & 0x1f);
}

NALUPriority nal_unit_ref_idc(uint8_t byte)
{
    return static_cast<NALUPriority>((byte & 0x60) >> 5);
}

#define MAP_ENUM_VALUE(p) strings[p] = #p
std::ostream& operator << (std::ostream& out, const NALUType value)
{
    static std::map<NALUType, std::string> strings;
    if (strings.empty())
    {
        MAP_ENUM_VALUE(SLICE);
        MAP_ENUM_VALUE(DPA);
        MAP_ENUM_VALUE(DPB);
        MAP_ENUM_VALUE(DPB);
        MAP_ENUM_VALUE(DPC);
        MAP_ENUM_VALUE(IDR);
        MAP_ENUM_VALUE(SEI);
        MAP_ENUM_VALUE(SPS);
        MAP_ENUM_VALUE(PPS);
        MAP_ENUM_VALUE(AUD);
        MAP_ENUM_VALUE(EOSEQ);
        MAP_ENUM_VALUE(EOSTREAM);
        MAP_ENUM_VALUE(FILL);
    }
    
    return out << strings[value];
}

std::ostream& operator << (std::ostream& out, const NALUPriority value)
{
    static std::map<NALUPriority, std::string> strings;
    if (strings.empty())
    {
        MAP_ENUM_VALUE(DISPOSABLE);
        MAP_ENUM_VALUE(LOW);
        MAP_ENUM_VALUE(HIGH);
        MAP_ENUM_VALUE(HIGHEST);
    }
    
    return out << strings[value];
}


void print_nalu_header(const uint8_t* data)
{
    std::cout << left << "  "
    << setw(8) << nal_unit_type(*data) << ": "
    << nal_unit_ref_idc(*data)
    << std::endl;
}

void print_nalus(MediaBuffer* buffer)
{
    // This parsing code assumes that MediaBuffer contains
    // a single Access Unit of one or more complete NAL Units
    while (buffer->dataSize() > 1)
    {
        int dataOffset = buffer->dataOffset();
        int dataSize = buffer->dataSize();
        const uint8_t* data = buffer->data();
        
        // is this a NALU with a 3 byte start code prefix
        if (dataSize >= 3 &&
            0x00 == data[0] &&
            0x00 == data[1] &&
            0x01 == data[2])
        {
            print_nalu_header(data + 3);
            
            // advance in the buffer
            buffer->setData(dataOffset + 3, dataSize - 3);
        }
        // OR is this a NALU with a 4 byte start code prefix
        else if (dataSize >= 4 &&
                 0x00 == data[0] &&
                 0x00 == data[1] &&
                 0x00 == data[2] &&
                 0x01 == data[3])
        {
            print_nalu_header(data + 4);
            
            // advance in the buffer
            buffer->setData(dataOffset + 4, dataSize - 4);
        }
        else
        {
            // advance in the buffer
            buffer->setData(dataOffset + 1, dataSize - 1);
        }
        
        // NOTE: Some NALUs may have a trailing zero byte. The `while`
        // condition `buffer->dataSize() > 1` will effectively
        // skip the trailing zero byte.
    }
}

void write_au_file(string outputDir, int au_index, MediaBuffer* buffer)
{
    ostringstream file_name;
    file_name << outputDir << "/au_" << setw(4) << setfill('0') << au_index << ".h264";
    ofstream file(file_name.str(), ios::binary | ios::trunc);
    if (file)
    {
        file.write((const char*)buffer->data(), buffer->dataSize());
    }
}

bool parse_h264_stream(string inputFile, string outputDir)
{
    // Create an input socket from file
    primo::ref<MediaSocket> inSocket(Library::createMediaSocket());
    inSocket->setFile(primo::ustring(inputFile));
    
    // Create an output socket with one video pin
    primo::ref<VideoStreamInfo> outStreamInfo(Library::createVideoStreamInfo());
    
    primo::ref<MediaPin> outPin(Library::createMediaPin());
    outPin->setStreamInfo(outStreamInfo.get());
    
    primo::ref<MediaSocket> outSocket(Library::createMediaSocket());
    outSocket->pins()->add(outPin.get());
    
    // Create Transcoder
    primo::ref<Transcoder> transcoder(Library::createTranscoder());
    transcoder->inputs()->add(inSocket.get());
    transcoder->outputs()->add(outSocket.get());
    
    auto res = transcoder->open();
    printError("transcoder open", transcoder->error());
    
    if (!res)
        return false;
    
    int32_t inputIndex = 0;
    primo::ref<MediaSample> accessUnit(Library::createMediaSample());
    
    if (!makeDir(outputDir))
    {
        cout << "cannot create output directory: " << outputDir << endl;
        return false;
    }
    
    while (transcoder->pull(inputIndex, accessUnit.get()))
    {
        // Each call to Transcoder::pull returns one Access Unit.
        // The Access Unit may contain one or more NAL units.
        
        static int au_index = 0;
        auto au_buffer = accessUnit->buffer();
        cout << "AU #" << au_index << ", " << au_buffer->dataSize() << " bytes" << endl;
        write_au_file(outputDir, au_index, au_buffer);
        print_nalus(au_buffer);
        ++au_index;
    }
    transcoder->close();
    return true;
}

int main(int argc, char* argv[])
{
    Options opt;
    switch (prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
    }
    
    Library::initialize();
    
    // set your license string
    // Library::setLicense("avblocks-license-xml");
    
    bool parseResult = parse_h264_stream(opt.input_file, opt.output_dir);
    
    Library::shutdown();
    
    if (!parseResult)
        return 1;
    
    cout << "\nSuccessfully parsed input file: " << opt.input_file << "\n";
    cout << "Output directory: " << opt.output_dir << "\n\n";
    return 0;
}