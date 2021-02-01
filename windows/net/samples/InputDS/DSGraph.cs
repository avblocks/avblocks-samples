/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.IO;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using DirectShowLib;

namespace InputDS
{
    class DSGraph
    {
        public IGraphBuilder graph;
        public IMediaControl mediaControl;
        public IMediaEventEx mediaEvent;

        public IBaseFilter videoGrabberFilter;
        public ISampleGrabber videoGrabber;
        public SampleGrabberCB videoGrabberCB;
        IBaseFilter videoNullFilter;

        public IBaseFilter audioGrabberFilter;
        public ISampleGrabber audioGrabber;
        public SampleGrabberCB audioGrabberCB;
        IBaseFilter audioNullFilter;

        public void Init(string inputFile)
        {
            Init(inputFile, null);
        }

        public void Init(IBaseFilter sourceFilter)
        {
            Init(null, sourceFilter);
        }

        void Init(string inputFile, IBaseFilter userSourceFilter)
        {
            Reset();

            if (!string.IsNullOrEmpty(inputFile) && (userSourceFilter != null))
                throw new ArgumentException("Specify only one kind of input");

            graph = new FilterGraph() as IFilterGraph2;
            if (null == graph)
                throw new COMException("Cannot create FilterGraph");

            mediaControl = graph as IMediaControl;
            if (null == mediaControl)
                throw new COMException("Cannot obtain IMediaControl");

            mediaEvent = graph as IMediaEventEx;
            if (null == mediaEvent)
                throw new COMException("Cannot obtain IMediaEventEx");

            // remove reference clock
            IMediaFilter mf = graph as IMediaFilter;
            mf.SetSyncSource(null);

            int hr = 0;

            string sourceFilterInfoDumpPath = Path.Combine(Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location), "source_filter_info_dump.txt");

            if (!string.IsNullOrEmpty(inputFile))
            {
                IBaseFilter sourceFilter = null;

                try
                {
                    hr = graph.AddSourceFilter(inputFile, "Source", out sourceFilter);
                    DsError.ThrowExceptionForHR(hr);

                    System.IO.File.WriteAllText(sourceFilterInfoDumpPath, Util.DumpFilterInfo(sourceFilter));

                    InitVideoGrabber(sourceFilter);
                    InitAudioGrabber(sourceFilter);
                }
                finally
                {
                    Util.ReleaseComObject(ref sourceFilter);
                }
            }
            else
            {
                hr = graph.AddFilter(userSourceFilter, "Source");
                DsError.ThrowExceptionForHR(hr);

                System.IO.File.WriteAllText(sourceFilterInfoDumpPath, Util.DumpFilterInfo(userSourceFilter));

                InitVideoGrabber(userSourceFilter);
                InitAudioGrabber(userSourceFilter);
            }
        }

        private void InitVideoGrabber(IBaseFilter sourceF)
        {
            videoGrabberFilter = new SampleGrabber() as IBaseFilter;
            if (videoGrabberFilter == null)
                throw new COMException("Cannot create SampleGrabber");

            int hr = graph.AddFilter(videoGrabberFilter, "Video Sample Grabber");
            DsError.ThrowExceptionForHR(hr);

            videoGrabber = videoGrabberFilter as ISampleGrabber;
            if (videoGrabber == null)
                throw new COMException("Cannot obtain ISampleGrabber");

            {
                AMMediaType mt = new AMMediaType();
                mt.majorType = DirectShowLib.MediaType.Video;
                mt.subType = DirectShowLib.MediaSubType.RGB24;

                hr = videoGrabber.SetMediaType(mt);
                DsError.ThrowExceptionForHR(hr);

                DsUtils.FreeAMMediaType(mt);
            }

            hr = ConnectSampleGrabber(graph, sourceF, videoGrabberFilter);

            if (0 != hr)
            {
                // Cannot connect the video grabber. Remove the filter from the graph.
                hr = graph.RemoveFilter(videoGrabberFilter);
                DsError.ThrowExceptionForHR(hr);

                Util.ReleaseComObject(ref videoGrabberFilter);
                videoGrabber = null;
                return;
            }

            videoNullFilter = new NullRenderer() as IBaseFilter;
            if (videoNullFilter == null)
                throw new COMException("Cannot create NullRenderer");

            hr = graph.AddFilter(videoNullFilter, "Null Filter");
            DsError.ThrowExceptionForHR(hr);

            hr = Util.ConnectFilters(graph, videoGrabberFilter, videoNullFilter);
            DsError.ThrowExceptionForHR(hr);

            videoGrabberCB = new SampleGrabberCB();
            hr = videoGrabber.SetCallback(videoGrabberCB, (int)CBMethod.Sample);
            DsError.ThrowExceptionForHR(hr);
        }

