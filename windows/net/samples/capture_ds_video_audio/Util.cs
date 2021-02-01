using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using DirectShowLib;
using System.Diagnostics;
using PrimoSoftware.AVBlocks;
using System.Linq;

namespace CaptureDS
{
    static class Util
    {
        public const int WM_STOP_CAPTURE = WinAPI.WM_APP + 1;

        struct ColorSpaceEntry
        {
            public Guid videoSubType;
            public ColorFormat colorFormat;

            public ColorSpaceEntry(Guid videoSubType, ColorFormat colorFormat)
            {
                this.videoSubType = videoSubType;
                this.colorFormat = colorFormat;
            }
        };

        private static ColorSpaceEntry[] ColorSpaceTab =
        {
            new ColorSpaceEntry(MediaSubType.RGB24,	ColorFormat.BGR24),
            new ColorSpaceEntry(MediaSubType.ARGB32,ColorFormat.BGRA32), // with alpha
            new ColorSpaceEntry(MediaSubType.RGB32,	ColorFormat.BGR32),
            new ColorSpaceEntry(MediaSubType.RGB565,ColorFormat.BGR565),
            new ColorSpaceEntry(MediaSubType.ARGB1555,ColorFormat.BGR555),// with alpha
            new ColorSpaceEntry(MediaSubType.RGB555,ColorFormat.BGR555),// with alpha
            new ColorSpaceEntry(MediaSubType.ARGB4444,ColorFormat.BGR444),// with alpha

            // tested mappings
            new ColorSpaceEntry(MediaSubType.YV12, ColorFormat.YV12),
            new ColorSpaceEntry(MediaSubType.I420, ColorFormat.YUV420),
            new ColorSpaceEntry(MediaSubType.IYUV, ColorFormat.YUV420),
            new ColorSpaceEntry(MediaSubType.YUY2, ColorFormat.YUY2),

            // possible mappings
            new ColorSpaceEntry(MediaSubType.NV12, ColorFormat.NV12),
            new ColorSpaceEntry(MediaSubType.UYVY, ColorFormat.UYVY),
            new ColorSpaceEntry(MediaSubType.Y411, ColorFormat.Y411),
            new ColorSpaceEntry(MediaSubType.Y41P, ColorFormat.Y41P),
            new ColorSpaceEntry(MediaSubType.YVU9, ColorFormat.YVU9),
        };
        
        public static void ReleaseComObject<T>(ref T comObject)
        {
            if (comObject != null)
            {
                Marshal.ReleaseComObject(comObject);

                comObject = default(T);
            }
        }

        public static void DisposeObject<T>(ref T obj) where T:IDisposable
        {
            if (obj != null)
            {
                obj.Dispose();
                obj = default(T);
            }
        }

