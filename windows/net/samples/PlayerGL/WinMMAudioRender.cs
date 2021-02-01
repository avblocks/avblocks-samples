/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;


namespace PlayerGLSample
{
    interface WinMMAudioRenderCallback
    {
	    bool NextAudioBuffer(byte[] buffer, ref int length);
	    void PlaybackProgress(long totalDuration, long lastBufferDuration, long bytesPerSec);
    };

    class WinMMAudioRender: IDisposable
    {
        #region Native
        internal class Native
        {
            [StructLayout(LayoutKind.Sequential)]
            public class WAVEHDR
            {
                public IntPtr lpData;
                public int dwBufferLength;
                public int dwBytesRecorded;
                public IntPtr dwUser;
                public int dwFlags;
                public int dwLoops;
                public IntPtr lpNext;
                public IntPtr reserved;
            }

            [StructLayout(LayoutKind.Sequential)] 
            public class WAVEFORMATEX
            {
                public short wFormatTag;
                public short nChannels;
                public int nSamplesPerSec;
                public int nAvgBytesPerSec;
                public short nBlockAlign;
                public short wBitsPerSample;
                public short cbSize;
            }

            public const int WAVE_FORMAT_PCM = 1;
            public const int WAVE_FORMAT_IEEE_FLOAT = 3;

            public const int MMSYSERR_NOERROR = 0;
            public const int MM_WOM_OPEN = 0x3BB;
            public const int MM_WOM_CLOSE = 0x3BC;
            public const int MM_WOM_DONE = 0x3BD;

            public const int CALLBACK_FUNCTION = 0x00030000;

            // Can be used instead of a device id to open a device
            public const uint WAVE_MAPPER = unchecked((uint)(-1));

            public delegate void WaveOutProcDelegate(IntPtr hdrvr, int uMsg, IntPtr dwInstance, IntPtr dwParam1, IntPtr dwParam2);

            // native calls
            [DllImport("winmm.dll")]
            public static extern int waveOutPrepareHeader(IntPtr hWaveOut, IntPtr lpWaveOutHdr, int uSize);

            [DllImport("winmm.dll")]
            public static extern int waveOutUnprepareHeader(IntPtr hWaveOut, IntPtr lpWaveOutHdr, int uSize);

            [DllImport("winmm.dll")]
            public static extern int waveOutWrite(IntPtr hWaveOut, IntPtr lpWaveOutHdr, int uSize);

            [DllImport("winmm.dll")]
            public static extern int waveOutOpen(out IntPtr hWaveOut, uint uDeviceID, WAVEFORMATEX lpFormat, WaveOutProcDelegate dwCallback, IntPtr dwInstance, int dwFlags);

            [DllImport("winmm.dll")]
            public static extern int waveOutReset(IntPtr hWaveOut);

            [DllImport("winmm.dll")]
            public static extern int waveOutClose(IntPtr hWaveOut);

            [DllImport("winmm.dll")]
            public static extern int waveOutRestart(IntPtr hWaveOut);

            [DllImport("winmm.dll")]
            public static extern int waveOutPause(IntPtr hWaveOut);
        }
        #endregion

        #region Disposable impl
        bool _disposed = false;

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
            Stop();

            _disposed = true;
        }

        ~WinMMAudioRender()
        {
            Dispose(false);
        }
        #endregion

        #region WaveOutBuffer
        internal class WaveOutBuffer: IDisposable
        {
            internal Native.WAVEHDR _hdr;
            internal byte[] _data;
            internal GCHandle _gchHdr;
            internal GCHandle _gchData;
            internal GCHandle _gchThis;

            bool _disposed;

            public WaveOutBuffer(int size)
            {
                _hdr = new Native.WAVEHDR();
                _gchThis = GCHandle.Alloc(this);
                _gchHdr = GCHandle.Alloc(_hdr, GCHandleType.Pinned);
                _hdr.dwUser = (IntPtr)_gchThis;
                _data = new byte[size];
                _gchData = GCHandle.Alloc(_data, GCHandleType.Pinned);
                _hdr.lpData = _gchData.AddrOfPinnedObject();
                _hdr.dwBufferLength = size;
            }

            public void Dispose()
            {
                Dispose(true);
                GC.SuppressFinalize(this);
            }

            ~WaveOutBuffer()
            {
                Dispose(false);
            }

            protected virtual void Dispose(bool disposing)
            {
                if (_disposed)
                    return;

                if (disposing)
                {
                    _gchHdr.Free();
                    _gchData.Free();
                    _gchThis.Free();
                }

                // Free any unmanaged objects here. 
                _disposed = true;
            }
        }
        #endregion

        WinMMAudioRenderCallback _callback;
	    bool _started = false;
	    long _totalDuration = 0;
        IntPtr _hWaveout = IntPtr.Zero;
        Native.WAVEFORMATEX _fmt;
        List<WaveOutBuffer> _buffers = new List<WaveOutBuffer>();
        Native.WaveOutProcDelegate _waveOutProc = new Native.WaveOutProcDelegate(WaveOutProc);
        GCHandle _gchThis;

	    void CreateBuffers(int buffersCount, int bufferLength)
        {
            for (int i = 0; i < buffersCount; i++)
            {
                WaveOutBuffer buffer = new WaveOutBuffer(bufferLength);
                buffer._hdr.dwBufferLength = bufferLength;

                int result = Native.waveOutPrepareHeader(_hWaveout, buffer._gchHdr.AddrOfPinnedObject(), Marshal.SizeOf(buffer._hdr));
                System.Diagnostics.Debug.Assert(result == Native.MMSYSERR_NOERROR);

                _buffers.Add(buffer);
            }
        }

