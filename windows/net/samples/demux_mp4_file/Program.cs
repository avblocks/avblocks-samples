/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;

namespace DemuxMp4FileSample
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

            bool demuxResult = DemuxMP4(opt);

            Library.Shutdown();

            return demuxResult ? (int)ExitCodes.Success : (int)ExitCodes.DecodingError;
        }

        static bool DemuxMP4(Options opt)
        {
            using (var transcoder = GenerateOutputFileName(opt))
            {
                if (transcoder == null)
                    return false;

                if (!transcoder.Open())
                {
                    PrintError("Transcoder open", transcoder.Error);
                    return false;
                }

                if (!transcoder.Run())
                {
                    PrintError("Transcoder run", transcoder.Error);
                    return false;
                }

                transcoder.Close();
                return true;
            }
        }

        static Transcoder GenerateOutputFileName(Options opt)
        {
            using (var info = new MediaInfo())
            {
                info.Inputs[0].File = opt.InputFile;

                if (!info.Open())
                {
                    PrintError("MediaInfo open", info.Error);
                    return null;
                }

                MediaSocket inSocket = MediaSocket.FromMediaInfo(info);

                info.Close();

                Transcoder transcoder = new Transcoder();
                transcoder.AllowDemoMode = true;
                transcoder.Inputs.Add(inSocket);

                bool audio = false;
                bool video = false;

                for (int i = 0; i < inSocket.Pins.Count; ++i)
                {
                    string fileName;
                    if (inSocket.Pins[i].StreamInfo.MediaType == MediaType.Audio && !audio)
                    {
                        audio = true;
                        fileName = opt.OutputFile + ".aud.mp4";
                    }
                    else if (inSocket.Pins[i].StreamInfo.MediaType == MediaType.Video && !video)
                    {
                        video = true;
                        fileName = opt.OutputFile + ".vid.mp4";
                    }
                    else
                    {
                        inSocket.Pins[i].Connection = PinConnection.Disabled;
                        continue;
                    }

                    MediaSocket outSocket = new MediaSocket();
                    outSocket.Pins.Add(inSocket.Pins[i]);
                    DeleteFile(fileName);
                    outSocket.File = fileName;

                    transcoder.Outputs.Add(outSocket);

                    Console.WriteLine("Output file: {0}", fileName);
                }

                return transcoder;
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
                Console.WriteLine("{0}, facility:{1} code:{2} hint:{3}", e.Message ?? "", e.Facility, e.Code, e.Hint ?? "");
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
