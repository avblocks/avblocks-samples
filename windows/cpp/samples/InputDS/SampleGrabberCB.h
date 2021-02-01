/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

class SampleGrabberCB: public ISampleGrabberCB
{
	long m_cRef;

public:
	// IUnknown interface
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

	// ISampleGrabberCB interface
	HRESULT STDMETHODCALLTYPE BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen);
	HRESULT STDMETHODCALLTYPE SampleCB(double SampleTime, IMediaSample *pSample);

protected:
	primo::codecs::MediaSample *m_mediaSample;
	primo::codecs::MediaBuffer *m_mediaBuffer;

	primo::avblocks::Transcoder *m_pTranscoder;
	int m_TranscoderInputIndex;

	const primo::error::ErrorInfo *m_pTranscoderError;
	IMediaControl				*m_pControl;

	CRITICAL_SECTION             *m_pTranscoderSync;

	bool ProcessSample(uint8_t *pBuf, uint32_t bufSize, double sampleTime);

	// how many times the callback has been called
    long m_SamplesProcessed;

	void Reset();

	SampleGrabberCB();
	~SampleGrabberCB();
public:
	static HRESULT CreateInstance(SampleGrabberCB **ppCB);

	void Init(primo::avblocks::Transcoder *pTranscoder, int socketNum, IMediaControl *pControl);

	void SetTranscoderSync(CRITICAL_SECTION *pCriticalSection);

	const primo::error::ErrorInfo *get_TranscoderError() const;
};

// Sample grabber callback method
enum CBMethod
{
	CBMethod_Sample = 0, // the original sample from the upstream filter
	CBMethod_Buffer = 1  // a copy of the sample of the upstream filter
};