	    void ReleaseBuffers()
        {
	        for (int i = 0; i < _buffers.Count; i++)
	        {
                WaveOutBuffer buffer = _buffers[i];

                int result = Native.waveOutUnprepareHeader(_hWaveout, buffer._gchHdr.AddrOfPinnedObject(), Marshal.SizeOf(buffer._hdr));
                System.Diagnostics.Debug.Assert(result == Native.MMSYSERR_NOERROR);

                buffer.Dispose();
	        }

	        _buffers.Clear();
        }


        public bool Start(WinMMAudioRenderCallback callback, int sampleRate, int channels, int bitsPerSample)
        {
            if (null == callback)
                return false;

            if ((0 == sampleRate) || (0 == channels) || (0 == bitsPerSample))
                return false;

            // only mono and stereo are supported
            if (channels > 2)
                return false;

            Stop();

            _callback = callback;

            _fmt = new Native.WAVEFORMATEX();

            _fmt.wFormatTag = Native.WAVE_FORMAT_PCM;
            _fmt.nChannels = (short)channels;
            _fmt.nSamplesPerSec = sampleRate;
            _fmt.nBlockAlign = (short)((bitsPerSample >> 3) * channels);
            _fmt.nAvgBytesPerSec = _fmt.nBlockAlign * sampleRate;
            _fmt.wBitsPerSample = (short)bitsPerSample;
            _fmt.cbSize = (short)Marshal.SizeOf(_fmt);

            _gchThis = GCHandle.Alloc(this);

            int result = Native.waveOutOpen(out _hWaveout, Native.WAVE_MAPPER, _fmt, _waveOutProc, (IntPtr)_gchThis, Native.CALLBACK_FUNCTION);
	        if (result != Native.MMSYSERR_NOERROR)
	        {
                _gchThis.Free();
		        _hWaveout = IntPtr.Zero;
		        return false;
	        }

	        _started = true;
	        _totalDuration = 0;

	        CreateBuffers(5, _fmt.nBlockAlign * (_fmt.nSamplesPerSec / 50)); // 5 buffers, 20ms each buffer

            result = Native.waveOutPause(_hWaveout);
            System.Diagnostics.Debug.Assert(result == Native.MMSYSERR_NOERROR);

	        // write the empty buffers
	        for (int i = 0; i < _buffers.Count; i++)
	        {
		        WaveOutBuffer buffer = _buffers[i];
                buffer._hdr.dwBufferLength = 0;

                result = Native.waveOutWrite(_hWaveout, buffer._gchHdr.AddrOfPinnedObject(), Marshal.SizeOf(buffer._hdr));
                System.Diagnostics.Debug.Assert(Native.MMSYSERR_NOERROR == result);
	        }

            result = Native.waveOutRestart(_hWaveout);
            System.Diagnostics.Debug.Assert(result == Native.MMSYSERR_NOERROR);

	        return true;
        }

        public void Stop()
        {
            if (IntPtr.Zero == _hWaveout)
                return;

            _started = false;

            int result = Native.waveOutReset(_hWaveout);
            System.Diagnostics.Debug.Assert(result == Native.MMSYSERR_NOERROR);

            ReleaseBuffers();

            result = Native.waveOutClose(_hWaveout);
            System.Diagnostics.Debug.Assert(result == Native.MMSYSERR_NOERROR);

            _gchThis.Free();
            _hWaveout = IntPtr.Zero;
            _callback = null;
            _fmt = null;
        }

        public bool Started
        {
            get { return _started; }
        }

        internal static void WaveOutProc(IntPtr hdrvr, int uMsg, IntPtr dwInstance, IntPtr dwParam1, IntPtr dwParam2)
        {
            if (uMsg == Native.MM_WOM_DONE)
            {
                try
                {
                    System.Diagnostics.Debug.Assert(dwInstance != IntPtr.Zero);
                    WinMMAudioRender instance = (WinMMAudioRender)((GCHandle)dwInstance).Target;

                    System.Diagnostics.Debug.Assert(dwParam1 != IntPtr.Zero);
                    Native.WAVEHDR hdr = (Native.WAVEHDR)Marshal.PtrToStructure(dwParam1, typeof(Native.WAVEHDR));

                    System.Diagnostics.Debug.Assert(hdr.dwUser != IntPtr.Zero);
                    WaveOutBuffer waveOutBuffer = (WaveOutBuffer)((GCHandle)hdr.dwUser).Target;

		            if (hdr.dwBufferLength > 0)
		            {
			            instance._totalDuration += hdr.dwBufferLength;
			            instance._callback.PlaybackProgress(instance._totalDuration, hdr.dwBufferLength, instance._fmt.nAvgBytesPerSec);
		            }

		            // if we are still playing get a new audio buffer
		            if (instance._started)
		            {
			            int length = 0;

                        if (instance._callback.NextAudioBuffer(waveOutBuffer._data, ref length))
			            {
                            System.Diagnostics.Debug.Assert(length <= waveOutBuffer._data.Length);

                            waveOutBuffer._hdr.dwBufferLength = length;

                            int result = Native.waveOutWrite(instance._hWaveout,  waveOutBuffer._gchHdr.AddrOfPinnedObject(), Marshal.SizeOf(waveOutBuffer._hdr));
                            System.Diagnostics.Debug.Assert(result == Native.MMSYSERR_NOERROR);
			            }
			            else
			            {
				            // end of stream read
				            instance._started = false;
			            }
		            }
                }
                catch(Exception ex)
                {
                    System.Diagnostics.Debug.WriteLine(ex.ToString());
                }
            }
        }
    }
}
