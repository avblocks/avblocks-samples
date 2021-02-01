/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "stdafx.h"
#include "DSGraph.h"
#include "Util.h"


HRESULT DSGraph::Init(PCWSTR pszInputFile, IBaseFilter *pUserSourceFilter)
{
	Reset();

	// specify only one kind of input
	if((pszInputFile != NULL) && (pUserSourceFilter != NULL))
		return E_FAIL;

	HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&pGraph));
	if (FAILED(hr))
		return hr;

	hr = pGraph->QueryInterface(IID_PPV_ARGS(&pMediaControl));
	if (FAILED(hr))
		return hr;

	hr = pGraph->QueryInterface(IID_PPV_ARGS(&pMediaEvent));
	if (FAILED(hr))
		return hr;

	{
		// remove reference clock
		IMediaFilter *pMediaFilter = NULL;
		hr = pGraph->QueryInterface(IID_PPV_ARGS(&pMediaFilter));
		if (FAILED(hr))
			return hr;

		pMediaFilter->SetSyncSource(NULL);
		SafeRelease(&pMediaFilter);
	}

	if(pszInputFile != NULL)
	{
		IBaseFilter *pSourceFilter = NULL;

		hr = pGraph->AddSourceFilter(pszInputFile, L"Source", &pSourceFilter);
		if (FAILED(hr))
		{
			SafeRelease(&pSourceFilter);
			return hr;
		}

		hr = InitVideoGrabber(pSourceFilter);
		if (FAILED(hr))
		{
			SafeRelease(&pSourceFilter);
			return hr;
		}

		hr = InitAudioGrabber(pSourceFilter);
		if (FAILED(hr))
		{
			SafeRelease(&pSourceFilter);
			return hr;
		}

		SafeRelease(&pSourceFilter);
	}
	else
	{
		hr = pGraph->AddFilter(pUserSourceFilter, L"Source");
		if (FAILED(hr))
		{
			return hr;
		}

		hr = InitVideoGrabber(pUserSourceFilter);
		if (FAILED(hr))
		{
			return hr;
		}

		hr = InitAudioGrabber(pUserSourceFilter);
		if (FAILED(hr))
		{
			return hr;
		}
	}

	return hr;
}

HRESULT DSGraph::InitVideoGrabber(IBaseFilter *pSourceFilter)
{
	// Create the Sample Grabber filter.
	HRESULT hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pVideoGrabberFilter));
	if (FAILED(hr))
		return hr;

	hr = pGraph->AddFilter(pVideoGrabberFilter, L"Video Sample Grabber");
	if (FAILED(hr))
		return hr;

	hr = pVideoGrabberFilter->QueryInterface(IID_PPV_ARGS(&pVideoGrabber));
	if (FAILED(hr))
		return hr;

	{
		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(mt));
		mt.majortype = MEDIATYPE_Video;
		mt.subtype = MEDIASUBTYPE_RGB24;

		hr = pVideoGrabber->SetMediaType(&mt);
		if (FAILED(hr))
			return hr;

		FreeMediaType(mt);
	}

	hr = ConnectSampleGrabber(pGraph, pSourceFilter, pVideoGrabberFilter);

    if (FAILED(hr))
    {
        // Cannot connect the video grabber. Remove the filter from the graph.
		hr = pGraph->RemoveFilter(pVideoGrabberFilter);
		if (FAILED(hr))
			return hr;

		SafeRelease(&pVideoGrabberFilter);
		SafeRelease(&pVideoGrabber);
        return hr;
    }

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pVideoNullFilter));
	if (FAILED(hr))
		return hr;

	hr = pGraph->AddFilter(pVideoNullFilter, L"Null Filter");
	if (FAILED(hr))
		return hr;

	hr = ConnectFilters(pGraph, pVideoGrabberFilter, pVideoNullFilter);
	if (FAILED(hr))
		return hr;

	hr = SampleGrabberCB::CreateInstance(&pVideoGrabberCB);
	if (FAILED(hr))
		return hr;

	hr = pVideoGrabber->SetCallback(pVideoGrabberCB, CBMethod_Sample);
	if (FAILED(hr))
		return hr;

	return hr;
}


