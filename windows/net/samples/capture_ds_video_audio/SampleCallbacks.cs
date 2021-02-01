using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Diagnostics;
using DirectShowLib;
using PrimoSoftware.AVBlocks;

namespace CaptureDS
{
    class SampleGrabberCB: ISampleGrabberCB
    {
        protected long lastMediaTime = -1;

        protected string name;
        protected MediaState mediaState;
        protected bool bProcess = true;
        
        // how many times the callback has been called
        protected long sampleIndex;

        // how many samples are processed (based on the media time)
        protected long sampleProcessed;

        // how many samples are dropped (based on the media time)
        protected long sampleDropped;

        protected int streamNumber;
        protected IntPtr mainWindow;


        /*
         * Specifies whether to use Transcoder.PushUnmanaged or Transcoder.Push for encoding.
         * PushUnmanaged uses UnmanagedMediaBuffer to pass the captured unamanaged data directly
         * to the Transcoder and saves copying from unmanaged to managed memory.
         */ 
        private bool unmanaged = false;

        MediaSample sample = new MediaSample();

        bool ProcessSample(IntPtr pBuffer, int dataLen, double sampleTime)
        {
            bool pushResult = true;

            if ((mediaState != null) && ((mediaState.transcoder != null)))
            {
                if (sampleTime < 0)
                    sampleTime = 0;

                if (unmanaged)
                {
                    if (sample.UnmanagedBuffer == null)
                        sample.UnmanagedBuffer = new UnmanagedMediaBuffer();

                    sample.UnmanagedBuffer.Attach(pBuffer, dataLen, true);
                }
                else
                {
                    if (sample.Buffer == null || sample.Buffer.Start.Length != dataLen)
                    {
                        sample.Buffer = new MediaBuffer(new byte[dataLen]);
                    }

                    Marshal.Copy(pBuffer, sample.Buffer.Start, 0, dataLen);
                    sample.Buffer.SetData(0, dataLen);
                }

                sample.StartTime = sampleTime;

                //System.Diagnostics.Trace.WriteLine(
                //    string.Format("transcoder.Push(stream: {0}, sampleTime: {1}, sampleData: {2})", 
                //    StreamNumber, sample.StartTime, sample.Buffer.DataSize));

                /*
                 * The built-in transcoder is not thread-safe
                 * and usually the calls to Transcoder.Push/PushUnmanaged must be synchronized,
                 * However the CompositeTranscoder (implemented in this app)
                 * allows that input samples from different streams can be pushed simultaneously.
                 *  
                 */
#if (TRACE && DEBUG)
                lock (mediaState.transcoder)
#endif
                {
                    if (unmanaged)
                    {
                        pushResult = mediaState.transcoder.PushUnmanaged(StreamNumber, sample);
                    }
                    else
                    {
                        pushResult = mediaState.transcoder.Push(StreamNumber, sample);
                    }
                }
            }

            if (pushResult)
                return true;


            WinAPI.PostMessage(MainWindow, Util.WM_STOP_CAPTURE, new IntPtr(streamNumber), IntPtr.Zero);
            bProcess = false;
            return false;
        }


        public int BufferCB(double sampleTime, IntPtr pBuffer, int bufferLen)
        {
            if (!bProcess)
                return WinAPI.E_FAIL;

            sampleIndex += 1;

            bool processed = ProcessSample(pBuffer, bufferLen, sampleTime);

            return processed ? WinAPI.S_OK : WinAPI.E_FAIL;

        }

        public int SampleCB(double sampleTime, IMediaSample pSample)
        {
            try
            {
                if (!bProcess)
                    return WinAPI.E_FAIL;

                // internal stats
                ++sampleIndex;
                long tStart, tEnd;

                pSample.GetMediaTime(out tStart, out tEnd);
                Debug.Assert(tStart < tEnd);
                Debug.Assert(tStart > lastMediaTime);
                sampleProcessed += tEnd - tStart;
                sampleDropped += tStart - lastMediaTime - 1;
                lastMediaTime = tEnd - 1;

                int dataLen = pSample.GetActualDataLength();
                IntPtr bufPtr;
                int hr = pSample.GetPointer(out bufPtr);
                Debug.Assert(0 == hr);

                bool processed = ProcessSample(bufPtr, dataLen, sampleTime);

                return processed ? WinAPI.S_OK : WinAPI.E_FAIL;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.ToString());
            }
            finally
            {
                Marshal.ReleaseComObject(pSample);
            }

            return WinAPI.E_FAIL;
        } // end of SampleCB

        public IntPtr MainWindow
        {
            get { return mainWindow; }
            set { mainWindow = value; }
        }

        public long SampleIndex
        {
            get { return sampleIndex; }
        }

        public int StreamNumber
        {
            get { return streamNumber; }
            set { streamNumber = value; }
        }

        public long ProcessedSamples
        {
            get { return sampleProcessed; }
        }

        public long DroppedSamples
        {
            get { return sampleDropped; }
        }

        public SampleGrabberCB(string name)
        {
            if (!string.IsNullOrEmpty(name))
            {
                this.name = name;
            }
            else
            {
                this.name = "SampleGrabberCB";
            }
        }

        ~SampleGrabberCB()
        {
            Reset();
        }

        public MediaState MediaState
        {
            get { return mediaState; }
            set { mediaState = value; }
        }

        public void Reset()
        {
            sampleIndex = 0;
            sampleProcessed = 0;
            sampleDropped = 0;

            mediaState = null;
            bProcess = true;

            lastMediaTime = -1;

            sample.Buffer = null;

            if (sample.UnmanagedBuffer != null)
                sample.UnmanagedBuffer.Release();

            sample.UnmanagedBuffer = null;
        }

    } // end of SampleGrabberCB

    // Sample grabber callback method
    enum CBMethod
    {
        Sample = 0, // the original sample from the upstream filter
        Buffer = 1  // a copy of the sample of the upstream filter
    };
}
