/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

#include <windows.h>
#include <MMReg.h>
#include <vector>


class WinMMAudioRenderCallback
{
public:
	virtual bool NextAudioBuffer(void* buffer, int* length, int maxBufferLength) = 0;
	virtual void PlaybackProgress(__int64 totalDuration, __int64 lastBufferDuration, __int64 bytesPerSec) = 0;
};

class WinMMAudioRender
{
public:
	WinMMAudioRender();
	~WinMMAudioRender();

	bool Start(WinMMAudioRenderCallback* provider, int sampleRate, int channels, int bitsPerSample);
	void Stop();

	bool Started();

private:
	WinMMAudioRenderCallback* _callback;
	HWAVEOUT _waveout;
	WAVEFORMATEXTENSIBLE _fmt;
	std::vector<WAVEHDR*> _buffers;
	bool _started;
	__int64 _totalDuration;
	int _bufferLength;

	void CreateBuffers(int buffersCount, int bufferLength);
	void ReleaseBuffers();

	static void CALLBACK CallbackProc(HWAVEOUT waveout, UINT msg, DWORD_PTR userData, DWORD_PTR p1, DWORD_PTR p2);
};
