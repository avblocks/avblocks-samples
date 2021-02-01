/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;
using System.IO;

namespace DecAvcAuSample
{
    class Program
    {
        static int Main(string[] args)
        {
            var opt = new Options();

            if (!opt.Prepare(args))
                return opt.Error ? (int)ExitCodes.OptionsError : (int)ExitCodes.Success;

            Library.Initialize();

            // Set license information. To run AVBlocks in demo mode, comment the next line out
            // Library.SetLicense("<license-string>");

            bool transcodeResult = transcodeAUs(opt);

            Library.Shutdown();

            return transcodeResult ? (int)ExitCodes.Success : (int)ExitCodes.EncodingError;
        }

        static public bool setTranscode( Transcoder transcoder, string imgFile, Options opt)
        {
            using (var info = new MediaInfo())
            {
                info.Inputs[0].File = imgFile;

                if (!info.Open())
                {
                    PrintError("MediaInfo open", info.Error);
                    return false;
                }

                // prepare input socket
                var inSocket = MediaSocket.FromMediaInfo(info);
                inSocket.File = null;
                inSocket.Stream = null;

                // prepare output socket
                var outSocket = InitOutputSocket(opt, inSocket.Pins[0].StreamInfo);

                transcoder.Inputs.Add(inSocket);
                transcoder.Outputs.Add(outSocket);

                DeleteFile(opt.OutputFile);

                if (!transcoder.Open())
                {
                    PrintError("Transcoder open", transcoder.Error);
                    return false;
                }
            }

            return true;
        }

        static public MediaSocket InitOutputSocket(Options opt, StreamInfo streamInfo)
        {
            var outSocket = new MediaSocket();
            var outPin    = new MediaPin();
            var outVsi    = new VideoStreamInfo();

            if (opt.Color != null)
                outVsi.ColorFormat = (opt.Color.Id == ColorFormat.Unknown) ? ColorFormat.YUV420 : opt.Color.Id;
            else
                outVsi.ColorFormat = ColorFormat.YUV420;

            if (opt.Height > 0)
                outVsi.FrameHeight = opt.Height;

            if (opt.Width > 0)
                outVsi.FrameWidth = opt.Width;

            if (opt.Fps > 0)
                outVsi.FrameRate = opt.Fps;

            outVsi.StreamType = StreamType.UncompressedVideo;
            outVsi.ScanType   = ScanType.Progressive;
            outPin.StreamInfo = outVsi;

            outSocket.Pins.Add(outPin);

            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

            if (opt.OutputFile == null)
            {
                if (opt.Height > 0 && opt.Width > 0)
                {
                    opt.OutputFile = exeDir + "\\decoded_" +
                                     opt.FrameSize +
                                     ".yuv";
                }
                else
                {
                    VideoStreamInfo vsi = (VideoStreamInfo)streamInfo;
                    opt.OutputFile = exeDir + "\\decoded_" +
                                     vsi.FrameWidth +
                                     "x" +
                                     vsi.FrameHeight +
                                     ".yuv";
                }
            }

            outSocket.File       = opt.OutputFile;
            outSocket.StreamType = StreamType.UncompressedVideo;

            return outSocket;
        }

        static string BuildImgPath(Options opt, int index)
        {
            string pattern = "au_{0:0000}.h264";
            string path = opt.InputDir + "\\" + string.Format(pattern, index);

            return path;
        }

        static bool transcodeAUs(Options opt)
        {
            using (var transcoder = new Transcoder())
            {
                transcoder.AllowDemoMode = true;

                string imgFile = BuildImgPath(opt, 0);

                if (!setTranscode(transcoder, imgFile, opt))
                    return false;

                for (int i = 0; ; i++)
                {
                    if (i > 0)
                        imgFile = BuildImgPath(opt, i);

                    if (!File.Exists(imgFile))
                        break;

                    var sample = new MediaSample();
                    sample.Buffer = new MediaBuffer(File.ReadAllBytes(imgFile));

                    if (!transcoder.Push(0, sample))
                    {
                        PrintError("Transcoder push", transcoder.Error);
                        return false;
                    }
                }

                if (!transcoder.Flush())
                {
                    PrintError("Transcoder flush", transcoder.Error);
                    return false;
                }

                Console.WriteLine("Output file: " + opt.OutputFile);

                transcoder.Close();
            }

            return true;
        }

        static void DeleteFile(string filename)
        {
            try
            {
                if (System.IO.File.Exists(filename))
                    System.IO.File.Delete(filename);
            }
            catch { }
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

        enum ExitCodes : int
        {
            Success       = 0,
            OptionsError  = 1,
            EncodingError = 2,
        }
    }

    class YUVFile
    {
        public VideoStreamInfo vsi { get; set; }
        public MediaPin pin { get; set; }
        public MediaSocket socket { get; set; }

        public YUVFile(string file, ColorFormat color, int width, int height, double fps)
        {
            vsi    = new VideoStreamInfo();
            pin    = new MediaPin();
            socket = new MediaSocket();

            // set video stream properties
            vsi.StreamType = StreamType.UncompressedVideo;
            vsi.ScanType = ScanType.Progressive;
            vsi.ColorFormat = (color == ColorFormat.Unknown) ? ColorFormat.YUV420 : color;

            if (width > 0)
                vsi.FrameWidth = width;

            if (height > 0)
                vsi.FrameHeight = height;

            if (fps > 0.0)
                vsi.FrameRate = fps;

            // provide pin with stream info
            pin.StreamInfo = vsi;

            // set socket properties
            socket.File = file;
            socket.StreamType = StreamType.UncompressedVideo;

            // set socket pins 
            socket.Pins.Add(pin);
        }
    }

    class ElementaryStream
    {
        public VideoStreamInfo vsi { get; set; }
        public MediaPin pin { get; set; }
        public MediaSocket socket { get; set; }

        public ElementaryStream(StreamType streamType, int width, int height, double fps)
        {
            vsi    = new VideoStreamInfo();
            pin    = new MediaPin();
            socket = new MediaSocket();

            // set video stream properties
            vsi.StreamType = streamType;

            if (width > 0)
                vsi.FrameWidth = width;

            if (height > 0)
                vsi.FrameHeight = height;

            if (fps > 0.0)
                vsi.FrameRate = fps;

            // provide pin with stream info
            pin.StreamInfo = vsi;

            // set socket properties
            socket.StreamType = StreamType.H264;

            // set socket pin
            socket.Pins.Add(pin);

        } 
    } 
}
