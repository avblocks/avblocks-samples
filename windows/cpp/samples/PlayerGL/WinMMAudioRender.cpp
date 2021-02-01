/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "WinMMAudioRender.h"

#include <ks.h>
#include <assert.h>
#include <ksmedia.h>

#pragma comment(lib, "winmm")


WinMMAudioRender::WinMMAudioRender() :
	_callback(NULL), 
	_waveout(NULL),
	_started(false),
	_totalDuration(0),
	_bufferLength(0)
{

}

WinMMAudioRender::~WinMMAudioRender()
{
	Stop();
}

bool WinMMAudioRender::Started()
{
	return _started;
}

void WinMMAudioRender::CreateBuffers(int buffersCount, int bufferLength)
{
	_bufferLength = bufferLength;

	for (int i = 0; i < buffersCount; i++)
	{
		WAVEHDR* hdr = (WAVEHDR*)calloc(1, sizeof(WAVEHDR));
		hdr->dwBufferLength = bufferLength;
		hdr->lpData = (LPSTR)malloc(bufferLength);
		MMRESULT result = waveOutPrepareHeader(_waveout, hdr, sizeof(WAVEHDR));
		assert(MMSYSERR_NOERROR == result);
		_buffers.push_back(hdr);
	}
}

void WinMMAudioRender::ReleaseBuffers()
{
	for (unsigned int i = 0; i < _buffers.size(); i++)
	{
		WAVEHDR* hdr = _buffers[i];
		MMRESULT result = waveOutUnprepareHeader(_waveout, hdr, sizeof(WAVEHDR));
		assert(MMSYSERR_NOERROR == result);
		free(hdr->lpData);
		free(hdr);
	}

	_buffers.clear();
}

void WinMMAudioRender::Stop()
{
	if (NULL == _waveout)
		return;

	_started = false;

	MMRESULT result = waveOutReset(_waveout);
	assert(MMSYSERR_NOERROR == result);

	ReleaseBuffers();

	result = waveOutClose(_waveout);
	assert(MMSYSERR_NOERROR == result);

	_waveout = NULL;
	_callback = NULL;
	memset(&_fmt, 0, sizeof(_fmt));
}

bool WinMMAudioRender::Start(WinMMAudioRenderCallback* callback, int sampleRate, int channels, int bitsPerSample)
{
	Stop();

	if (NULL == callback)
		return false;

	if ((0 == sampleRate) ||  (0 == channels) || (0 == bitsPerSample))
		return false;

	memset(&_fmt, 0, sizeof(_fmt));
	_callback = callback;

	_fmt.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE);
	_fmt.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	_fmt.Format.nChannels = channels;
	_fmt.Format.nSamplesPerSec = sampleRate;
	_fmt.Format.wBitsPerSample = bitsPerSample;
	_fmt.Format.nBlockAlign = (_fmt.Format.wBitsPerSample >> 3) * _fmt.Format.nChannels;
	_fmt.Format.nAvgBytesPerSec = _fmt.Format.nBlockAlign * _fmt.Format.nSamplesPerSec;
	_fmt.Samples.wValidBitsPerSample = _fmt.Format.wBitsPerSample;
	_fmt.SubFormat = bitsPerSample < 32 ? KSDATAFORMAT_SUBTYPE_PCM : KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;

	switch (channels)
	{
		case 1: _fmt.dwChannelMask = KSAUDIO_SPEAKER_MONO; break;
		case 2: _fmt.dwChannelMask = KSAUDIO_SPEAKER_STEREO; break;
		default: return false; // only mono and stereo are supported
	}
	
	MMRESULT result = waveOutOpen(&_waveout, WAVE_MAPPER, &_fmt.Format, (DWORD_PTR)CallbackProc, (DWORD_PTR) this, CALLBACK_FUNCTION);
	if (result != MMSYSERR_NOERROR)
	{
		_waveout = NULL;
		return false;
	}

	_started = true;
	_totalDuration = 0;

	CreateBuffers(5, _fmt.Format.nBlockAlign * (_fmt.Format.nSamplesPerSec / 50)); // 5 buffers, 20ms each buffer

	result = waveOutPause(_waveout);
	assert(MMSYSERR_NOERROR == result);

	// write the empty buffers
	for (unsigned int i = 0; i < _buffers.size(); i++)
	{
		WAVEHDR* hdr = _buffers[i];
		hdr->dwBufferLength = 0;
		result = waveOutWrite(_waveout, hdr, sizeof(WAVEHDR));
		assert(MMSYSERR_NOERROR == result);
	}

	result = waveOutRestart(_waveout);
	assert(MMSYSERR_NOERROR == result);

	return true;
}

void CALLBACK WinMMAudioRender::CallbackProc(HWAVEOUT waveout, UINT msg, DWORD_PTR userData, DWORD_PTR p1, DWORD_PTR p2)
{
	switch (msg)
	{
	case WOM_DONE:
		WinMMAudioRender* instance = (WinMMAudioRender*)userData;
		WAVEHDR* hdr = (WAVEHDR*)p1;

		if (hdr->dwBufferLength > 0)
		{
			instance->_totalDuration += hdr->dwBufferLength;
			instance->_callback->PlaybackProgress(instance->_totalDuration, hdr->dwBufferLength, instance->_fmt.Format.nAvgBytesPerSec);
		}

		// if we are still playing get a new audio buffer
		if (instance->_started)
		{
			int length = 0;

			if (instance->_callback->NextAudioBuffer((void*)hdr->lpData, &length, instance->_bufferLength))
			{
				assert(length <= instance->_bufferLength);
				hdr->dwBufferLength = length;
				MMRESULT result = waveOutWrite(waveout, hdr, sizeof(WAVEHDR));
				assert(MMSYSERR_NOERROR == result);
			}
			else
			{
				// end of stream read
				instance->_started = false;
			}
		}
	}
}
