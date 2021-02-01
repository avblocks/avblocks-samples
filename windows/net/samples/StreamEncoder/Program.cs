/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.Reflection;
using System.IO;
using PrimoSoftware.AVBlocks;

namespace StreamEncoderSample
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

            bool encodeResult = Encode(opt);

            Library.Shutdown();

            return encodeResult ? (int)ExitCodes.Success : (int)ExitCodes.EncodeError;
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            EncodeError = 2,
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

        static bool Encode(Options opt)
        {
            try { File.Delete(opt.OutputFile); }
            catch { }

            System.IO.Stream inputStream = null;
            System.IO.Stream outputStream = null;

            try
            {
                inputStream = new System.IO.FileStream(opt.YuvFile, FileMode.Open, FileAccess.Read, FileShare.Read);
                outputStream = new System.IO.FileStream(opt.OutputFile, FileMode.Create, FileAccess.Write, FileShare.None);

                using (var transcoder = new Transcoder())
                {
                    // In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
                    transcoder.AllowDemoMode = true;

                    // Configure input
                    var instream = new VideoStreamInfo
                    {
                        FrameRate = opt.YuvFps, // the input frame rate determines how fast the video is played
                        FrameWidth = opt.YuvWidth,
                        FrameHeight = opt.YuvHeight,
                        ColorFormat = opt.YuvColor.Id,
                        StreamType = PrimoSoftware.AVBlocks.StreamType.UncompressedVideo,
                        ScanType = ScanType.Progressive
                    };

                    var inpin = new MediaPin
                    {
                        StreamInfo = instream
                    };

                    var insocket = new MediaSocket
                    {
                        Stream = inputStream,
                        StreamType = PrimoSoftware.AVBlocks.StreamType.UncompressedVideo
                    };


                    insocket.Pins.Add(inpin);

                    transcoder.Inputs.Add(insocket);

                    // Configure output
                    var outsocket = MediaSocket.FromPreset(opt.OutputPreset.Name);
                    outsocket.Stream = outputStream;

                    transcoder.Outputs.Add(outsocket);

                    bool res = transcoder.Open();
                    PrintError("Open Transcoder", transcoder.Error);
                    if (!res)
                        return false;

                    res = transcoder.Run();
                    PrintError("Run Transcoder", transcoder.Error);
                    if (!res)
                        return false;

                    transcoder.Close();
                }
            }
            finally
            {
                if (inputStream != null)
                {
                    inputStream.Dispose();
                    inputStream = null;
                }

                if (outputStream != null)
                {
                    outputStream.Dispose();
                    outputStream = null;
                }
            }

            return true;
        }
    }
}
