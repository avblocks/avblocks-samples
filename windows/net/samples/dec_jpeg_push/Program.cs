/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;

namespace DecodeJpegPushSample
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

            bool decodeResult = DecodeJpeg(opt.InputFile, opt.OutputFile);

            Library.Shutdown();

            return decodeResult ? (int)ExitCodes.Success : (int)ExitCodes.DecodeError;
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            DecodeError = 2,
        }

        static bool DecodeJpeg(string inputFile, string outputFile)
        {
            int frameWidth, frameHeight;

            if (!GetFrameSize(inputFile, out frameWidth, out frameHeight))
                return false;

            Console.WriteLine("Input frame size: {0}x{1}", frameWidth, frameHeight);

            // read input bytes
            byte[] inputData;
            try
            {
                inputData = System.IO.File.ReadAllBytes(inputFile);
            }
            catch (System.Exception e)
            {
                Console.WriteLine(e.ToString());
                return false;
            }

            DeleteFile(outputFile);

            MediaSocket inSocket = createInputSocket(frameWidth, frameHeight);
            MediaSocket outSocket = createOutputSocket(outputFile, frameWidth, frameHeight);

            // create Transcoder
            using (Transcoder transcoder = new Transcoder())
            {
                transcoder.AllowDemoMode = true;
                transcoder.Inputs.Add(inSocket);
                transcoder.Outputs.Add(outSocket);

                bool res = transcoder.Open();
                PrintError("Open Transcoder", transcoder.Error);
                if (!res)
                    return false;

                MediaBuffer buffer = new MediaBuffer();
                buffer.Attach(inputData, true);

                MediaSample sample = new MediaSample();
                sample.Buffer = buffer;

                res = transcoder.Push(0, sample);

                PrintError("Push Transcoder", transcoder.Error);
                if (!res)
                    return false;

                transcoder.Flush();
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

        static bool GetFrameSize(string inputFile, out int frameWidth, out int frameHeight)
        {
            frameWidth = 0;
            frameHeight = 0;

            using (MediaInfo info = new MediaInfo())
            {
                info.Inputs[0].File = inputFile;

                bool res = info.Open();
                PrintError("Open MediaInfo", info.Error);
                if (!res)
                    return false;

                foreach(var socket in info.Outputs)
                {
                    foreach(var pin in socket.Pins)
                    {
                        VideoStreamInfo vsi = (VideoStreamInfo)pin.StreamInfo;
                        frameWidth = vsi.FrameWidth;
                        frameHeight = vsi.FrameHeight;
                        return true;
                    }
                }
            }

            return false;
        }

        static MediaSocket createInputSocket(int frameWidth, int frameHeight)
        {
            MediaSocket socket = new MediaSocket();
            socket.StreamType = StreamType.Jpeg;
            socket.Stream = null;
            socket.File = null;

            MediaPin pin = new MediaPin();
            socket.Pins.Add(pin);
            VideoStreamInfo vsi = new VideoStreamInfo();
            pin.StreamInfo = vsi;

            vsi.StreamType = StreamType.Jpeg;
            vsi.ScanType = ScanType.Progressive;

            vsi.FrameWidth = frameWidth;
            vsi.FrameHeight = frameHeight;

            return socket;
        }

        static MediaSocket createOutputSocket(string outputFile, int frameWidth, int frameHeight)
        {
            MediaSocket socket = new MediaSocket();
            socket.File = outputFile;
            socket.StreamType = StreamType.UncompressedVideo;

            MediaPin pin = new MediaPin();
            socket.Pins.Add(pin);
            VideoStreamInfo vsi = new VideoStreamInfo();
            pin.StreamInfo = vsi;

            vsi.StreamType = StreamType.UncompressedVideo;
            vsi.ScanType = ScanType.Progressive;
            vsi.ColorFormat = ColorFormat.YUV420;

            vsi.FrameWidth = frameWidth;
            vsi.FrameHeight = frameHeight;

            return socket;
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