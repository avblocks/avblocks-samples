/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

// InputDS.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "Options.h"
#include "Util.h"
#include "DSGraph.h"
#include "SampleGrabberCB.h"
#include <atlstr.h>

using namespace primo::avblocks;
using namespace primo::codecs;
using namespace std;

bool ConfigureVideoInput(DSGraph &dsGraph, Transcoder *pTranscoder)
{
	AM_MEDIA_TYPE mt;
	HRESULT hr = dsGraph.pVideoGrabber->GetConnectedMediaType(&mt);
	if (FAILED(hr))
		return false;

	// Examine the format block.
	if ((mt.majortype != MEDIATYPE_Video) || (mt.formattype != FORMAT_VideoInfo) || 
		(mt.cbFormat < sizeof(VIDEOINFOHEADER)) || (mt.pbFormat == NULL))
	{
		FreeMediaType(mt);
		return false;
	}

	VIDEOINFOHEADER *pvh = (VIDEOINFOHEADER*)mt.pbFormat;

	primo::avblocks::MediaPin *pin = primo::avblocks::Library::createMediaPin();
	pin->setConnection(primo::avblocks::PinConnection::Auto);

	primo::codecs::VideoStreamInfo *streamInfo = primo::avblocks::Library::createVideoStreamInfo();

	if(pvh->AvgTimePerFrame > 0)
		streamInfo->setFrameRate((double)10000000 / pvh->AvgTimePerFrame);

	streamInfo->setBitrate(0);

	streamInfo->setFrameWidth(pvh->bmiHeader.biWidth);
	streamInfo->setFrameHeight(abs(pvh->bmiHeader.biHeight));

	streamInfo->setDisplayRatioWidth(streamInfo->frameWidth());
	streamInfo->setDisplayRatioHeight(streamInfo->frameHeight());
	streamInfo->setColorFormat(GetColorFormat(mt.subtype));
	streamInfo->setScanType(ScanType::Progressive);
	streamInfo->setDuration(0.0);
	streamInfo->setStreamType(primo::codecs::StreamType::UncompressedVideo);

    switch(streamInfo->colorFormat())
    {
		case ColorFormat::BGR32:
		case ColorFormat::BGRA32:
        case ColorFormat::BGR24:
        case ColorFormat::BGR444:
        case ColorFormat::BGR555:
        case ColorFormat::BGR565:
			streamInfo->setFrameBottomUp(pvh->bmiHeader.biHeight > 0);
            break;
    }

	pin->setStreamInfo(streamInfo);
	streamInfo->release();

	primo::avblocks::MediaSocket * inSocket = primo::avblocks::Library::createMediaSocket();
	inSocket->pins()->add(pin);
	inSocket->setStreamType(pin->streamInfo()->streamType());
	pin->release();

	dsGraph.pVideoGrabberCB->Init(pTranscoder, pTranscoder->inputs()->count(), dsGraph.pMediaControl);

	pTranscoder->inputs()->add(inSocket);
	inSocket->release();

	FreeMediaType(mt);

	return true;
}

bool ConfigureAudioInput(DSGraph &dsGraph, Transcoder *pTranscoder)
{
	AM_MEDIA_TYPE mt;
	HRESULT hr = dsGraph.pAudioGrabber->GetConnectedMediaType(&mt);
	if (FAILED(hr))
		return false;

	// Examine the format block.
	if ((mt.majortype != MEDIATYPE_Audio) || (mt.formattype != FORMAT_WaveFormatEx) || (mt.pbFormat == NULL))
	{
		FreeMediaType(mt);
		return false;
	}

	WAVEFORMATEX *wfx = (WAVEFORMATEX*)mt.pbFormat;

	primo::avblocks::MediaPin *pin = primo::avblocks::Library::createMediaPin();
	pin->setConnection(primo::avblocks::PinConnection::Auto);

	primo::codecs::AudioStreamInfo *streamInfo = primo::avblocks::Library::createAudioStreamInfo();

	streamInfo->setBitsPerSample(wfx->wBitsPerSample);
	streamInfo->setChannels(wfx->nChannels);
	streamInfo->setSampleRate(wfx->nSamplesPerSec);
    streamInfo->setStreamType(StreamType::LPCM);

	pin->setStreamInfo(streamInfo);
	streamInfo->release();

	primo::avblocks::MediaSocket * inSocket = primo::avblocks::Library::createMediaSocket();
	inSocket->pins()->add(pin);
	inSocket->setStreamType(pin->streamInfo()->streamType());
	pin->release();

	dsGraph.pAudioGrabberCB->Init(pTranscoder, pTranscoder->inputs()->count(), dsGraph.pMediaControl);

	pTranscoder->inputs()->add(inSocket);
	inSocket->release();

	FreeMediaType(mt);

	return true;
}

