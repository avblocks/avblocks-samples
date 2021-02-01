#include "StdAfx.h"

// from DShow base libraries
void FreeMediaType(AM_MEDIA_TYPE& mt)
{
    if (mt.cbFormat != 0) {
        CoTaskMemFree((PVOID)mt.pbFormat);

        // Strictly unnecessary but tidier
        mt.cbFormat = 0;
        mt.pbFormat = NULL;
    }
    if (mt.pUnk != NULL) {
        mt.pUnk->Release();
        mt.pUnk = NULL;
    }
}

void DeleteMediaType(AM_MEDIA_TYPE* pmt)
{
    if (pmt == NULL)
        return;

    FreeMediaType(*pmt);
    CoTaskMemFree((PVOID)pmt);
}

// Tear down everything downstream of a given filter
HRESULT NukeDownstream(IFilterGraph* pGraph, IBaseFilter* pFilter)
{
	if (!pFilter)
		return E_FAIL;

	IEnumPins* pEnumPins = NULL;
	IPin* pPins = NULL;

	HRESULT hr = pFilter->EnumPins(&pEnumPins);
	if (FAILED(hr))
		return hr;

	pEnumPins->Reset(); // start at the first pin

	while (pEnumPins->Next(1, &pPins, NULL) == S_OK)
	{
		if (pPins)
		{
			PIN_DIRECTION pindir;
			pPins->QueryDirection(&pindir);
			if (pindir == PINDIR_OUTPUT)
			{
				IPin* pTo = NULL;
				pPins->ConnectedTo(&pTo);
				if (pTo)
				{
					PIN_INFO pi;
					hr = pTo->QueryPinInfo(&pi);

					if (hr == S_OK)
					{
						NukeDownstream(pGraph, pi.pFilter);

						pGraph->Disconnect(pTo);
						pGraph->Disconnect(pPins);
						pGraph->RemoveFilter(pi.pFilter);

						SafeRelease(pi.pFilter);


					}
					SafeRelease(pTo);
				}
			}
			SafeRelease(pPins);
		}
	}


	SafeRelease(pEnumPins);
	return S_OK;
}

HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
	IMoniker * pMoniker;
	IRunningObjectTable *pROT;
	HRESULT hr;

	if (!pUnkGraph || !pdwRegister)
		return E_POINTER;

	if (FAILED(GetRunningObjectTable(0, &pROT)))
		return E_FAIL;

	WCHAR wsz[256] = {0};
	swprintf_s(wsz,256, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());

	hr = CreateItemMoniker(L"!", wsz, &pMoniker);
	if (SUCCEEDED(hr)) 
	{
		// Use the ROTFLAGS_REGISTRATIONKEEPSALIVE to ensure a strong reference
		// to the object.  Using this flag will cause the object to remain
		// registered until it is explicitly revoked with the Revoke() method.
		//
		// Not using this flag means that if GraphEdit remotely connects
		// to this graph and then GraphEdit exits, this object registration 
		// will be deleted, causing future attempts by GraphEdit to fail until
		// this application is restarted or until the graph is registered again.
		hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph, 
			pMoniker, pdwRegister);
		pMoniker->Release();
	}

	pROT->Release();
	return hr;
}


// Removes a filter graph from the Running Object Table
void RemoveGraphFromRot(DWORD pdwRegister)
{
	IRunningObjectTable *pROT;

	if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) 
	{
		pROT->Revoke(pdwRegister);
		pROT->Release();
	}
}

HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest)
{
	if ((!pGraph) || (!pSrc) || (!pDest))
	{
		return E_POINTER;
	}

	// Find an output pin on the first filter.
	IPin *pOut = 0;
	HRESULT hr = GetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
	if (FAILED(hr)) 
	{
		return hr;
	}

	// Find an input pin on the downstream filter.
	IPin *pIn = 0;
	hr = GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
	if (FAILED(hr))
	{
		return hr;
	}
	// Try to connect them.
	hr = pGraph->ConnectDirect(pOut, pIn, NULL);
	pIn->Release();
	pOut->Release();
	return hr;
}

