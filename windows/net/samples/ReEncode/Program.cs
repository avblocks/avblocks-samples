/*
 *  Copyright (c) 2014 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.Reflection;
using System.IO;
using System.Collections.Generic;
using PrimoSoftware.AVBlocks;

namespace ReEncodeSample
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

            bool encodeResult = ReEncode(opt);

            Library.Shutdown();

            return encodeResult ? (int)ExitCodes.Success : (int)ExitCodes.EncodeError;
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            EncodeError = 2,
        }

        static bool ReEncode(Options opt)
        {
            if (File.Exists(opt.OutputFile))
                File.Delete(opt.OutputFile);

            using (var transcoder = new Transcoder())
            {
                // In order to use the production release for testing (without a valid license),
                // the transcoder demo mode must be enabled.
                transcoder.AllowDemoMode = true;

                using (var mediaInfo = new MediaInfo())
                {
                    mediaInfo.Inputs[0].File = opt.InputFile;

                    if (!mediaInfo.Open())
                    {
                        PrintError("Open MediaInfo", mediaInfo.Error);
                        return false;
                    }

                    // Add Inputs
                    {
                        var socket = MediaSocket.FromMediaInfo(mediaInfo);
                        transcoder.Inputs.Add(socket);
                    }
                }

                // Add Outputs
                {
                    // Create output socket
                    var socket = new MediaSocket();
                    var inSocket = transcoder.Inputs[0];

                    socket.StreamType = inSocket.StreamType;
                    socket.File = opt.OutputFile;

                    // Add pins with ReEncode parameter set to Use.On
                    foreach (var inPin in inSocket.Pins)
                    {
                        StreamInfo si = (StreamInfo)inPin.StreamInfo.Clone();
                        var pin = new MediaPin();
                        pin.StreamInfo = (StreamInfo)si.Clone();

                        if ((MediaType.Video == si.MediaType) && opt.ReEncodeVideo)
                        {
                            pin.Params.Add(Param.ReEncode, Use.On);
                        }

                        if ((MediaType.Audio == si.MediaType) && opt.ReEncodeAudio)
                        {
                            pin.Params.Add(Param.ReEncode, Use.On);
                        }

                        socket.Pins.Add(pin);
                    }

                    transcoder.Outputs.Add(socket);
                }
                

                bool result = transcoder.Open();
	            PrintError("Open Transcoder", transcoder.Error);
	            if (!result)
		            return false;

	            result = transcoder.Run();
                PrintError("Run Transcoder", transcoder.Error);
	            if (!result)
		            return false;

	            transcoder.Close();
            }

            return true;
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
