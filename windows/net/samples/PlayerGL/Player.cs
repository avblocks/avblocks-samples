/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware;
using PrimoSoftware.AVBlocks;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace PlayerGLSample
{

class Player : WinMMAudioRenderCallback, IDisposable
{
    #region Disposable impl
    bool _disposed = false;

    /*
     * Specifies whether the player will use Transcoder.PullUnmanaged or Transcoder.Pull to decode audio/video frames.
     * The unmanaged decoding is faster because it saves copying decoded data from unmanaged to managed memory.
     * It is preferred when the decoded unmanaged data can be passed directly to unmanaged code
     * for further processing or rendering.
     */
    bool _unmanaged = true;

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    protected virtual void Dispose(bool disposing)
    {
        if (_disposed)
            return;

        if (disposing)
        {
            // Free any other managed objects here. 
        }

        // Free any unmanaged objects here. 
        Close();

        _disposed = true;
    }
    #endregion

    #region vars
    bool _cancellationPending;

	Transcoder _transcoder;
	VideoStreamInfo _videoStreamInfo;
	AudioStreamInfo _audioStreamInfo;
	int _videoStreamIndex = -1;
	int _audioStreamIndex = -1;

	WinMMAudioRender  _audioRender;
	OpenGLVideoRender _videoRender;
    MainForm _mainForm;

	object _csAVQueue = new object();
	object _csClock = new object();

    bool _decoderEOS;
    LinkedList<MediaSample> _audioQueue = new LinkedList<MediaSample>();
    LinkedList<MediaSample> _videoQueue = new LinkedList<MediaSample>();

	double _audioStartPTS;
	double _videoStartPTS;

	double _audioElapsedTime;
	long _baseTimeTicks;

    System.Threading.Thread _decoderThread;
    #endregion


    #region WinMMAudioRenderCallback
    public bool NextAudioBuffer(byte[] buffer, ref int length)
    {
	    if (_cancellationPending)
	    {
		    length = 0;
		    return false;
	    }

	    lock(_csAVQueue)
        {
	        int bytesWritten = 0;

	        while ((_audioQueue.Count > 0) && (bytesWritten < buffer.Length))
	        {
		        MediaSample mediaSample = _audioQueue.First.Value;

                if (_unmanaged)
                {
                    UnmanagedMediaBuffer mediaBuffer = mediaSample.UnmanagedBuffer;

                    int chunk = Math.Min(mediaBuffer.DataSize, buffer.Length - bytesWritten);
                    Marshal.Copy(mediaBuffer.DataPtr, buffer, bytesWritten, chunk);

                    bytesWritten += chunk;
                    mediaBuffer.Remove(chunk);

                    if (mediaBuffer.DataSize == 0)
                    {
                        mediaBuffer.Release();
                        _audioQueue.RemoveFirst();
                    }
                }
                else
                {
                    MediaBuffer mediaBuffer = mediaSample.Buffer;

                    int chunk = Math.Min(mediaBuffer.DataSize, buffer.Length - bytesWritten);
                    Array.Copy(mediaBuffer.Start, mediaBuffer.DataOffset, buffer, bytesWritten, chunk);

                    bytesWritten += chunk;

                    {
                        int newDataOffset = mediaBuffer.DataOffset + chunk;
                        int newDataSize = mediaBuffer.DataSize - chunk;
                        if (0 == newDataSize)
                            newDataOffset = 0;

                        mediaBuffer.SetData(newDataOffset, newDataSize);
                    }

                    if (mediaBuffer.DataSize == 0)
                    {
                        _audioQueue.RemoveFirst();
                    }
                }
		        
	        }

            length = bytesWritten;
        }
    
	    return true;
    }

    public void PlaybackProgress(long totalDuration, long lastBufferDuration, long bytesPerSec)
    {
        lock (_csClock)
        {
            _audioElapsedTime = (double)totalDuration / (double)bytesPerSec;
            _baseTimeTicks = DateTime.Now.Ticks;
        }
    }
    #endregion

    public void Close()
    {
        if (null != _transcoder)
        {
            _transcoder.Dispose();
            _transcoder = null;
        }

        _decoderThread = null;

        _videoStreamInfo = null;
        _audioStreamInfo = null;

        _videoStreamIndex = -1;
        _audioStreamIndex = -1;
        _decoderEOS = false;
        _cancellationPending = false;
    }

    public void EventLoop()
    {
        StartDecoderThread();
        FillAVQueue();
        InitClock();
        StarRenders();

        {
            _cancellationPending = false;

            while (!_cancellationPending && !IsInputProcessed())
            {
                Application.DoEvents();

                RefreshRenders();

                System.Threading.Thread.Sleep(1);
            }
        }

        JoinDecoderThread();
        StopRenders();

        ClearAVQueue();
    }

    double GetClock()
    {
        lock (_csClock)
        {
            TimeSpan ts = new TimeSpan(DateTime.Now.Ticks - _baseTimeTicks);
            double baseTimeElapsed = ts.TotalSeconds;

            if (_audioRender != null)
            {
                return _audioStartPTS + _audioElapsedTime + baseTimeElapsed;
            }
            else
            {
                return _videoStartPTS + baseTimeElapsed;
            }
        }
    }

    void InitClock()
    {
        lock (_csAVQueue)
        {
            _audioStartPTS = -1.0;
            _videoStartPTS = -1.0;
            _baseTimeTicks = DateTime.Now.Ticks;
            _audioElapsedTime = 0;

            if (_audioQueue.Count > 0)
                _audioStartPTS = _audioQueue.First.Value.StartTime;

            if (_videoQueue.Count > 0)
                _videoStartPTS = _videoQueue.First.Value.StartTime;
        }
    }

    bool IsAVQueueFull()
    {
        lock (_csAVQueue)
        {
            int maxVideoSamples;
            double maxVideoDuration;
            double maxAudioDuration;

            if (isHdVideo())
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

            if (_videoQueue.Count > maxVideoSamples)
                return true;

            if (_videoQueue.Count > 1)
            {
                double duration = _videoQueue.Last.Value.StartTime - _videoQueue.First.Value.StartTime;
                if (duration > maxVideoDuration)
                    return true;
            }

            if (_audioQueue.Count > 1)
            {
                double duration = _audioQueue.Last.Value.StartTime - _audioQueue.First.Value.StartTime;
                if (duration > maxAudioDuration)
                    return true;
            }
        }

	    return false;
    }

    bool IsInputProcessed()
    {
        lock (_csAVQueue)
        {
            return (_audioQueue.Count == 0) && (_videoQueue.Count == 0) && _decoderEOS;
        }
    }

    void ClearAVQueue()
    {
        lock (_csAVQueue)
        {
            _audioQueue.Clear();
            _videoQueue.Clear();
        }
    }

    void FillAVQueue()
    {
	    while (!IsAVQueueFull() && !_decoderEOS && !_cancellationPending)
	    {
            System.Threading.Thread.Sleep(5);
		    continue;
	    }
    }

    // returns true if video resolution is > 1280x720
    bool isHdVideo()
    {
        if (_videoStreamInfo == null)
		    return false;

	    return (_videoStreamInfo.FrameWidth > 1280) || (_videoStreamInfo.FrameHeight > 720);
    }

    void StartDecoderThread()
    {
	    _decoderEOS = false;
        _decoderThread = new System.Threading.Thread(DecoderThread);
        _decoderThread.Start();
    }

    void JoinDecoderThread()
    {
	    if (null != _decoderThread)
	    {
            _decoderThread.Join();
            _decoderThread = null;
	    }
    }

    void DecoderThread()
    {
	    while (!_decoderEOS && !_cancellationPending)
	    {
		    if (IsAVQueueFull())
		    {
                System.Threading.Thread.Sleep(1);
			    continue;
		    }

		    int index = -1;

		    MediaSample mediaSample = new MediaSample();
            bool res;

            if (_unmanaged)
            {
                res = _transcoder.PullUnmanaged(out index, mediaSample);
            }
            else 
            {
                res = _transcoder.Pull(out index, mediaSample);
            }

		    if (res)
		    {
                lock (_csAVQueue)
                {
                    if (index == _videoStreamIndex)
                    {
                        _videoQueue.AddLast(mediaSample);
                    }
                    else if (index == _audioStreamIndex)
                    {
                        _audioQueue.AddLast(mediaSample);
                    }
                }
		    }
		    else
		    {
			    _decoderEOS = true;
		    }
	    }
    }

    void RefreshRenders()
    {
        lock (_csAVQueue)
        {
            if (null != _videoRender)
            {
                if (_videoQueue.Count > 0)
                {
                    MediaSample sample = _videoQueue.First.Value;

                    if (GetClock() >= sample.StartTime)
                    {
                        if (_unmanaged)
                        {
                            _videoRender.SetFrame(sample.UnmanagedBuffer.DataPtr, _videoStreamInfo.FrameWidth, _videoStreamInfo.FrameHeight);
                            _videoRender.Draw();
                            sample.UnmanagedBuffer.Release();
                            _videoQueue.RemoveFirst();
                        }
                        else
                        {
                            MediaBuffer buffer = sample.Buffer;
                            _videoRender.SetFrame(buffer.Start, _videoStreamInfo.FrameWidth, _videoStreamInfo.FrameHeight);
                            _videoRender.Draw();
                            _videoQueue.RemoveFirst();
                        }
                    }
                }
            }
        }
    }



    bool ConfigureStreams(string filePath)
    {
        using (MediaInfo mediaInfo = new MediaInfo())
        {
            mediaInfo.Inputs[0].File = filePath;

            if (!(mediaInfo.Open()))
                return false;

            // configure streams
            foreach (var socket in mediaInfo.Outputs)
            {
                foreach (var pin in socket.Pins)
                {
                    StreamInfo si = pin.StreamInfo;
                    MediaType mediaType = si.MediaType;

                    if ((MediaType.Video == mediaType) && (null == _videoStreamInfo))
                    {
                        VideoStreamInfo vsi = (VideoStreamInfo)si;
                        _videoStreamInfo = new VideoStreamInfo();

                        _videoStreamInfo.FrameWidth = vsi.FrameWidth;
                        _videoStreamInfo.FrameHeight = vsi.FrameHeight;
                        _videoStreamInfo.DisplayRatioWidth = vsi.DisplayRatioWidth;
                        _videoStreamInfo.DisplayRatioHeight = vsi.DisplayRatioHeight;
                        _videoStreamInfo.FrameRate = vsi.FrameRate;
                    }

                    if ((MediaType.Audio == mediaType) && (null == _audioStreamInfo))
                    {
                        AudioStreamInfo asi = (AudioStreamInfo)si;
                        _audioStreamInfo = new AudioStreamInfo();

                        _audioStreamInfo.BitsPerSample = asi.BitsPerSample;
                        _audioStreamInfo.Channels = asi.Channels;
                        _audioStreamInfo.SampleRate = asi.SampleRate;
                    }
                }
            }
        }

	    return true;
    }

    static int calculatePadding(int val, int granularity)
    {
        return (granularity - val % granularity) % granularity;
    }

    public bool Open(string filePath)
    {
	    Close();

	    if (!ConfigureStreams(filePath))
	    {
		    Close();
		    return false;
	    }

	    _transcoder = new Transcoder();
	    // In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
	    _transcoder.AllowDemoMode = true;

	    // Configure input
	    {
            using (MediaInfo mediaInfo = new MediaInfo())
            {
                mediaInfo.Inputs[0].File = filePath;

                if (!(mediaInfo.Open()))
                    return false;

                MediaSocket socket = MediaSocket.FromMediaInfo(mediaInfo);
                _transcoder.Inputs.Add(socket);
            }
	    }

	    // Configure video output
	    if (_videoStreamInfo != null)
	    {
		    _videoStreamInfo.ColorFormat = ColorFormat.BGR24;
		    _videoStreamInfo.FrameBottomUp = true;
		    _videoStreamInfo.StreamType = StreamType.UncompressedVideo;
		    _videoStreamInfo.ScanType = ScanType.Progressive;

		    MediaPin pin = new MediaPin();

            int displayWidth = Screen.PrimaryScreen.Bounds.Width;
            int displayHeight = Screen.PrimaryScreen.Bounds.Height;

            if ((_videoStreamInfo.FrameWidth > displayWidth) ||
                ((_videoStreamInfo.FrameHeight > displayHeight)))
            {
                // resize the video
                double displayAspect = (double)displayWidth / (double)displayHeight;
                double videoAspect = (double)_videoStreamInfo.DisplayRatioWidth / (double)_videoStreamInfo.DisplayRatioHeight;

                int width = 0;
                int height = 0;

                if (videoAspect < displayAspect)
                {
                    width = displayWidth;
                    height = (int)(displayWidth / videoAspect);
                }
                else
                {
                    width = (int)(displayHeight * videoAspect);
                    height = displayHeight;
                }

                width += calculatePadding(width, 2);
                height += calculatePadding(height, 2);

                _videoStreamInfo.FrameWidth = width;
                _videoStreamInfo.FrameHeight = height;

                {
                    pin.Params.Add(Param.Video.Resize.InterpolationMethod, PrimoSoftware.AVBlocks.InterpolationMethod.Linear);
                }
            }

		    pin.StreamInfo = _videoStreamInfo;

		    MediaSocket socket = new MediaSocket();
		    socket.StreamType = StreamType.UncompressedVideo;
		    socket.Pins.Add(pin);

		    _videoStreamIndex = _transcoder.Outputs.Count;
		    _transcoder.Outputs.Add(socket);
	    }

	    // Configure audio output
	    if (_audioStreamInfo != null)
	    {
		    _audioStreamInfo.BitsPerSample = 16;

		    // WinMM audio render supports only mono and stereo
		    if (_audioStreamInfo.Channels > 2)
			    _audioStreamInfo.Channels = 2;

		    _audioStreamInfo.StreamType = StreamType.LPCM;

		    MediaPin pin = new MediaPin();
		    pin.StreamInfo = _audioStreamInfo;

		    MediaSocket socket = new MediaSocket();
		    socket.StreamType = StreamType.LPCM;
		    socket.Pins.Add(pin);

		    _audioStreamIndex = _transcoder.Outputs.Count;
            _transcoder.Outputs.Add(socket);
	    }

	    if (!_transcoder.Open())
	    {
		    Close();
		    return false;
	    }

	    return true;
    }


    void StarRenders()
    {
        _mainForm = new MainForm();
        _mainForm.FormClosed += MainForm_FormClosed;
        _mainForm.Show();

        _videoRender = _mainForm.OpenGLVideoRender;

	    if (_videoStreamInfo != null)
	    {
		    _videoRender.SetDisplayAspect(_videoStreamInfo.DisplayRatioWidth, _videoStreamInfo.DisplayRatioHeight);
		    _videoRender.Start();
	    }

	    if (_audioStreamInfo != null)
	    {
		    _audioRender = new WinMMAudioRender();
		    _audioRender.Start(this, _audioStreamInfo.SampleRate, _audioStreamInfo.Channels, _audioStreamInfo.BitsPerSample);
	    }
    }

    void MainForm_FormClosed(object sender, FormClosedEventArgs e)
    {
        _cancellationPending = true;
    }

    void StopRenders()
    {
        _videoRender = null;

	    if (null != _audioRender)
	    {
		    _audioRender.Stop();
            _audioRender.Dispose();
		    _audioRender = null;
	    }

        _mainForm.Dispose();
    }
}

}
