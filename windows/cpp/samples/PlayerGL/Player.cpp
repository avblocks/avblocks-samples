/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#include "Player.h"
#include <tchar.h>
#include <algorithm>

using namespace primo::codecs;
using namespace primo::avblocks;

Player::Player(HINSTANCE hInstance, LPCTSTR windowTitle) :
	_hInstance(hInstance),
	_windowTitle(windowTitle),
	_hDecoderThread(NULL),
	_transcoder(NULL),
	_videoStreamInfo(NULL),
	_audioStreamInfo(NULL),
	_videoStreamIndex(-1),
	_audioStreamIndex(-1),
	_decoderEOS(false),
	_cancellationPending(false),
	_audioRender(NULL),
	_videoRender(NULL),
	_baseTime(0),
	_audioStartPTS(0),
	_videoStartPTS(0),
	_audioElapsedTime(0)
{

}

Player::~Player()
{
	Close();
}

void Player::Close()
{
	if (NULL != _transcoder)
	{
		_transcoder->release();
		_transcoder = NULL;
	}

	if (NULL != _videoStreamInfo)
	{
		_videoStreamInfo->release();
		_videoStreamInfo = NULL;
	}

	if (NULL != _audioStreamInfo)
	{
		_audioStreamInfo->release();
		_audioStreamInfo = NULL;
	}

	_hDecoderThread = NULL;

	_videoStreamIndex = -1;
	_audioStreamIndex = -1;
	_decoderEOS = false;
	_cancellationPending = false;
}

bool Player::ConfigureStreams(LPCTSTR filePath)
{
	MediaInfo* mediaInfo = Library::createMediaInfo();

	mediaInfo->inputs()->at(0)->setFile(filePath);

	if (!(mediaInfo->open()))
	{
		mediaInfo->release();
		return false;
	}

	// configure streams
    for (int i = 0; i < mediaInfo->outputs()->count(); i++)
    {
        auto socket = mediaInfo->outputs()->at(i);
        for (int j = 0; j < socket->pins()->count(); j++)
        {
            StreamInfo* psi = socket->pins()->at(j)->streamInfo();

            MediaType::Enum mediaType = psi->mediaType();

            if ((MediaType::Video == mediaType) && (NULL == _videoStreamInfo))
            {
                VideoStreamInfo* vsi = static_cast<VideoStreamInfo*>(psi);
                _videoStreamInfo = Library::createVideoStreamInfo();

                _videoStreamInfo->setFrameWidth(vsi->frameWidth());
                _videoStreamInfo->setFrameHeight(vsi->frameHeight());
                _videoStreamInfo->setDisplayRatioWidth(vsi->displayRatioWidth());
                _videoStreamInfo->setDisplayRatioHeight(vsi->displayRatioHeight());
                _videoStreamInfo->setFrameRate(vsi->frameRate());
            }

            if ((MediaType::Audio == mediaType) && (NULL == _audioStreamInfo))
            {
                AudioStreamInfo* asi = static_cast<AudioStreamInfo*>(psi);
                _audioStreamInfo = Library::createAudioStreamInfo();

                _audioStreamInfo->setBitsPerSample(asi->bitsPerSample());
                _audioStreamInfo->setChannels(asi->channels());
                _audioStreamInfo->setSampleRate(asi->sampleRate());
            }
        }
    }

	mediaInfo->release();
	mediaInfo = NULL;

	return true;
}


