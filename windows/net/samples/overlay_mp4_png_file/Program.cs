/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;

namespace OverlayMp4PngFileSample
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

            bool overlayed = ApplyOverlay(opt);

            Library.Shutdown();

            return overlayed ? (int)ExitCodes.Success : (int)ExitCodes.EncodingError;
        }

        static bool ApplyOverlay(Options opt)
        {
            DeleteFile(opt.OutputFile);

            // create Transcoder
            using (Transcoder transcoder = new Transcoder())
            {
                transcoder.AllowDemoMode = true;
                
                MediaSocket inputSocket = new MediaSocket();
                inputSocket.File = opt.InputFile;
                transcoder.Inputs.Add(inputSocket);

                MediaSocket outputSocket = ConfigureOutputSocket(opt);
                transcoder.Outputs.Add(outputSocket);

                bool res = transcoder.Open();
                
                if (!res)
                {
                    PrintError("Transcoder open", transcoder.Error);
                    return false;
                }

                res = transcoder.Run();
                
                if (!res)
                {
                    PrintError("Transcoder run", transcoder.Error);
                    return false;
                }

                transcoder.Close();
            }

            Console.WriteLine("Output: " + opt.OutputFile);

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

        static MediaSocket ConfigureOutputSocket(Options opt)
        {
            MediaSocket socket = new MediaSocket();
            socket.File = opt.OutputFile;

            MediaPin pin = new MediaPin();
            socket.Pins.Add(pin);
            VideoStreamInfo vsi = new VideoStreamInfo();
            pin.StreamInfo = vsi;

            VideoStreamInfo overlayVsi = new VideoStreamInfo();
            overlayVsi.StreamType = StreamType.Png;

            pin.Params.Add(Param.Video.Overlay.Mode, AlphaCompositingMode.Atop);
            pin.Params.Add(Param.Video.Overlay.LocationX, opt.PositionX);
            pin.Params.Add(Param.Video.Overlay.LocationY, opt.PositionY);
            pin.Params.Add(Param.Video.Overlay.BackgroundAlpha, 1);
            pin.Params.Add(Param.Video.Overlay.ForegroundAlpha, opt.Alpha);

            pin.Params.Add(Param.Video.Overlay.ForegroundBufferFormat, overlayVsi);
            pin.Params.Add(Param.Video.Overlay.ForegroundBuffer, new MediaBuffer(System.IO.File.ReadAllBytes(opt.Watermark)));

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
                Console.WriteLine("{0}, facility:{1} code:{2} hint:{3}", e.Message, e.Facility, e.Code, e.Hint);
            }
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            EncodingError = 2,
        }
    }
}
