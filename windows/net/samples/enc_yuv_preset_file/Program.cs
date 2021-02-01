/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.IO;
using System.Reflection;
using PrimoSoftware.AVBlocks;

namespace YuvEncoderSample
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

        enum ExitCodes: int
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

            using (var transcoder = new Transcoder())
            {
                // Transcoder demo mode must be enabled, 
                // in order to use the OEM release for testing (without a valid license).
                transcoder.AllowDemoMode = true;

                // Configure input
                // The input stream frame rate determines the playback speed
                var instream = new VideoStreamInfo {
                    StreamType = PrimoSoftware.AVBlocks.StreamType.UncompressedVideo,
                    FrameRate = opt.YuvFps, 
                    FrameWidth = opt.YuvWidth,
                    FrameHeight = opt.YuvHeight,
                    ColorFormat = opt.YuvColor.Id,
                    ScanType = ScanType.Progressive
                };

                var inpin = new MediaPin {
                    StreamInfo = instream
                };

                var insocket = new MediaSocket {
                    StreamType = PrimoSoftware.AVBlocks.StreamType.UncompressedVideo,
                    File = opt.YuvFile
                };

                insocket.Pins.Add(inpin);

                transcoder.Inputs.Add(insocket);

                // Configure output
                var outsocket = MediaSocket.FromPreset(opt.OutputPreset.Name);

                outsocket.File = opt.OutputFile;

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

            return true;
        }
    }
}
