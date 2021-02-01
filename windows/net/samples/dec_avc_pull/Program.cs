/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;

namespace DecAvcPullSample
{
    class Program
    {
        static int Main(string[] args)
        {
            var opt = new Options();

            if (!opt.Prepare(args))
                return opt.Error ? (int)ExitCodes.OptionsError : (int)ExitCodes.Success;

            Library.Initialize();

            bool decode = DecodeH264Stream(opt);

            Library.Shutdown();

            return decode ? (int)ExitCodes.Success : (int)ExitCodes.DecodingError;
        }

        static bool DecodeH264Stream(Options opt)
        {
            // Create an input socket from file
            MediaSocket inSocket = new MediaSocket();
            inSocket.File = opt.InputFile;

            // Create an output socket with one YUV 4:2:0 video pin
            VideoStreamInfo outStreamInfo = new VideoStreamInfo();
            outStreamInfo.StreamType = StreamType.UncompressedVideo;
            outStreamInfo.ColorFormat = ColorFormat.YUV420;
            outStreamInfo.ScanType = ScanType.Progressive;

            MediaPin outPin = new MediaPin();
            outPin.StreamInfo = outStreamInfo;

            MediaSocket outSocket = new MediaSocket();
            outSocket.StreamType = StreamType.UncompressedVideo;

            outSocket.Pins.Add(outPin);

            // Create Transcoder
            using (var transcoder = new Transcoder())
            {
                transcoder.AllowDemoMode = true;
                transcoder.Inputs.Add(inSocket);
                transcoder.Outputs.Add(outSocket);

                if (transcoder.Open())
                {
                    DeleteFile(opt.OutputFile);

                    int inputIndex;
                    MediaSample yuvFrame = new MediaSample();

                    int frameCounter = 0;

                    using (System.IO.FileStream outfile = System.IO.File.OpenWrite(opt.OutputFile))
                    {
                        while (transcoder.Pull(out inputIndex, yuvFrame))
                        {
                            // Each call to Transcoder::pull returns a raw YUV 4:2:0 frame. 
                            outfile.Write(yuvFrame.Buffer.Start, yuvFrame.Buffer.DataOffset, yuvFrame.Buffer.DataSize);
                            ++frameCounter;
                        }

                        PrintError("Transcoder pull", transcoder.Error);

                        Console.WriteLine("Frames decoded: {0}", frameCounter);
                        Console.WriteLine("Output file: {0}", opt.OutputFile);


                        outfile.Close();
                    }

                    transcoder.Close();
                    return true;
                }

                PrintError("Transcoder open", transcoder.Error);

                return false;
            }
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
                Console.WriteLine("{0}, facility:{1} code:{2} hint:{3}", e.Message ?? "", (int)e.Facility, e.Code, e.Hint ?? "");
            }
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            DecodingError = 2,
        }
    }
}
