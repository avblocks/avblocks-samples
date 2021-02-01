/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Diagnostics;
using DirectShowLib;
using PrimoSoftware.AVBlocks;

namespace InputDS
{
    class SampleGrabberCB: ISampleGrabberCB
    {
        Transcoder transcoder;
        int transcoderInputIndex;
        DirectShowLib.IMediaControl mediaControl;
        ErrorInfo transcoderError;
        long samplesProcessed;
        byte[] sampleBuffer;

        bool ProcessSample(IntPtr pBuffer, int dataLen, double sampleTime)
        {
            // not initialized
            if (null == transcoder)
                return true;

	        if (sampleTime < 0)
		        sampleTime = 0;

            if((null == sampleBuffer) || (sampleBuffer.Length != dataLen))
            {
                sampleBuffer = new byte[dataLen];
            }

            Marshal.Copy(pBuffer, sampleBuffer, 0, dataLen);

            MediaSample inputSample = new MediaSample();
            inputSample.Buffer = new MediaBuffer(sampleBuffer);
            inputSample.StartTime = sampleTime;

            System.Diagnostics.Debug.WriteLine(string.Format("transcoder.Push(stream: {0}, sampleTime: {1}, sampleData: {2})", transcoderInputIndex, inputSample.StartTime, inputSample.Buffer.DataSize));

            // transcoder.Push() is not threads safe.
            // lock (transcoder){ } ensure that only one thread is calling transcoder.Push()
            lock (transcoder)
            {
                if (!transcoder.Push(transcoderInputIndex, inputSample))
                {
                    transcoderError = transcoder.Error.Clone() as ErrorInfo;

                    System.Threading.ThreadPool.QueueUserWorkItem(delegate
                    {
                        // call mediaControl.Stop() from a new thread otherwise it will deadlock
                        int hr = mediaControl.Stop();
                    }, null);

                    return false;
                }
            }

	        return true;
        }
       
        int ISampleGrabberCB.BufferCB(double sampleTime, IntPtr pBuffer, int bufferLen)
        {
            if (transcoderError != null)
                return WinAPI.E_FAIL;

            samplesProcessed += 1;

            bool processed = ProcessSample(pBuffer, bufferLen, sampleTime);

            return processed ? WinAPI.S_OK : WinAPI.E_FAIL;
        }

        int ISampleGrabberCB.SampleCB(double sampleTime, IMediaSample pSample)
        {
            try
            {
                if (transcoderError != null)
                    return WinAPI.E_FAIL;

                samplesProcessed += 1;

                int dataLen = pSample.GetActualDataLength();
                IntPtr bufPtr;
                int hr = pSample.GetPointer(out bufPtr);

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

        public SampleGrabberCB()
        {
            Reset();
        }

        void Reset()
        {
            transcoder = null;
            transcoderInputIndex = 0;
            mediaControl = null;
            transcoderError = null;
            samplesProcessed = 0;
            sampleBuffer = null;
        }

        public void Init(Transcoder transcoder, int transcoderInputIndex, IMediaControl mediaControl)
        {
	        Reset();

            this.transcoder = transcoder;
            this.transcoderInputIndex = transcoderInputIndex;
            this.mediaControl = mediaControl;
        }

        public ErrorInfo TranscoderError
        {
            get { return transcoderError; }
        }
    } 

    // Sample grabber callback method
    enum CBMethod
    {
        Sample = 0, // the original sample from the upstream filter
        Buffer = 1  // a copy of the sample of the upstream filter
    };
}
