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
using PrimoSoftware.AVBlocks;
using DirectShowLib;

namespace InputDS
{
    class Program
    {
        static int Main(string[] args)
        {
            var opt = new Options();

            if (!opt.Prepare(args))
                return opt.Error ? (int)ExitCodes.OptionsError : (int)ExitCodes.Success;

            try
            {
                PrimoSoftware.AVBlocks.Library.Initialize();

                // Set license information. To run AVBlocks in demo mode, comment the next line out
                // Library.SetLicense("<license-string>");

                EncodeDirectShowInput(opt.InputFile, opt.OutputFile, opt.OutputPreset);
            }
            catch (Exception ex)
            {
                System.Console.WriteLine(ex.ToString());

                return (int)ExitCodes.EncodeError;
            }
            finally
            {
                PrimoSoftware.AVBlocks.Library.Shutdown();
            }

            return (int)ExitCodes.Success;
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            EncodeError = 2,
        }

        static void EncodeDirectShowInput(string inputFile, string outputFile, string preset)
        {
            if (File.Exists(outputFile))
                File.Delete(outputFile);

	        DSGraph dsGraph = new DSGraph();
            Transcoder transcoder = new Transcoder();

            // In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
            transcoder.AllowDemoMode = true;

            try
            {
                Console.WriteLine("Initializing DirectShow graph.");

                /*
                    If the source is a DirectShow filter instead of a file then:
                   1) Create an instance of the source filter
                   2) Configure source filter
                   3) Call dsGraph.Init(sourceFilter);

                   For example dsGraph.Init(inputFile) can be replaced with the following code:
                   
                     1) Create an instance of the source filter 
                    
                        // FileSourceAsync filter
                        IBaseFilter sourceFilter = Util.CreateFilter(new Guid("e436ebb5-524f-11ce-9f53-0020af0ba770"));
                        
                        // or WM ASF Reader filter
                        IBaseFilter sourceFilter = Util.CreateFilter(new Guid("187463A0-5BB7-11D3-ACBE-0080C75E246E"));
                  
                     2) Configure source filter
                        IFileSourceFilter fileSourceFilter = sourceFilter as IFileSourceFilter;
                        fileSourceFilter.Load(inputFile, null);

                     3)
                        dsGraph.Init(sourceFilter);
                */

                dsGraph.Init(inputFile);

                if (dsGraph.videoGrabber != null)
                    ConfigureVideoInput(dsGraph, transcoder);

                if (dsGraph.audioGrabber != null)
                    ConfigureAudioInput(dsGraph, transcoder);

                if ((dsGraph.videoGrabber == null) && (dsGraph.audioGrabber == null))
                {
                    Console.WriteLine("No audio or video can be read from the DirectShow graph.");
                    return;
                }

	            // Configure output
	            {
                    MediaSocket outSocket = MediaSocket.FromPreset(preset);
                    outSocket.File = outputFile;
                    transcoder.Outputs.Add(outSocket);
	            }

                bool res = transcoder.Open();

                PrintError("Open Transcoder", transcoder.Error);
                if (!res)
                    return;

                //DBG
                //var rot = new DsROTEntry(dsGraph.graph);

                Console.WriteLine("Running DirectShow graph.");
                int hr = dsGraph.mediaControl.Run();
                DsError.ThrowExceptionForHR(hr);
                
                while(true)
                {
                    FilterState fs;
                    dsGraph.mediaControl.GetState(-1, out fs);

                    if (fs != FilterState.Running)
                        break;

                    EventCode ev;
                    dsGraph.mediaEvent.WaitForCompletion(1000, out ev);

                    if(EventCode.Complete == ev)
                        break;
                }

                Console.WriteLine("DirectShow graph is stopped.");

                if ((dsGraph.videoGrabberCB != null) && (dsGraph.videoGrabberCB.TranscoderError != null))
                    PrintError("Transcoder Error", transcoder.Error);

                if ((dsGraph.audioGrabberCB != null) && (dsGraph.audioGrabberCB.TranscoderError != null))
                    PrintError("Transcoder Error", transcoder.Error);

                Console.WriteLine("Closing transcoder.");

                if (!transcoder.Flush())
                    PrintError("Flush Transcoder", transcoder.Error);

                transcoder.Close();
            }
            finally
            {
                dsGraph.Reset();
                transcoder.Dispose();
            }
        }

