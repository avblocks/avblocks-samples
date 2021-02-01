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

bool transcodeSplit(const Options& opt, const std::wstring& outFile, const char* preset,
					  double startTime, double& processedTime, int64_t& processedSize, bool& isSplit);

double getMinDuration(const std::wstring& file);

bool encode_mpg_dvd(Options& opt)
{
	int splitCount = 0;
	double startTime = 0;

	std::wstring exeDir = getExeDir();

	double minDuration = getMinDuration(opt.in_file);

	while(true)
	{
		double processedTime = 0;
		int64_t processedSize = 0;
		bool isSplit = false;

		std::wostringstream filename;
		filename << exeDir << L"\\enc_mpg_dvd." << setw(3) << setfill(L'0') << ++splitCount << L".mpg";
		
        std::wstring outFile = filename.str();
		wcout << L"encoding " << outFile << endl;

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

int wmain(int argc, wchar_t* argv[])
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
