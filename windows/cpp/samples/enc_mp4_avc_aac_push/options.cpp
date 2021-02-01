/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "stdafx.h"
#include "util.h"
#include "options.h"
#include "program_options.h"

using namespace std;
using namespace primo::program_options;

void setDefaultOptions(Options& opt)
{
	opt.inputAudioFile = getExeDir() + L"\\..\\assets\\aud\\avsync_44100_s16_2ch.pcm";
	opt.inputVideoFile = getExeDir() + L"\\..\\assets\\vid\\avsync_240x180_29.97fps.yuv";
	opt.outputFile = getExeDir() + L"\\avsync.mp4";
	wcout << L"Using default options: enc_mp4_avc_aac_push --inVideo " << opt.inputVideoFile << " --inAudio " << opt.inputAudioFile << " --output " << opt.outputFile << endl << endl;
}

void printUsage(OptionsConfig<wchar_t>& optionsConfig)
{
	wcout << endl << L"Usage: enc_mp4_avc_aac_push --inAudio <file> --inVideo <file> --output <file>" << endl;
	doHelp<TCHAR>(wcout, optionsConfig);
}

bool prepareOptions(int argc, wchar_t* argv[], Options& opt)
{
	OptionsConfig<wchar_t> optionsConfig;
	optionsConfig.addOptions()
		(_T("help,?"), opt.help, _T(""))
		(_T("inAudio,ia"), opt.inputAudioFile, wstring(), _T("input audio file"))
		(_T("inVideo,iv"), opt.inputVideoFile, wstring(), _T("input video file"))
		(_T("output,o"), opt.outputFile, wstring(), _T("output mixed file"));

	try
	{
		scanArgv<wchar_t>(optionsConfig, argc, argv);
	}
	catch (ParseFailure<wchar_t> &ex)
	{
		wcout << ex.message() << endl;
		printUsage(optionsConfig);
		opt.parseError = true;
		return false;
	}

	if(opt.help)
	{
		printUsage(optionsConfig);
		return false;
	}

	if(argc == 1)
	{
		setDefaultOptions(opt);
	}

	return true;
}