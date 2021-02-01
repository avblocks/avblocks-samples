/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#pragma once

#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <list>
#include <tchar.h>
#include "WinMMAudioRender.h"
#include "OpenGLVideoRender.h"

#include <AVBlocks.h>
#include <PrimoReference++.h>


class CriticalSection
{
public:
	CriticalSection()
	{
		_cs = new CRITICAL_SECTION;
		::InitializeCriticalSection(_cs);
	}

	~CriticalSection()
	{
		::DeleteCriticalSection(_cs);
		delete _cs;
	}

	void Lock()
	{
		::EnterCriticalSection(_cs);
	}

	void Unlock()
	{
		::LeaveCriticalSection(_cs);
	}

private:
	CRITICAL_SECTION* _cs;
};


class ScopedLock
{
	CriticalSection *_cs;
public:
	ScopedLock(CriticalSection *cs):_cs(cs)
	{
		_cs->Lock();
	}

	~ScopedLock()
	{
		_cs->Unlock();
	}
};


class Player : WinMMAudioRenderCallback
{
	HINSTANCE _hInstance;
	LPCTSTR _windowTitle;

	bool _cancellationPending;

	primo::avblocks::Transcoder *_transcoder;
	primo::codecs::VideoStreamInfo *_videoStreamInfo;
	primo::codecs::AudioStreamInfo *_audioStreamInfo;
	int _videoStreamIndex;
	int _audioStreamIndex;

	bool isHdVideo();

	bool ConfigureStreams(LPCTSTR filePath);

	WinMMAudioRender  *_audioRender;
	OpenGLVideoRender *_videoRender;

	//WinMMAudioRenderCallback
	virtual bool NextAudioBuffer(void* buffer, int* length, int maxBufferLength);
	virtual void PlaybackProgress(__int64 totalDuration, __int64 lastBufferDuration, __int64 bytesPerSec);

	HANDLE _hDecoderThread;
	static DWORD WINAPI DecoderThreadProc(LPVOID parameter);
	DWORD DecoderThread();
	void StartDecoderThread();
	void JoinDecoderThread();

	void FillAVQueue();
	void StarRenders();
	void StopRenders();
	void RefreshRenders();

	bool IsInputProcessed();

	CriticalSection _csAVQueue;
	CriticalSection _csClock;

	typedef std::list<primo::codecs::MediaSample *> samplelist;
	samplelist _audioQueue;
	samplelist _videoQueue;
	bool _decoderEOS;
	bool IsAVQueueFull();
	void ClearAVQueue();

	void InitClock();
	double GetClock();

	double _audioStartPTS;
	double _videoStartPTS;

	double _audioElapsedTime;
	clock_t _baseTime;

	int calculatePadding(int val, int granularity)
	{
		return (granularity - val % granularity) % granularity;
	}

public:
	Player(HINSTANCE hInstance, LPCTSTR windowTitle);

	~Player();

	bool Open(LPCTSTR filePath);
	void Close();

	void EventLoop();
};