bool Player::Open(LPCTSTR filePath)
{
	Close();

	if (!ConfigureStreams(filePath))
	{
		Close();
		return false;
	}

	_transcoder = Library::createTranscoder();
	// In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
	_transcoder->setAllowDemoMode(TRUE);

	// Configure input
	{
		MediaSocket *socket(Library::createMediaSocket());
		socket->setFile(filePath);
		_transcoder->inputs()->add(socket);
		socket->release();
	}

	MediaPin *pin(Library::createMediaPin());

	// Configure video output
	if (_videoStreamInfo != NULL)
	{
		_videoStreamInfo->setColorFormat(ColorFormat::BGR24);
		_videoStreamInfo->setFrameBottomUp(TRUE);
		_videoStreamInfo->setStreamType(StreamType::UncompressedVideo);
		_videoStreamInfo->setScanType(ScanType::Progressive);

		const int displayWidth = GetSystemMetrics(SM_CXSCREEN);
		const int displayHeight = GetSystemMetrics(SM_CYSCREEN);

		if ((_videoStreamInfo->frameWidth() > displayWidth) ||
			((_videoStreamInfo->frameHeight() > displayHeight)))
		{
			// resize the video
			double displayAspect = (double)displayWidth / (double)displayHeight;
			double videoAspect = (double)_videoStreamInfo->displayRatioWidth() / (double)_videoStreamInfo->displayRatioHeight();

			int width = 0;
			int height = 0;

			if (videoAspect < displayAspect)
			{
				width = displayWidth;
				height = static_cast<int>(displayWidth / videoAspect);
			}
			else
			{
				width = static_cast<int>(displayHeight * videoAspect);
				height = displayHeight;
			}

			width += calculatePadding(width, 2);
			height += calculatePadding(height, 2);

			_videoStreamInfo->setFrameWidth(width);
			_videoStreamInfo->setFrameHeight(height);

			pin->params()->addInt(primo::avblocks::Param::Video::Resize::InterpolationMethod, primo::codecs::InterpolationMethod::Linear);
		}

		pin->setStreamInfo(_videoStreamInfo);

		MediaSocket *socket(Library::createMediaSocket());
		socket->setStreamType(StreamType::UncompressedVideo);
		socket->pins()->add(pin);

		_videoStreamIndex = _transcoder->outputs()->count();
		_transcoder->outputs()->add(socket);

		pin->release();
		socket->release();
	}

	// Configure audio output
	if (_audioStreamInfo != NULL)
	{
		_audioStreamInfo->setBitsPerSample(16);

		// WinMM audio render supports only mono and stereo
		if (_audioStreamInfo->channels() > 2)
			_audioStreamInfo->setChannels(2);

		_audioStreamInfo->setStreamType(StreamType::LPCM);

		MediaPin *pin(Library::createMediaPin());
		pin->setStreamInfo(_audioStreamInfo);

		MediaSocket *socket(Library::createMediaSocket());
		socket->setStreamType(StreamType::LPCM);
		socket->pins()->add(pin);

		_audioStreamIndex = _transcoder->outputs()->count();
		_transcoder->outputs()->add(socket);

		pin->release();
		socket->release();
	}

	if (!_transcoder->open())
	{
		Close();
		return false;
	}

	return true;
}

void Player::StartDecoderThread()
{
	_decoderEOS = false;
	_hDecoderThread = CreateThread(NULL, 0, DecoderThreadProc, this, NULL, NULL);
}

void Player::JoinDecoderThread()
{
	if (NULL != _hDecoderThread)
	{
		WaitForSingleObject(_hDecoderThread, INFINITE);
		CloseHandle(_hDecoderThread);
		_hDecoderThread = NULL;
	}
}

DWORD WINAPI Player::DecoderThreadProc(LPVOID data)
{
	Player* instance = (Player*)data;
	return instance->DecoderThread();
}

DWORD Player::DecoderThread()
{
	while (!_decoderEOS && !_cancellationPending)
	{
		if (IsAVQueueFull())
		{
			Sleep(1);
			continue;
		}

		int32_t index = -1;
		primo::codecs::MediaSample *mediaSample = primo::avblocks::Library::createMediaSample();
		if (_transcoder->pull(index, mediaSample))
		{
			ScopedLock lock(&_csAVQueue);

			if (index == _videoStreamIndex)
			{
				_videoQueue.push_back(mediaSample);
			}
			else if (index == _audioStreamIndex)
			{
				_audioQueue.push_back(mediaSample);
			}
			else
			{
				mediaSample->release();
			}
		}
		else
		{
			_decoderEOS = true;
			mediaSample->release();
		}
	}

	return 0;
}

void Player::ClearAVQueue()
{
	ScopedLock lock(&_csAVQueue);

	{
		for (samplelist::iterator it = _audioQueue.begin(); it != _audioQueue.end(); ++it)
			(*it)->release();

		_audioQueue.clear();
	}

	{
		for (samplelist::iterator it = _videoQueue.begin(); it != _videoQueue.end(); ++it)
			(*it)->release();

		_videoQueue.clear();
	}
}

void Player::FillAVQueue()
{
	while (!IsAVQueueFull() && !_decoderEOS && !_cancellationPending)
	{
		Sleep(5);
		continue;
	}
}

void Player::StarRenders()
{
	// set default width and height
	int wndWidth = 800;
	int wndHeight = 600;

	_videoRender = new OpenGLVideoRender(_hInstance, _windowTitle, wndWidth, wndHeight);

	if (_videoStreamInfo != NULL)
	{
		_videoRender->SetDisplayAspect(_videoStreamInfo->displayRatioWidth(), _videoStreamInfo->displayRatioHeight());
		_videoRender->Start();
	}

	if (_audioStreamInfo != NULL)
	{
		_audioRender = new WinMMAudioRender();
		_audioRender->Start(this, _audioStreamInfo->sampleRate(), _audioStreamInfo->channels(), _audioStreamInfo->bitsPerSample());
	}
}

void Player::StopRenders()
{
	if (NULL != _videoRender)
	{
		delete _videoRender;
		_videoRender = NULL;
	}

	if (NULL != _audioRender)
	{
		delete _audioRender;
		_audioRender = NULL;
	}
}

bool Player::IsInputProcessed()
{
	ScopedLock lock(&_csAVQueue);
	return (_audioQueue.size() == 0) && (_videoQueue.size() == 0) && _decoderEOS;
}

