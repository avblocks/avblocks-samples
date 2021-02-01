/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

using System;
using System.Reflection;
using System.IO;
using PrimoSoftware.AVBlocks;

namespace MuxMp4AvcAacFileSample
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

            bool encodeResult = MP4Mux(opt);

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

        static bool MP4Mux(Options opt)
        {
            try { File.Delete(opt.OutputFile); }
            catch (Exception) { }

            bool audioStreamDetected = false;
            bool videoStreamDetected = false;

            using (var transcoder = new Transcoder())
            {
                // Transcoder demo mode must be enabled,
                // in order to use the production release for testing (without a valid license).
                transcoder.AllowDemoMode = true;

                // configure inputs
                foreach (string inputFile in opt.InputFiles)
                {
                    using (MediaInfo info = new MediaInfo())
                    {
                        info.Inputs[0].File = inputFile;

                        if (!info.Open())
                        {
                            PrintError("mediaInfo.Open", info.Error);
                            return false;
                        }

                        MediaSocket inputSocket = MediaSocket.FromMediaInfo(info);

                        for (int i = 0; i < inputSocket.Pins.Count; i++)
                        {
                            MediaPin pin = inputSocket.Pins[i];

			                if(pin.StreamInfo.StreamType == StreamType.H264)
			                {
				                if(videoStreamDetected)
				                {
					                pin.Connection = PinConnection.Disabled;
				                }
				                else
				                {
					                videoStreamDetected = true;
                                    Console.WriteLine("Muxing video input: {0}", inputFile);
				                }
			                }
			                else if(pin.StreamInfo.StreamType == StreamType.Aac)
			                {
				                if(audioStreamDetected)
				                {
					                pin.Connection = PinConnection.Disabled;
				                }
				                else
				                {
					                audioStreamDetected = true;
                                    Console.WriteLine("Muxing audio input: {0}", inputFile);
				                }
			                }
			                else
			                {
				                pin.Connection = PinConnection.Disabled;
			                }
                        }

                        transcoder.Inputs.Add(inputSocket);
                    }
                }

	            // Configure output
	            {
                    MediaSocket socket = new MediaSocket();
                    socket.File = opt.OutputFile;
                    socket.StreamType = StreamType.Mp4;

		            if(videoStreamDetected)
		            {
			            VideoStreamInfo streamInfo = new VideoStreamInfo();
                        streamInfo.StreamType = StreamType.H264;
			            streamInfo.StreamSubType = StreamSubType.Avc1;

                        MediaPin pin = new MediaPin();
                        pin.StreamInfo = streamInfo;
                        socket.Pins.Add(pin);
		            }

                    if (audioStreamDetected)
                    {
                        AudioStreamInfo streamInfo = new AudioStreamInfo();
                        streamInfo.StreamType = StreamType.Aac;
                        streamInfo.StreamSubType = StreamSubType.AacMp4;

                        MediaPin pin = new MediaPin();
                        pin.StreamInfo = streamInfo;
                        socket.Pins.Add(pin);
                    }

		            if(opt.FastStart)
		            {
                        socket.Params.Add(Param.Muxer.MP4.FastStart, 1);
		            }

		            transcoder.Outputs.Add(socket);
	            }

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
