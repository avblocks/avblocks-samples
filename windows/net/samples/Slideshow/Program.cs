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

namespace SlideshowSample
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

        static bool Encode(Options opt)
        {
            string outFilename = "cube." + opt.FileExtension;
            const int imageCount = 250;
            const double inputFrameRate = 25.0;

            using (var transcoder = new Transcoder())
            {
                // In order to use the OEM release for testing (without a valid license),
                // the transcoder demo mode must be enabled.
                transcoder.AllowDemoMode = true;

                try
                {
                    bool result;

                    try
                    {
                        File.Delete(outFilename);
                    }catch{}

                    // Configure Input
                    {
                        using (MediaInfo medInfo = new MediaInfo())
                        {
                            medInfo.Inputs[0].File = GetImagePath(0);

                            result = medInfo.Open();
                            PrintError("Open MediaInfo", medInfo.Error);
                            if (!result)
                                return false;

                            VideoStreamInfo vidInfo = (VideoStreamInfo)medInfo.Outputs[0].Pins[0].StreamInfo.Clone();
                            vidInfo.FrameRate = inputFrameRate;

                            MediaPin pin = new MediaPin();
                            pin.StreamInfo = vidInfo;

                            MediaSocket socket = new MediaSocket();
                            socket.Pins.Add(pin);

                            transcoder.Inputs.Add(socket);
                        }
                    }

                    // Configure Output
                    {
                        MediaSocket socket = MediaSocket.FromPreset(opt.PresetID);
                        socket.File = outFilename;

                        transcoder.Outputs.Add(socket);
                    }

                    // Encode Images
                    result = transcoder.Open();
                    PrintError("Open Transcoder", transcoder.Error);
                    if (!result)
                        return false;

                    for (int i = 0; i < imageCount; i++)
                    {
                        string imagePath = GetImagePath(i);

                        MediaBuffer mediaBuffer = new MediaBuffer(File.ReadAllBytes(imagePath));

                        MediaSample mediaSample = new MediaSample();
                        mediaSample.StartTime = i / inputFrameRate;
                        mediaSample.Buffer = mediaBuffer;

                        if (!transcoder.Push(0, mediaSample))
                        {
                            PrintError("Push Transcoder", transcoder.Error);
                            return false;
                        }
                    }

                    result = transcoder.Flush();
                    PrintError("Flush Transcoder", transcoder.Error);
                    if (!result)
                        return false;

                    transcoder.Close();
                    Console.WriteLine("Output video: \"{0}\"", outFilename);
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.ToString());
                    return false;
                }
            }

            return true;
        }

        static string GetImagePath(int imageNumber)
        {
            string exeDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            return Path.Combine(exeDir, string.Format(@"..\assets\img\cube{0:0000}.jpeg", imageNumber));
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