static void PrintError(wchar_t *action, const primo::error::ErrorInfo *pErrorInfo)
{
    if (action != NULL)
    {
		wcout << action << _T(":");
    }

	
    if (primo::error::ErrorFacility::Success == pErrorInfo->facility())
    {
		wcout <<  _T("Success");
    }
    else
    {
		if (pErrorInfo->message())
		{
			wcout << pErrorInfo->message() << _T(", ");
		}

		wcout << _T("facility:") << pErrorInfo->facility() << _T(", error:") << pErrorInfo->code();
    }

	wcout << endl;
}

bool EncodeDirectShowInput(const wchar_t *pszInputFile, const wchar_t *pszOutputFile, const char* preset)
{
	DeleteFile(pszOutputFile);

	DSGraph dsGraph;
	
    auto transcoder = primo::make_ref(Library::createTranscoder());
	// In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
	transcoder->setAllowDemoMode(TRUE);

	wcout << _T("Initializing DirectShow graph.") << endl;

    /*
        If the source is a DirectShow filter instead of a file then:
        1) Create an instance of the source filter
        2) Configure source filter
        3) Call dsGraph.Init(sourceFilter);

        For example dsGraph.Init(inputFile) can be replaced with the following code:
                   
            1) Create an instance of the source filter 
                    
            // FileSourceAsync filter
				IBaseFilter *pSourceFilter = NULL;
				HRESULT hresult = CoCreateInstance(CLSID_AsyncReader, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pSourceFilter));

            // or WM ASF Reader filter
				IBaseFilter *pSourceFilter = NULL;
				HRESULT hresult = CoCreateInstance(CLSID_WMAsfReader, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pSourceFilter));
                  
            2) Configure source filter
				IFileSourceFilter *pFileSourceFilter = NULL;
				hresult = pSourceFilter->QueryInterface(IID_PPV_ARGS(&pFileSourceFilter));
				hresult = pFileSourceFilter->Load(pszInputFile, NULL);

            3)
            hresult = dsGraph.Init(pSourceFilter);
    */


    dsGraph.Init(pszInputFile);

    if (dsGraph.pVideoGrabber != NULL)
        ConfigureVideoInput(dsGraph, transcoder.get());

    if (dsGraph.pAudioGrabber != NULL)
        ConfigureAudioInput(dsGraph, transcoder.get());

    if ((dsGraph.pVideoGrabber == NULL) && (dsGraph.pAudioGrabber == NULL))
    {
        wcout << _T("No audio or video can be read from the DirectShow graph.") << endl;
        return false;
    }

	// Configure output
	{
		auto outSocket = primo::make_ref(Library::createMediaSocket(preset));
        outSocket->setFile(pszOutputFile);

		transcoder->outputs()->add(outSocket.get());
	}

	bool_t res = transcoder->open();

    PrintError(_T("Open Transcoder"), transcoder->error());
    if (!res)
        return false;

	CRITICAL_SECTION transcoderSync;
	InitializeCriticalSection(&transcoderSync);

    if (dsGraph.pVideoGrabberCB != NULL)
        dsGraph.pVideoGrabberCB->SetTranscoderSync(&transcoderSync);

    if (dsGraph.pAudioGrabberCB != NULL)
        dsGraph.pAudioGrabberCB->SetTranscoderSync(&transcoderSync);

    wcout << _T("Running DirectShow graph.") << endl;

	HRESULT hr = dsGraph.pMediaControl->Run();
	if(FAILED(hr))
		return false;

    while(true)
    {
		OAFilterState fs;
		dsGraph.pMediaControl->GetState(-1, &fs);

		if(fs != State_Running)
			break;

		long ev;
		dsGraph.pMediaEvent->WaitForCompletion(1000, &ev);

		if(ev == EC_COMPLETE)
			break;
    }

	wcout << _T("DirectShow graph is stopped.") << endl;

    if ((dsGraph.pVideoGrabberCB != NULL) && (dsGraph.pVideoGrabberCB->get_TranscoderError() != NULL))
        PrintError(_T("Transcoder Error"), transcoder->error());

    if ((dsGraph.pAudioGrabberCB != NULL) && (dsGraph.pAudioGrabberCB->get_TranscoderError() != NULL))
        PrintError(_T("Transcoder Error"), transcoder->error());

	wcout << _T("Closing transcoder.") << endl;

    if (!transcoder->flush())
        PrintError(_T("Flush Transcoder"), transcoder->error());

    transcoder->close();

	DeleteCriticalSection(&transcoderSync);

	return true;
};


int _tmain(int argc, wchar_t* argv[])
{
	Options opt;

	switch(prepareOptions( opt, argc, argv))
	{
		case Command: return 0;
		case Error: return 1;
	}
			
	CoInitialize(NULL);

	primo::avblocks::Library::initialize();
	primo::avblocks::Library::setLicense("YOUR AVBLOCKS LICENSE");

	bool encode = EncodeDirectShowInput(opt.input_file.c_str(), opt.output_file.c_str(), opt.preset.name);
    
	primo::avblocks::Library::shutdown();

    CoUninitialize();

    return encode ? 0 : 1;
}
