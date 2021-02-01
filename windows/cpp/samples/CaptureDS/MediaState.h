#pragma once

class MediaState
{
public:
	MediaState(void);
	~MediaState(void);

	IMediaControl* pMediaControl;
	IFilterGraph2* pGraph;
	ICaptureGraphBuilder2* pCaptureGraph;
	IBaseFilter* pAudioInput;
	IBaseFilter* pVideoInput;
	IBaseFilter* pSmartTee;
	IBaseFilter* pPreviewRenderer;

	IBaseFilter* pAudioGrabberFilter;
	IBaseFilter* pVideoGrabberFilter;
	ISampleGrabber* pAudioGrabber;
	ISampleGrabber* pVideoGrabber;

	IBaseFilter* pAudioNullRenderer;
	IBaseFilter* pVideoNullRenderer;

	DWORD dwROT;

	CRITICAL_SECTION transcoderSync;

	AM_MEDIA_TYPE VideoType;
	AM_MEDIA_TYPE AudioType;

	IAMDroppedFrames* pDroppedFrames;

	primo::avblocks::Transcoder *pTranscoder;

	void Reset(bool full);
};