        static void ConfigureVideoInput(DSGraph graph, Transcoder transcoder)
        {
            AMMediaType mt = new AMMediaType();
            int hr;

            try
            {
                hr = graph.videoGrabber.GetConnectedMediaType(mt);
                DsError.ThrowExceptionForHR(hr);

                if((mt.majorType != DirectShowLib.MediaType.Video) || 
                    (mt.formatType != DirectShowLib.FormatType.VideoInfo))
                {
                    throw new COMException("Unexpected format type");
                }

                VideoInfoHeader vih = (VideoInfoHeader)Marshal.PtrToStructure(mt.formatPtr, typeof(VideoInfoHeader));

                VideoStreamInfo videoInfo = new VideoStreamInfo();

                if (vih.AvgTimePerFrame > 0)
                    videoInfo.FrameRate = (double)10000000 / vih.AvgTimePerFrame;

                videoInfo.Bitrate = 0; 
                videoInfo.FrameHeight = Math.Abs(vih.BmiHeader.Height);
                videoInfo.FrameWidth = vih.BmiHeader.Width;
                videoInfo.DisplayRatioWidth = videoInfo.FrameWidth;
                videoInfo.DisplayRatioHeight = videoInfo.FrameHeight;
                videoInfo.ColorFormat = Util.GetColorFormat(ref mt.subType);
                videoInfo.Duration = 0;
                videoInfo.StreamType = StreamType.UncompressedVideo;
                videoInfo.ScanType = ScanType.Progressive;


                switch (videoInfo.ColorFormat)
                {
                    case ColorFormat.BGR32:
                    case ColorFormat.BGRA32:
                    case ColorFormat.BGR24:
                    case ColorFormat.BGR444:
                    case ColorFormat.BGR555:
                    case ColorFormat.BGR565:
                        videoInfo.FrameBottomUp = (vih.BmiHeader.Height > 0);
                        break;
                }

                MediaSocket inputSocket = new MediaSocket();
                MediaPin inputPin = new MediaPin();
                inputPin.StreamInfo = videoInfo;
                inputSocket.Pins.Add(inputPin);
                inputSocket.StreamType = StreamType.UncompressedVideo;

                graph.videoGrabberCB.Init(transcoder, transcoder.Inputs.Count, graph.mediaControl);

                transcoder.Inputs.Add(inputSocket);
            }
            finally
            {
                DsUtils.FreeAMMediaType(mt);
            }
        }

        static void ConfigureAudioInput(DSGraph graph, Transcoder transcoder)
        {
            AMMediaType mt = new AMMediaType();
            int hr;

            try
            {
                hr = graph.audioGrabber.GetConnectedMediaType(mt);
                DsError.ThrowExceptionForHR(hr);

                if ((mt.majorType != DirectShowLib.MediaType.Audio) ||
                    (mt.formatType != DirectShowLib.FormatType.WaveEx))
                {
                    throw new COMException("Unexpected format type");
                }

                WaveFormatEx wfx = (WaveFormatEx)Marshal.PtrToStructure(mt.formatPtr, typeof(WaveFormatEx));

                AudioStreamInfo audioInfo = new AudioStreamInfo();
                audioInfo.BitsPerSample = wfx.wBitsPerSample;
                audioInfo.Channels = wfx.nChannels;
                audioInfo.SampleRate = wfx.nSamplesPerSec;
                audioInfo.StreamType = StreamType.LPCM;

                MediaSocket inputSocket = new MediaSocket();
                MediaPin inputPin = new MediaPin();
                inputPin.StreamInfo = audioInfo;
                inputSocket.Pins.Add(inputPin);
                inputSocket.StreamType = StreamType.LPCM;

                graph.audioGrabberCB.Init(transcoder, transcoder.Inputs.Count, graph.mediaControl);

                transcoder.Inputs.Add(inputSocket);
            }
            finally
            {
                DsUtils.FreeAMMediaType(mt);
            }
        }

        static void PrintError(string action, ErrorInfo e)
        {
            if (action != null)
            {
                Console.Write("{0}: ", action);
            }

            if (ErrorFacility.Success == e.Facility)
            {
                Console.WriteLine("Success");
                return;
            }
            else
            {
                Console.WriteLine("{0}, facility:{1} code:{2} hint:{3}", e.Message ?? "", e.Facility, e.Code, e.Hint ?? "");
            }
        }
    }
}