        public static int GetPin(IBaseFilter filter, PinDirection pinDir, string name, out IPin ppPin)
        {
            ppPin = null;

            IEnumPins enumPins = null;
            IPin[] pins = new IPin[1] { null };

            try
            {
                int hr = filter.EnumPins(out enumPins);
                DsError.ThrowExceptionForHR(hr);

                while (enumPins.Next(1, pins, IntPtr.Zero) == 0)
                {
                    PinInfo pi;
                    hr = pins[0].QueryPinInfo(out pi);

                    bool found = false;
                    if (hr == 0 && pi.dir == pinDir && pi.name == name)
                    {
                        found = true;
                        
                        ppPin = pins[0];
                        
                        DsUtils.FreePinInfo(pi);
                    }

                    if (found)
                        return 0;
                   
                    Util.ReleaseComObject(ref pins[0]);
                }

                // Did not find a matching pin.
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

        public static int GetUnconnectedPin(IBaseFilter filter, PinDirection pinDir, out IPin ppPin)
        {
            ppPin = null;

            IEnumPins enumPins = null;
            IPin[] pins = new IPin[1] { null };
            IPin tmpPin = null;

            try
            {
                int hr = filter.EnumPins(out enumPins);
                DsError.ThrowExceptionForHR(hr);

                while (enumPins.Next(1, pins, IntPtr.Zero) == 0)
                {
                    PinDirection thisPinDir;
                    hr = pins[0].QueryDirection(out thisPinDir);

                    if (hr == 0 && thisPinDir == pinDir)
                    {
                        hr = pins[0].ConnectedTo(out tmpPin);
                        if (tmpPin != null)  // Already connected, not the pin we want.
                        {
                            Util.ReleaseComObject(ref tmpPin);
                        }
                        else  // Unconnected, this is the pin we want.
                        {
                            ppPin = pins[0];
                            return 0;
                        }
                    }

                    Util.ReleaseComObject(ref pins[0]);
                }

                // Did not find a matching pin.
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


        public static int ConnectFilters(IGraphBuilder graph, IBaseFilter src, IBaseFilter dest)
        {
            if ((graph == null) || (src == null) || (dest == null))
            {
                return WinAPI.E_FAIL;
            }

            // Find an output pin on the upstream filter.
            IPin pinOut = null;
            IPin pinIn = null;

            try
            {
                int hr = GetUnconnectedPin(src, PinDirection.Output, out pinOut);
                DsError.ThrowExceptionForHR(hr);

                // Find an input pin on the downstream filter.

                hr = GetUnconnectedPin(dest, PinDirection.Input, out pinIn);
                DsError.ThrowExceptionForHR(hr);

                // Try to connect them.
                hr = graph.ConnectDirect(pinOut, pinIn, null);
                DsError.ThrowExceptionForHR(hr);

                return 0;
            }
            catch (COMException)
            {
            }
            finally
            {
                Util.ReleaseComObject(ref pinIn);
                Util.ReleaseComObject(ref pinOut);
            }

            return WinAPI.E_FAIL;
        }

        public static ColorFormat GetColorFormat(ref Guid videoSubtype)
        {
            for (int i = 0; i < ColorSpaceTab.Length; i++)
            {
                if (ColorSpaceTab[i].videoSubType == videoSubtype)
                    return ColorSpaceTab[i].colorFormat;
            }

            return ColorFormat.Unknown;
        }

        // Tear down everything downstream of a given filter
        public static int NukeDownstream(IFilterGraph graph, IBaseFilter filter)
        {
            if (filter == null)
                return WinAPI.E_FAIL;

            IEnumPins enumPins = null;
            IPin[] pins = new IPin[1] { null };

            try
            {
                int hr = filter.EnumPins(out enumPins);
                DsError.ThrowExceptionForHR(hr);
                enumPins.Reset(); // start at the first pin

                while (enumPins.Next(1, pins, IntPtr.Zero) == 0)
                {
                    if (pins[0] != null)
                    {
                        PinDirection pindir;
                        pins[0].QueryDirection(out pindir);
                        if (pindir == PinDirection.Output)
                        {
                            IPin pTo = null;
                            pins[0].ConnectedTo(out pTo);
                            if (pTo != null)
                            {
                                PinInfo pi;
                                hr = pTo.QueryPinInfo(out pi);

                                if (hr == 0)
                                {
                                    NukeDownstream(graph, pi.filter);

                                    graph.Disconnect(pTo);
                                    graph.Disconnect(pins[0]);
                                    graph.RemoveFilter(pi.filter);

                                    Util.ReleaseComObject(ref pi.filter);
                                    DsUtils.FreePinInfo(pi);
                                }
                                Marshal.ReleaseComObject(pTo);
                            }
                        }
                        Marshal.ReleaseComObject(pins[0]);
                    }
                }

                return 0;
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

        public static string AudioLogForVideoFile(string videoFile, PresetDescriptor preset)
        {
            var audioPin = Util.MediaSocketFromPreset(preset.Name).Pins.FirstOrDefault(pin =>
                (pin.StreamInfo.MediaType == PrimoSoftware.AVBlocks.MediaType.Audio));

            if (audioPin == null)
                return string.Empty;

            var ext = DefaulAudioFileExtension(audioPin.StreamInfo);

            if (ext == null)
                return string.Empty;

            return System.IO.Path.ChangeExtension(videoFile, ext);
        }

        public static string DefaulAudioFileExtension(StreamInfo si)
        {
            switch (si.StreamType) {
                case StreamType.Aac: return "aac";
                case StreamType.MpegAudio:
                    switch (si.StreamSubType) {
                        case StreamSubType.MpegAudioLayer2: return "mp2";
                        case StreamSubType.MpegAudioLayer3: return "mp3";
                        default: return "mpa";
                    }
                case StreamType.LPCM: return "pcm";
                default: return null;
            }
        }

        public static void SetH264FastEncoding(IDictionary<string, object> plist)
        {
            plist.Add(Param.Encoder.Video.H264.Profile, H264Profile.Baseline);
            plist.Add(Param.Encoder.Video.H264.EntropyCodingMode, H264EntropyCodingMode.CAVLC);
            plist.Add(Param.Encoder.Video.H264.NumBFrames, 0);
            plist.Add(Param.Encoder.Video.H264.NumRefFrames, 1);
            plist.Add(Param.Encoder.Video.H264.Transform8x8, false);
            plist.Add(Param.Encoder.Video.H264.KeyFrameInterval, 15);
            plist.Add(Param.Encoder.Video.H264.KeyFrameIDRInterval, 1);
            plist.Add(Param.Encoder.Video.H264.QualitySpeed, 0); // max speed
            plist.Add(Param.Encoder.Video.H264.RateControlMethod, H264RateControlMethod.ConstantQuant);
            plist.Add(Param.Encoder.Video.H264.RateControlQuantI, 26); // 0-51, default 20
            plist.Add(Param.Encoder.Video.H264.RateControlQuantP, 26); // 0-51, default 20
            plist.Add(Param.Encoder.Video.H264.DeblockingFilter, H264DeblockingFilter.InSlice);
            plist.Add(Param.Encoder.Video.H264.MESplitMode, H264MeSplitMode.Only16x16);
            plist.Add(Param.Encoder.Video.H264.MEMethod, H264MeMethod.UMH);
        }

        public static MediaSocket CustomOutputSocket(int frameWidth, int frameHeight, double framerate)
        {
            var socket = new MediaSocket();

            // video pin
            socket.Pins.Add(new MediaPin()
            {
                StreamInfo = new VideoStreamInfo()
                {
                    StreamType = StreamType.H264,
                    FrameWidth = frameWidth,
                    FrameHeight = frameHeight,
                    FrameRate = framerate,
                    DisplayRatioWidth = frameWidth,
                    DisplayRatioHeight = frameHeight,
                },
            });

            /*
             * Set H264 encoder params on the output pin to get the best encoding speed
             */
            SetH264FastEncoding(socket.Pins[0].Params);
            socket.Pins[0].Params.Add(Param.Encoder.Video.H264.FixedFramerate, false);

            // audio pin
            socket.Pins.Add(new MediaPin()
            {
                StreamInfo = new AudioStreamInfo()
                {
                    StreamType = StreamType.Aac,
                    SampleRate = 44100,
                    Channels = 2
                },
            });

            return socket;
        }

        public static MediaSocket MediaSocketFromPreset(string presetName)
        {
            // custom presets
            if (presetName == "custom-mp4-h264-704x576-25fps-aac") {
                return CustomOutputSocket(704, 576, 25);
            }
            else if (presetName == "custom-mp4-h264-704x576-12fps-aac") {
                return CustomOutputSocket(704, 576, 12);
            }
            else if (presetName == "custom-mp4-h264-352x288-25fps-aac") {
                return CustomOutputSocket(352, 288, 25);
            }
            else if (presetName == "custom-mp4-h264-352x288-12fps-aac") {
                return CustomOutputSocket(352, 288, 12);
            }
            else {
                // built-in presets
                return MediaSocket.FromPreset(presetName);
            }
        }

        public static bool IsSampleEmpty(MediaSample sample)
        {
            if (sample != null) {
                if (sample.Buffer != null && sample.Buffer.DataSize > 0)
                    return false;

                if (sample.UnmanagedBuffer != null && sample.UnmanagedBuffer.DataSize > 0)
                    return false;
            }
            return true;
        }
    }
}
