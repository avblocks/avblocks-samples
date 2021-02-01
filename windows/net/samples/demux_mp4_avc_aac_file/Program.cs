/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

using System;
using System.IO;
using PrimoSoftware.AVBlocks;

namespace DemuxMp4AvcAacFileSample
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

            bool encodeResult = demuxMP4(opt);

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

        static string GenerateOutputFileName(string fileName, int fileIndex, string type)
        {
            string outputFileName = fileName + string.Format(".{0:000}", fileIndex) + "." + type;

            Console.WriteLine("Output file: " + outputFileName);

            return outputFileName;
        }

        static Transcoder createTranscoder(Options opt)
        {
            var transcoder = new Transcoder() { AllowDemoMode = true };

            using (MediaInfo info = new MediaInfo())
            {
                info.Inputs[0].File = opt.InputFile;

                if (!info.Open())
                {
                    PrintError("MediaInfo open: ", info.Error);
                    return null;
                }

                MediaSocket inSocket = MediaSocket.FromMediaInfo(info);

                transcoder.Inputs.Add(inSocket);

                int vIndex = 0;
                int aIndex = 0;

                for (int i = 0; i < inSocket.Pins.Count; i++)
                {
                    int fileIndex;
                    string type;
                    if (inSocket.Pins[i].StreamInfo.MediaType == MediaType.Audio)
                    {
                        fileIndex = ++aIndex;
                        type = "aac";
                    }
                    else if (inSocket.Pins[i].StreamInfo.MediaType == MediaType.Video)
                    {
                        fileIndex = ++vIndex;
                        type = "h264";
                    }
                    else
                        continue;

                    MediaSocket outSocket = new MediaSocket();
                    MediaPin pin = (MediaPin)inSocket.Pins[i].Clone();
                    outSocket.Pins.Add(pin);

                    outSocket.File = GenerateOutputFileName(opt.OutputFile, fileIndex, type);
                    File.Delete(outSocket.File);

                    transcoder.Outputs.Add(outSocket);
                }

                return transcoder;
            }
        }

        static bool demuxMP4(Options opt)
        {
            using (var transcoder = createTranscoder(opt))
            {
                if (!transcoder.Open())
                {
                    PrintError("Transcoder open: ", transcoder.Error);
                    return false;
                }

                if (!transcoder.Run())
                {
                    PrintError("Transcoder run: ", transcoder.Error);
                    return false;
                }

                return true;
            }
        }
    }
}