HRESULT DSGraph::InitAudioGrabber(IBaseFilter *pSourceFilter)
{
	// Create the Sample Grabber filter.
	HRESULT hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pAudioGrabberFilter));
	if (FAILED(hr))
		return hr;

	hr = pGraph->AddFilter(pAudioGrabberFilter, L"Audio Sample Grabber");
	if (FAILED(hr))
		return hr;

	hr = pAudioGrabberFilter->QueryInterface(IID_PPV_ARGS(&pAudioGrabber));
	if (FAILED(hr))
		return hr;

	{
		AM_MEDIA_TYPE mt;
		ZeroMemory(&mt, sizeof(mt));
		mt.majortype = MEDIATYPE_Audio;
		mt.subtype = MEDIASUBTYPE_PCM;

		hr = pAudioGrabber->SetMediaType(&mt);
		if (FAILED(hr))
			return hr;

		FreeMediaType(mt);
	}

	hr = ConnectSampleGrabber(pGraph, pSourceFilter, pAudioGrabberFilter);

    if (FAILED(hr))
    {
        // Cannot connect the audio grabber. Remove the filter from the graph.
		hr = pGraph->RemoveFilter(pAudioGrabberFilter);
		if (FAILED(hr))
			return hr;

		SafeRelease(&pAudioGrabberFilter);
		SafeRelease(&pAudioGrabber);
        return hr;
    }

	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pAudioNullFilter));
	if (FAILED(hr))
		return hr;

	hr = pGraph->AddFilter(pAudioNullFilter, L"Null Filter");
	if (FAILED(hr))
		return hr;

	hr = ConnectFilters(pGraph, pAudioGrabberFilter, pAudioNullFilter);
	if (FAILED(hr))
		return hr;

	hr = SampleGrabberCB::CreateInstance(&pAudioGrabberCB);
	if (FAILED(hr))
		return hr;

	hr = pAudioGrabber->SetCallback(pAudioGrabberCB, CBMethod_Sample);
	if (FAILED(hr))
		return hr;

	return hr;
}


HRESULT DSGraph::ConnectSampleGrabber(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest)
{
	if((NULL == pGraph) || (NULL == pSrc) || (NULL == pDest))
		return E_FAIL;

	// try to connect to source filter unconnected output pins
	{
		IEnumPins *pEnum = NULL;
		IPin *pPin = NULL;
		HRESULT hr = pSrc->EnumPins(&pEnum);
		if (FAILED(hr))
			return hr;

		while (pEnum->Next(1, &pPin, NULL) == S_OK)
		{
			PIN_DIRECTION thisPinDir;
			hr = pPin->QueryDirection(&thisPinDir);
			if ((thisPinDir == PINDIR_OUTPUT) && (SUCCEEDED(hr)))
			{
				IPin *pTmp = 0;
				hr = pPin->ConnectedTo(&pTmp);
				if (SUCCEEDED(hr) && (NULL != pTmp))  // Already connected, not the pin we want.
				{
					pTmp->Release();
				}
				else  // Unconnected, this is the pin we want.
				{
					hr = ConnectFilters(pGraph, pPin, pDest);
					if(SUCCEEDED(hr))
					{
						SafeRelease(&pPin);
						SafeRelease(&pEnum);
						return S_OK;
					}
				}
			}
			pPin->Release();
		}

		pEnum->Release();
	}


	// try to connect to filters connected to the source filter
	{
		IEnumPins *pEnum = NULL;
		IPin *pPin = NULL;
		HRESULT hr = pSrc->EnumPins(&pEnum);
		if (FAILED(hr))
			return hr;

		while (pEnum->Next(1, &pPin, NULL) == S_OK)
		{
			PIN_DIRECTION thisPinDir;
			hr = pPin->QueryDirection(&thisPinDir);
			if ((thisPinDir == PINDIR_OUTPUT) && (SUCCEEDED(hr)))
			{
				IPin *pTmp = NULL;
				hr = pPin->ConnectedTo(&pTmp);
				if (SUCCEEDED(hr) && (NULL != pTmp))  // Already connected, the pin we want.
				{
					PIN_INFO pinInfo;
					hr = pTmp->QueryPinInfo(&pinInfo);

					if(SUCCEEDED(hr))
					{
						IBaseFilter *pConnectedTo = pinInfo.pFilter;

						if(NULL != pConnectedTo)
						{
							hr = ConnectSampleGrabber(pGraph, pConnectedTo, pDest);

							if(SUCCEEDED(hr))
							{
								SafeRelease(&pTmp);
								SafeRelease(&pPin);
								SafeRelease(&pEnum);
								return S_OK;
							}
						}
					}

					pTmp->Release();
				}
			}
			pPin->Release();
		}

		pEnum->Release();
	}

	return E_FAIL;
}
