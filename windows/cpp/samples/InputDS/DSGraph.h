/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once
#include "Util.h"
#include "SampleGrabberCB.h"


class DSGraph
{
	HRESULT Init(PCWSTR pszInputFile, IBaseFilter *pSourceFilter);

	HRESULT InitVideoGrabber(IBaseFilter *pSourceFilter);
	HRESULT InitAudioGrabber(IBaseFilter *pSourceFilter);

	HRESULT ConnectSampleGrabber(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);

public:

    IGraphBuilder *pGraph;
    IMediaControl *pMediaControl;
    IMediaEventEx *pMediaEvent;

    IBaseFilter *pVideoGrabberFilter;
    ISampleGrabber *pVideoGrabber;
	SampleGrabberCB *pVideoGrabberCB;
    IBaseFilter *pVideoNullFilter;

    IBaseFilter *pAudioGrabberFilter;
    ISampleGrabber *pAudioGrabber;
	SampleGrabberCB *pAudioGrabberCB;
    IBaseFilter *pAudioNullFilter;

	DSGraph()
	{
		pGraph = NULL;
		pMediaControl = NULL;
		pMediaEvent = NULL;

		pVideoGrabberFilter = NULL;
		pVideoGrabber = NULL;
		pVideoGrabberCB = NULL;
		pVideoNullFilter = NULL;

		pAudioGrabberFilter = NULL;
		pAudioGrabber = NULL;
		pAudioGrabberCB = NULL;
		pAudioNullFilter = NULL;
	}

	~DSGraph()
	{
		Reset();
	}

	void Reset()
	{
		SafeRelease(&pVideoGrabberFilter);
		SafeRelease(&pVideoGrabber);
		SafeRelease(&pVideoGrabberCB);
		SafeRelease(&pVideoNullFilter);

		SafeRelease(&pAudioGrabberFilter);
		SafeRelease(&pAudioGrabber);
		SafeRelease(&pAudioGrabberCB);
		SafeRelease(&pAudioNullFilter);

		SafeRelease(&pMediaControl);
		SafeRelease(&pMediaEvent);
		SafeRelease(&pGraph);
	}

	HRESULT Init(PCWSTR pszInputFile)
	{
		return Init(pszInputFile, NULL);
	}

	HRESULT Init(IBaseFilter *pSourceFilter)
	{
		return Init(NULL, pSourceFilter);
	}
};
