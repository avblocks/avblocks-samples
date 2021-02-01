/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;

namespace DecAvcFileSample
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

            bool decAvcFileResult = DecodeAvcFile(opt);

            Library.Shutdown();

            return decAvcFileResult ? (int)ExitCodes.Success : (int)ExitCodes.DecodingError;
        }

        static bool DecodeAvcFile(Options opt)
        {
            // transcoder will fail if output exists (by design)
            DeleteFile(opt.OutputFile);

            var mediaInfo = new PrimoSoftware.AVBlocks.MediaInfo();
            mediaInfo.Inputs[0].File = opt.InputFile;

            if(!mediaInfo.Open())
            {
                PrintError("MediaInfo.Open()", mediaInfo.Error);
                return false;
            }

            MediaSocket inSocket = MediaSocket.FromMediaInfo(mediaInfo);
            MediaSocket outSocket = CreateOutputSocket(opt);

            // create Transcoder
            using (Transcoder transcoder = new Transcoder())
            {
                transcoder.AllowDemoMode = true;
                transcoder.Inputs.Add(inSocket);
                transcoder.Outputs.Add(outSocket);

                bool res = transcoder.Open();
                PrintError("Transcoder open", transcoder.Error);
                if (!res)
                    return false;

                res = transcoder.Run();
                PrintError("Transcoder run", transcoder.Error);
                if (!res)
                    return false;

                transcoder.Close();
                PrintError("Transcoder close", transcoder.Error);
                if (!res)
                    return false;
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

        static MediaSocket CreateOutputSocket(Options opt)
        {
            MediaSocket socket = new MediaSocket();
            socket.File = opt.OutputFile;
            socket.StreamType = StreamType.UncompressedVideo;

            MediaPin pin = new MediaPin();
            socket.Pins.Add(pin);
            VideoStreamInfo vsi = new VideoStreamInfo();
            pin.StreamInfo = vsi;

            vsi.StreamType = StreamType.UncompressedVideo;
            vsi.ColorFormat = ColorFormat.YUV420;
            
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

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            DecodingError = 2,
        }
    }
}
