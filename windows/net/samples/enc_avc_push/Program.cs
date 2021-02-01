/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;

namespace EncAvcPushSample
{
    class Program
    {
        static int Main(string[] args)
        {
            var opt = new Options();

            if (!opt.Prepare(args))
                return (int)ExitCodes.OptionsError;

            Library.Initialize();

            MediaSocket inSocket = CreateInputSocket(opt);
            MediaSocket outSocket = CreateOutputSocket(opt);

            int exitCode = (int)ExitCodes.Success;

            // Set license information. To run AVBlocks in demo mode, comment the next line out
            // Library.SetLicense("<license-string>");
            using (Transcoder transcoder = new Transcoder())
            {
                transcoder.AllowDemoMode = true;
                transcoder.Inputs.Add(inSocket);
                transcoder.Outputs.Add(outSocket);

                DeleteFile(opt.OutputFile);

                if (transcoder.Open())
                {
                    PrintStatus("Transcoder open", transcoder.Error);
                    if (!EncodeH264Stream(opt, transcoder))
                        exitCode = (int)ExitCodes.EncodingError;

                    transcoder.Close();
                    PrintStatus("Transcoder close", transcoder.Error);
                }
                else
                {
                    PrintStatus("Transcoder open", transcoder.Error);
                    exitCode = (int)ExitCodes.EncodingError;
                }
            }

            Library.Shutdown();

            return exitCode;
        }

        static bool EncodeH264Stream(Options opt, Transcoder transcoder)
        {
            bool success = false;

            try
            {
                using (var file = System.IO.File.OpenRead(opt.InputFile))
                {
                    int videoBufferSize = MediaSample.VideoBufferSizeInBytes(opt.Width, opt.Height, opt.Color.Id);

                    if (videoBufferSize <= 0)
                        return false;

                    MediaSample mediaSample = new MediaSample();
                    MediaBuffer mediaBuffer = new MediaBuffer(videoBufferSize);
                    mediaSample.Buffer = mediaBuffer;

                    int readBytes;

                    while (true)
                    {
                        mediaBuffer.SetData(0, videoBufferSize);
                        readBytes = file.Read(mediaBuffer.Start, 0, mediaBuffer.DataSize);
                        if (readBytes == videoBufferSize)
                        {
                            mediaBuffer.SetData(0, readBytes);

                            if (!transcoder.Push(0, mediaSample))
                            {
                                PrintStatus("Transcoder push", transcoder.Error);
                                success = false;
                                break;
                            }

                            success = true;
                        }
                        else
                        {
                            if (!transcoder.Flush())
                                success = false;

                            PrintStatus("Transcoder flush", transcoder.Error);

                            break;
                        }
                    }
                }
            }
            catch (System.IO.DirectoryNotFoundException dnfe)
            {
                Console.WriteLine(dnfe);
                success = false;
            }

            return success;
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
            socket.StreamType = StreamType.UncompressedVideo;
            socket.File = null;
            socket.Stream = null;

            MediaPin pin = new MediaPin();
            socket.Pins.Add(pin);
            VideoStreamInfo vsi = new VideoStreamInfo();
            pin.StreamInfo = vsi;

            vsi.StreamType = StreamType.UncompressedVideo;
            vsi.ScanType = ScanType.Progressive;

            vsi.FrameWidth = opt.Width;
            vsi.FrameHeight = opt.Height;
            vsi.ColorFormat = opt.Color.Id;
            vsi.FrameRate = opt.Fps;

            return socket;
        }

        static MediaSocket CreateOutputSocket(Options opt)
        {
            MediaSocket socket = new MediaSocket();
            socket.File = opt.OutputFile;
            socket.StreamType = StreamType.H264;
            socket.StreamSubType = StreamSubType.AvcAnnexB;

            MediaPin pin = new MediaPin();
            socket.Pins.Add(pin);
            VideoStreamInfo vsi = new VideoStreamInfo();
            pin.StreamInfo = vsi;

            vsi.StreamType = StreamType.H264;
            vsi.StreamSubType = StreamSubType.AvcAnnexB;
            
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