HRESULT GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin)
{
	*ppPin = 0;
	IEnumPins *pEnum = 0;
	IPin *pPin = 0;
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
	{
		return hr;
	}
	while (pEnum->Next(1, &pPin, NULL) == S_OK)
	{
		PIN_DIRECTION ThisPinDir;
		pPin->QueryDirection(&ThisPinDir);
		if (ThisPinDir == PinDir)
		{
			IPin *pTmp = 0;
			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))  // Already connected, not the pin we want.
			{
				pTmp->Release();
			}
			else  // Unconnected, this is the pin we want.
			{
				pEnum->Release();
				*ppPin = pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}
	pEnum->Release();
	// Did not find a matching pin.
	return E_FAIL;
}

HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION pinDir, PCWSTR pinName, IPin **ppPin)
{
	*ppPin = 0;
	IEnumPins* pEnumPins;
	HRESULT hr = pFilter->EnumPins(&pEnumPins);
	if (FAILED(hr))
	{
		return hr;
	}

	IPin* pPin;
	while (pEnumPins->Next(1, &pPin, NULL) == S_OK)
	{
		PIN_INFO pinInfo;
		hr = pPin->QueryPinInfo(&pinInfo);
		if (hr == S_OK)
		{
			bool found = (pinInfo.dir == pinDir) && (0 == _wcsicmp(pinInfo.achName, pinName));
			if (pinInfo.pFilter)
				pFilter->Release();

			if (found)
			{
				pEnumPins->Release();
				*ppPin = pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}
	pEnumPins->Release();

	return E_FAIL;
}

primo::codecs::ColorFormat::Enum GetColorFormat(GUID& videoSubtype)
{
	using namespace primo::codecs;

	struct ColorSpaceEntry
	{
		GUID videoSubType;
		ColorFormat::Enum colorFormat;
	};

	static ColorSpaceEntry ColorSpaceTab[] = {
		
		{ MEDIASUBTYPE_RGB24,		ColorFormat::BGR24		},
		{ MEDIASUBTYPE_ARGB32,		ColorFormat::BGRA32 }, // with alpha
		{ MEDIASUBTYPE_RGB32,		ColorFormat::BGR32 },
		{ MEDIASUBTYPE_RGB565,		ColorFormat::BGR565 },
		{ MEDIASUBTYPE_ARGB1555,	ColorFormat::BGR555 }, // with alpha
		{ MEDIASUBTYPE_RGB555,		ColorFormat::BGR555 }, // with alpha
		{ MEDIASUBTYPE_ARGB4444,	ColorFormat::BGR444 }, // with alpha
		
		{ MEDIASUBTYPE_YV12,		ColorFormat::YV12		},
		{ MEDIASUBTYPE_I420,		ColorFormat::YUV420		},
		{ MEDIASUBTYPE_IYUV,		ColorFormat::YUV420		},
		{ MEDIASUBTYPE_NV12,		ColorFormat::NV12		},
		{ MEDIASUBTYPE_UYVY,		ColorFormat::UYVY		},
		{ MEDIASUBTYPE_Y411,		ColorFormat::Y411		},
		{ MEDIASUBTYPE_Y41P,		ColorFormat::Y41P		},
		{ MEDIASUBTYPE_YUY2,		ColorFormat::YUY2		},
		{ MEDIASUBTYPE_YVU9,		ColorFormat::YVU9		},
	};

	for (int i = 0; i < sizeof(ColorSpaceTab) / sizeof(ColorSpaceEntry); i++)
	{
		if (ColorSpaceTab[i].videoSubType == videoSubtype)
			return ColorSpaceTab[i].colorFormat;
	}

	return ColorFormat::Unknown;
}
