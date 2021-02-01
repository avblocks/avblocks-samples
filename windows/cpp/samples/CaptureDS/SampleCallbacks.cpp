#include "stdafx.h"
#include "SampleCallbacks.h"
#include "MediaState.h"

/// SampleGrabber Callback
bool SampleGrabberCB::ProcessSample(uint8_t *pBuf, uint32_t bufSize, double sampleTime)
{
	//DBG
	ATLTRACE2("sample streamNum:%d time:%f size:%d\r\n", StreamNumber, sampleTime, bufSize);

	bool pushResult = true;

	if (pMediaState && pMediaState->pTranscoder)
	{
		primo::avblocks::Transcoder* pEnc = pMediaState->pTranscoder;

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

		EnterCriticalSection(&(pMediaState->transcoderSync));

		pushResult = pEnc->push(StreamNumber, m_mediaSample);
		if (!pushResult)
		{
			ATLTRACE(pEnc->error()->message());
			ATLTRACE(" ");
			ATLTRACE(pEnc->error()->hint());
			ATLTRACE("\r\n");
		}

		LeaveCriticalSection(&(pMediaState->transcoderSync));

		if (!pushResult)
		{
			ATLTRACE(L"Push sample FAILED");
		}
	}

	if (pushResult)
		return true;

	bProcess = false;
	PostMessage(MainWindow, WM_STOP_CAPTURE, StreamNumber, 0);

	return false;	
}

HRESULT SampleGrabberCB::BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
{
	if (!bProcess)
		return E_FAIL;

	++SampleIndex;

	bool processed = ProcessSample(pBuffer, BufferLen, SampleTime);

	return processed ? S_OK : E_FAIL;
}

HRESULT SampleGrabberCB::SampleCB(double SampleTime, IMediaSample *pSample)
{
	if (!bProcess)
		return E_FAIL;

	// internal stats
	++SampleIndex;

	LONGLONG tStart, tEnd;
	pSample->GetMediaTime(&tStart, &tEnd);

	ATLASSERT(tStart < tEnd);
	ATLASSERT(tStart > lastMediaTime);

	SampleProcessed += tEnd - tStart;
	SampleDropped += tStart - lastMediaTime - 1;
	lastMediaTime = tEnd - 1;

	int dataLen = pSample->GetActualDataLength();
	BYTE*  pBuf;
	HRESULT hr = pSample->GetPointer(&pBuf);
	ATLASSERT(S_OK == hr);

	bool processed = ProcessSample(pBuf, dataLen, SampleTime);

	return processed ? S_OK : E_FAIL;
}

HRESULT SampleGrabberCB::QueryInterface(REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject)
{
	return S_FALSE;
}

ULONG SampleGrabberCB::AddRef(void)
{
	return 1;
}

ULONG SampleGrabberCB::Release(void)
{
	return 1;
}

SampleGrabberCB::SampleGrabberCB(PCWSTR name) : 
SampleIndex(0),
pMediaState(NULL),
StreamNumber(0),
MainWindow(NULL),
bProcess(true),
lastMediaTime(-1),
SampleProcessed(0),
SampleDropped(0),
m_mediaSample(NULL),
m_mediaBuffer(NULL)
{
	if (name)
	{
		wcscpy_s(this->name, 100, name);
	}
	else
	{
		wcscpy_s(this->name,100, L"SampleGrabberCB");
	}
}

void SampleGrabberCB::Reset()
{
	bProcess = true;

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

	SampleIndex = 0;
	pMediaState = NULL;

	SampleProcessed = 0;
	SampleDropped = 0;

	lastMediaTime = -1;
}

SampleGrabberCB::~SampleGrabberCB()
{
	Reset();
}
