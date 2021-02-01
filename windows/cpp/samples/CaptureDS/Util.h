#pragma once

void FreeMediaType(AM_MEDIA_TYPE& mt);
void DeleteMediaType(AM_MEDIA_TYPE* pmt);

HRESULT NukeDownstream(IFilterGraph* pGraph, IBaseFilter* pFilter);

HRESULT AddGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);
void RemoveGraphFromRot(DWORD pdwRegister);

HRESULT ConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc, IBaseFilter *pDest);
HRESULT GetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir, IPin **ppPin);
HRESULT GetPin(IBaseFilter *pFilter, PIN_DIRECTION pinDir, PCWSTR pinName, IPin **ppPin);

primo::codecs::ColorFormat::Enum GetColorFormat(GUID& videoSubtype);

template<class T> void DumpErrorState(T pobj)
{
	ATLTRACE2(L"Facility:%d ErrorCode:%d", pobj->error()->facility(), pobj->error()->code());
}
