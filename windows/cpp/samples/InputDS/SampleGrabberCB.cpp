/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "stdafx.h"
#include "SampleGrabberCB.h"

static DWORD WINAPI ThreadProcStopGraph( LPVOID lpParam ) 
{
	IMediaControl *pControl = (IMediaControl*) lpParam;
	HRESULT hr = pControl->Stop();
    return 0; 
} 

/// SampleGrabber Callback
bool SampleGrabberCB::ProcessSample(uint8_t *pBuf, uint32_t bufSize, double sampleTime)
{
	if(NULL == m_pTranscoder)
		return false;

	if (sampleTime < 0)
		sampleTime = 0;

	if(!m_mediaSample)
	{
		m_mediaSample = primo::avblocks::Library::createMediaSample();
	}

	if(m_mediaBuffer)
	{
		if(m_mediaBuffer->capacity() < bufSize)
		{
			m_mediaBuffer->release();
			m_mediaBuffer = NULL;
		}
	}

	if(!m_mediaBuffer)
	{
		m_mediaBuffer = primo::avblocks::Library::createMediaBuffer(bufSize);
		m_mediaSample->setBuffer(m_mediaBuffer);
	}

	memcpy(m_mediaBuffer->start(), pBuf, bufSize);

	m_mediaBuffer->setData(0, bufSize);
	m_mediaSample->setStartTime(sampleTime);

	// std::wcout << _T("transcoder.Push(stream: ") << m_TranscoderInputIndex << _T(", sampleTime: ") << sampleTime << _T(", sampleData: ") << bufSize << std::endl;

    // transcoder.Push() is not threads safe.
    // EnterCriticalSection ensure that only one thread is calling transcoder->push()

	if(m_pTranscoderSync)
		EnterCriticalSection(m_pTranscoderSync);

	bool_t bRes = m_pTranscoder->push(m_TranscoderInputIndex, m_mediaSample);

	if(!bRes)
	{
		if(m_pTranscoderError)
		{
			m_pTranscoderError->release();
			m_pTranscoderError = NULL;
		}

		m_pTranscoderError = m_pTranscoder->error()->clone();

		// call mediaControl.Stop() from a new thread otherwise it will deadlock
		CreateThread( NULL, 0, ThreadProcStopGraph, m_pControl, 0, NULL);
	}

	if(m_pTranscoderSync)
		LeaveCriticalSection(m_pTranscoderSync);

	return bRes ? true : false;
}

HRESULT SampleGrabberCB::BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
{
	if (m_pTranscoderError)
		return E_FAIL;

	m_SamplesProcessed += 1;

	bool processed = ProcessSample(pBuffer, BufferLen, SampleTime);

	return processed ? S_OK : E_FAIL;
}

HRESULT SampleGrabberCB::SampleCB(double SampleTime, IMediaSample *pSample)
{
	if (m_pTranscoderError)
		return E_FAIL;

	m_SamplesProcessed += 1;

	LONGLONG tStart, tEnd;
	HRESULT hr = pSample->GetMediaTime(&tStart, &tEnd);

	int dataLen = pSample->GetActualDataLength();

	BYTE*  pBuf = NULL;
	hr = pSample->GetPointer(&pBuf);
	assert(S_OK == hr);

	bool processed = ProcessSample(pBuf, dataLen, SampleTime);

	return processed ? S_OK : E_FAIL;
}

HRESULT SampleGrabberCB::QueryInterface(REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return S_FALSE;
}

STDMETHODIMP_(ULONG) SampleGrabberCB::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) SampleGrabberCB::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);

    if (cRef == 0)
    {
        delete this;
    }

    return cRef;
}

// Create a new instance of the object.
HRESULT SampleGrabberCB::CreateInstance(SampleGrabberCB **ppCB)
{
	if(ppCB)
	{
		*ppCB = new SampleGrabberCB();
	}

    return S_OK;
}

SampleGrabberCB::SampleGrabberCB():
			m_cRef(1),
			m_SamplesProcessed(0),
			m_TranscoderInputIndex(0),
			m_mediaSample(NULL),
			m_mediaBuffer(NULL),
			m_pTranscoder(NULL),
			m_pTranscoderError(NULL),
			m_pControl(NULL),
			m_pTranscoderSync(NULL)
{

}

void SampleGrabberCB::Reset()
{
	if(m_mediaSample)
	{
		m_mediaSample->release();
		m_mediaSample = NULL;
	}

	if(m_mediaBuffer)
	{
		m_mediaBuffer->release();
		m_mediaBuffer = NULL;
	}

	if(m_pTranscoderError)
	{
		m_pTranscoderError->release();
		m_pTranscoderError = NULL;
	}

	m_SamplesProcessed = 0;

	m_pTranscoder = NULL;
	m_TranscoderInputIndex = 0;

	m_pControl = NULL;

	m_pTranscoderSync = NULL;
}

void SampleGrabberCB::SetTranscoderSync(CRITICAL_SECTION *pCriticalSection)
{
	m_pTranscoderSync = pCriticalSection;
}

SampleGrabberCB::~SampleGrabberCB()
{
	Reset();
}

const primo::error::ErrorInfo *SampleGrabberCB::get_TranscoderError() const
{
	return m_pTranscoderError;
}

void SampleGrabberCB::Init(primo::avblocks::Transcoder *pTranscoder, int transcoderInputIndex, IMediaControl *pControl)
{
	Reset();

	m_pTranscoder = pTranscoder;
	m_TranscoderInputIndex = transcoderInputIndex;
	m_pControl = pControl;
}
