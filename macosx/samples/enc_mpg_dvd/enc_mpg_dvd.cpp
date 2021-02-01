/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

#include <cassert>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>

#include "util.h"
#include "options.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

bool transcodeSplit(const Options& opt, const std::string& outFile, const char* preset,
                    double startTime, double& processedTime, int64_t& processedSize, bool& isSplit);

double getMinDuration(const std::string& file);

bool encode_mpg_dvd(Options& opt)
{
    int splitCount = 0;
    double startTime = 0;
    
    std::string exeDir = getExeDir();
    
    double minDuration = getMinDuration(opt.in_file);
    
    while(true)
    {
        double processedTime = 0;
        int64_t processedSize = 0;
        bool isSplit = false;
        
        std::ostringstream filename;
        filename << exeDir
        << "/enc_mpg_dvd."
        << setw(3)
        << setfill('0')
        << ++splitCount
        << ".mpg";
        
        std::string outFile = filename.str();
        std::cout << "encoding " << outFile << endl;
        
        bool_t res = transcodeSplit(opt, outFile, primo::avblocks::Preset::Video::DVD::PAL_16x9_MP2,
                                    startTime, processedTime, processedSize, isSplit);
        
        if (!res)
            return false;

        if (!isSplit)
            break;
        
        startTime += processedTime;
		
		if (startTime > minDuration)
            break;
    }
    
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
    
    Library::initialize();
    
    bool encodeResult = encode_mpg_dvd(opt);
    
    Library::shutdown();
    
    return encodeResult ? 0 : 1;
}