void Player::RefreshRenders()
{
	ScopedLock lock(&_csAVQueue);

	if (NULL != _videoRender)
	{
		if (_videoQueue.size() > 0)
		{
			MediaSample *pSample = *_videoQueue.begin();

			if(GetClock() >= pSample->startTime())
			{
				MediaBuffer *pBuffer = pSample->buffer();

				_videoRender->SetFrame(pBuffer->data(), pBuffer->dataSize(), _videoStreamInfo->frameWidth(), _videoStreamInfo->frameHeight());

				_videoRender->Draw();

				pSample->release();
				_videoQueue.pop_front();
			}
		}
	}
}

void Player::InitClock()
{
	ScopedLock lock(&_csAVQueue);

	_audioStartPTS = -1.0;
	_videoStartPTS = -1.0;
	_baseTime = clock();
	_audioElapsedTime = 0;

	if(_audioQueue.size() > 0)
		_audioStartPTS = (*_audioQueue.begin())->startTime();

	if(_videoQueue.size() > 0)
		_videoStartPTS = (*_videoQueue.begin())->startTime();
}

void Player::EventLoop()
{
	StartDecoderThread();
	FillAVQueue();
	InitClock();
	StarRenders();

	{
		MSG msg;
		_cancellationPending = false;

		while (!_cancellationPending && !IsInputProcessed())
		{
			// check for messages
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					_cancellationPending = true;
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			RefreshRenders();

			Sleep(1);
		}
	}

	JoinDecoderThread();
	StopRenders();

	ClearAVQueue();
}

bool Player::IsAVQueueFull()
{
	ScopedLock lock(&_csAVQueue);

	int maxVideoSamples;
	double maxVideoDuration;
	double maxAudioDuration;

	if(isHdVideo())
	{
		// For HD video limits are lower.
		// The AV decoded queue is full if:
		// 10 video frames
		// 0.2 seconds video
		// 1.0 seconds audio
		maxVideoSamples = 10;
		maxVideoDuration = 0.2;
		maxAudioDuration = 1.0;
	}
	else
	{
		// The AV decoded queue is full if:
		// 50 video frames
		// 3 seconds video
		// 6 seconds audio
		maxVideoSamples = 50;
		maxVideoDuration = 3;
		maxAudioDuration = 6;
	}

	if (_videoQueue.size() > maxVideoSamples)
		return true;

	if(_videoQueue.size() > 1)
	{
		double duration = (*_videoQueue.rbegin())->startTime() - (*_videoQueue.begin())->startTime();
		if(duration > maxVideoDuration)
			return true;
	}

	if(_audioQueue.size() > 1)
	{
		double duration = (*_audioQueue.rbegin())->startTime() - (*_audioQueue.begin())->startTime();
		if(duration > maxAudioDuration)
			return true;
	}

	return false;
}

// returns true if video resolution is > 1280x720
bool Player::isHdVideo()
{
	if(!_videoStreamInfo)
		return false;

	return (_videoStreamInfo->frameWidth() > 1280) || (_videoStreamInfo->frameHeight() > 720);
}

double Player::GetClock()
{
	ScopedLock lock(&_csClock);

	double baseTimeElapsed = (double)(clock() - _baseTime) / (double)(CLOCKS_PER_SEC);

	if(_audioRender)
	{
		return _audioStartPTS + _audioElapsedTime + baseTimeElapsed;
	}
	else
	{
		return _videoStartPTS + baseTimeElapsed;
	}
}

bool Player::NextAudioBuffer(void* buffer, int* length, int maxBufferLength)
{
	if (_cancellationPending)
	{
		*length = 0;
		return false;
	}

	ScopedLock lock(&_csAVQueue);

	int bytesWritten = 0;

	while ((_audioQueue.size() > 0) && (bytesWritten < maxBufferLength))
	{
		MediaSample *pMediaSample = *_audioQueue.begin();
		MediaBuffer *pMediaBuffer = pMediaSample->buffer();

		int chunk = std::min<int>(pMediaBuffer->dataSize(), maxBufferLength - bytesWritten);
		memcpy((char*)buffer + bytesWritten, pMediaBuffer->data(), chunk);
		bytesWritten += chunk;

		{
			int newDataOffset = pMediaBuffer->dataOffset() + chunk;
			int newDataSize = pMediaBuffer->dataSize() - chunk;
			if (0 == newDataSize)
				newDataOffset = 0;

			pMediaBuffer->setData(newDataOffset, newDataSize);
		}

		if (pMediaBuffer->dataSize() == 0)
		{
			pMediaSample->release();
			_audioQueue.pop_front();
		}
	}

	*length = bytesWritten;
	return true;
}

void Player::PlaybackProgress(__int64 totalDuration, __int64 lastBufferDuration, __int64 bytesPerSec)
{
	ScopedLock lock(&_csClock);

	_audioElapsedTime = (double)totalDuration / (double)bytesPerSec;
	_baseTime = clock();
}
