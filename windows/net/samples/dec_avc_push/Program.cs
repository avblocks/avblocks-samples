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

namespace DecAvcPushSample
{
    class Program
    {
        static int Main(string[] args)
        {
            var opt = new Options();

            if (!opt.Prepare(args))
                return (int)ExitCodes.OptionsError;

            Library.Initialize();

            // Set license information. To run AVBlocks in demo mode, comment the next line out
            // Library.SetLicense("<license-string>");

            bool transcoderResult = Transcode(opt);
   
            Library.Shutdown();

            return transcoderResult ? 0 : 1;
        }

        static bool Transcode(Options opt)
        {
            using (Transcoder transcoder = new Transcoder())
            {
                transcoder.AllowDemoMode = true;

                MediaSocket inSocket = CreateInputSocket(opt);
                MediaSocket outSocket = CreateOutputSocket(opt);

                transcoder.Inputs.Add(inSocket);
                transcoder.Outputs.Add(outSocket);

                DeleteFile(opt.OutputFile);

                if (!transcoder.Open())
                {
                    PrintStatus("Transcoder open", transcoder.Error);
                    return false;
                }

                for (int fileCount = 0; ; fileCount++)
                {
                    string pattern = "au_{0:0000}.h264";
                    string fileName = string.Format(pattern, fileCount);
                    string filePath = Path.Combine(opt.InputDir, fileName);

                    if (!File.Exists(filePath))
                    {
                        fileName = string.Format(pattern, fileCount - 1);
                        Console.WriteLine("Decoded " + fileCount + " files." + "(last decoded file: " + fileName + ")");
                        break;
                    }

                    var sample = new MediaSample();
                    sample.Buffer = new MediaBuffer(File.ReadAllBytes(filePath));

                    if (!transcoder.Push(0, sample))
                    {
                        PrintStatus("Transcoder push", transcoder.Error);
                        return false;
                    }               
                }

                if (!transcoder.Flush())
                {
                    PrintStatus("Transcoder flush", transcoder.Error);
                    return false;
                }

                Console.WriteLine("Output file: " + opt.OutputFile);

                transcoder.Close();

                return true;
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

        static MediaSocket CreateInputSocket(Options opt)
        {
            MediaSocket socket = new MediaSocket();
            socket.StreamType = StreamType.Avc;

            MediaPin pin = new MediaPin();
            socket.Pins.Add(pin);

            VideoStreamInfo vsi = new VideoStreamInfo();
            pin.StreamInfo = vsi;

            vsi.StreamType = StreamType.Avc;
            vsi.ScanType = ScanType.Progressive;

            return socket;
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
            vsi.ScanType = ScanType.Progressive;
            vsi.ColorFormat = ColorFormat.YUV420;
            
            return socket;
        }

        static void PrintStatus(string action, ErrorInfo e)
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
            EncodingError = 2,
        }
    }
}
