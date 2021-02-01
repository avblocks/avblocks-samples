#pragma once

class MediaState;

class SampleGrabberCB : public ISampleGrabberCB
{
public:

	// IUnknown interface
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject);
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

	// ISampleGrabberCB interface
	HRESULT STDMETHODCALLTYPE BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen);
	HRESULT STDMETHODCALLTYPE SampleCB(double SampleTime, IMediaSample *pSample);

	// This class
protected:
	WCHAR name[100];

	primo::codecs::MediaSample *m_mediaSample;
	primo::codecs::MediaBuffer *m_mediaBuffer;

	bool bProcess;

	LONGLONG lastMediaTime;
    
public:
	SampleGrabberCB(PCWSTR name);
	~SampleGrabberCB();
	void Reset();
	bool ProcessSample(uint8_t *pBuf, uint32_t bufSize, double sampleTime);
	
	MediaState* pMediaState;
	unsigned int StreamNumber;
	HWND MainWindow;
	
	// how many times the callback has been called
    LONGLONG SampleIndex;

    // how many samples are processed (based on the media time)
    LONGLONG SampleProcessed;

    // how many samples are dropped (based on the media time)
    LONGLONG SampleDropped;
};

// Sample grabber callback method
enum CBMethod
{
	CBMethodSample = 0, // the original sample from the upstream filter
	CBMethodBuffer = 1  // a copy of the sample of the upstream filter
};