        private void InitAudioGrabber(IBaseFilter sourceF)
        {
            audioGrabberFilter = new SampleGrabber() as IBaseFilter;
            if (audioGrabberFilter == null)
                throw new COMException("Cannot create SampleGrabber");

            int hr = graph.AddFilter(audioGrabberFilter, "Audio Sample Grabber");
            DsError.ThrowExceptionForHR(hr);

            audioGrabber = audioGrabberFilter as ISampleGrabber;
            if (audioGrabber == null)
                throw new COMException("Cannot obtain ISampleGrabber");

            {
                AMMediaType mt = new AMMediaType();
                mt.majorType = DirectShowLib.MediaType.Audio;
                mt.subType = DirectShowLib.MediaSubType.PCM;

                hr = audioGrabber.SetMediaType(mt);
                DsError.ThrowExceptionForHR(hr);

                DsUtils.FreeAMMediaType(mt);
            }

            hr = ConnectSampleGrabber(graph, sourceF, audioGrabberFilter);

            if (0 != hr)
            {
                // Cannot connect the audio grabber. Remove the filter from the graph.
                hr = graph.RemoveFilter(audioGrabberFilter);
                DsError.ThrowExceptionForHR(hr);

                Util.ReleaseComObject(ref audioGrabberFilter);
                audioGrabber = null;
                return;
            }

            audioNullFilter = new NullRenderer() as IBaseFilter;
            if (audioNullFilter == null)
                throw new COMException("Cannot create NullRenderer");

            hr = graph.AddFilter(audioNullFilter, "Null Filter");
            DsError.ThrowExceptionForHR(hr);

            hr = Util.ConnectFilters(graph, audioGrabberFilter, audioNullFilter);
            DsError.ThrowExceptionForHR(hr);

            audioGrabberCB = new SampleGrabberCB();
            hr = audioGrabber.SetCallback(audioGrabberCB, (int)CBMethod.Sample);
            DsError.ThrowExceptionForHR(hr);
        }

        static int ConnectSampleGrabber(IGraphBuilder graph, IBaseFilter src, IBaseFilter dest)
        {
            if ((graph == null) || (src == null) || (dest == null))
                return WinAPI.E_FAIL;

            IEnumPins enumPins = null;

            // try to connect to source filter unconnected output pins
            try
            {
                IPin[] pins = new IPin[1] { null };

                int hr = src.EnumPins(out enumPins);
                DsError.ThrowExceptionForHR(hr);

                while (enumPins.Next(1, pins, IntPtr.Zero) == 0)
                {
                    try
                    {
                        PinDirection thisPinDir;
                        hr = pins[0].QueryDirection(out thisPinDir);

                        if (hr == 0 && thisPinDir == PinDirection.Output)
                        {
                            IPin tmpPin = null;

                            hr = pins[0].ConnectedTo(out tmpPin);
                            if (tmpPin != null)  // Already connected, not the pin we want.
                            {
                                Util.ReleaseComObject(ref tmpPin);
                            }
                            else  // Unconnected, this is the pin we want.
                            {
                                hr = Util.ConnectFilters(graph, pins[0], dest);

                                if (0 == hr)
                                    return 0;
                            }
                        }
                    }
                    finally
                    {
                        Util.ReleaseComObject(ref pins[0]);
                    }
                }
            }
            catch (COMException)
            {
            }
            finally
            {
                Marshal.ReleaseComObject(enumPins);
            }


            // try to connect to filters connected to the source filter
            try
            {
                IPin[] pins = new IPin[1] { null };

                int hr = src.EnumPins(out enumPins);
                DsError.ThrowExceptionForHR(hr);

                while (enumPins.Next(1, pins, IntPtr.Zero) == 0)
                {
                    try
                    {
                        PinDirection thisPinDir;
                        hr = pins[0].QueryDirection(out thisPinDir);

                        if (hr == 0 && thisPinDir == PinDirection.Output)
                        {
                            IPin tmpPin = null;

                            hr = pins[0].ConnectedTo(out tmpPin);
                            if (tmpPin != null)  // Already connected, the pin we want.
                            {
                                try
                                {
                                    PinInfo pinInfo;
                                    hr = tmpPin.QueryPinInfo(out pinInfo);
                                    IBaseFilter connectedTo = null;
                                    if(hr == 0)
                                        connectedTo = pinInfo.filter;

                                    if (connectedTo != null)
                                    {
                                        hr = ConnectSampleGrabber(graph, connectedTo, dest);
                                        if (0 == hr)
                                            return hr;
                                    }
                                }
                                finally
                                {
                                    Util.ReleaseComObject(ref tmpPin);
                                }
                            }
                        }
                    }
                    finally
                    {
                        Util.ReleaseComObject(ref pins[0]);
                    }
                }
            }
            catch (COMException)
            {
            }
            finally
            {
                Marshal.ReleaseComObject(enumPins);
            }

            return WinAPI.E_FAIL;
        }

        public void Reset()
        {
            mediaControl = null;
            mediaEvent = null;
            Util.ReleaseComObject(ref graph);

            Util.ReleaseComObject(ref videoGrabberFilter);
            videoGrabber = null;
            Util.ReleaseComObject(ref videoNullFilter);
            videoGrabberCB = null;

            Util.ReleaseComObject(ref audioGrabberFilter);
            audioGrabber = null;
            Util.ReleaseComObject(ref audioNullFilter);
            audioGrabberCB = null;
        }
    };
}
