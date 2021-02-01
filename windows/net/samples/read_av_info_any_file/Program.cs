/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;

namespace ReadAVInfoAnyFileSample
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

            bool avInfoResult = PrintAVInfo(opt.InputFile);

            Library.Shutdown();

            return avInfoResult ? (int)ExitCodes.Success : (int)ExitCodes.AVinfoError;
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            AVinfoError = 2,
        }

        static bool PrintAVInfo(string inputFile)
        {
            using (MediaInfo info = new MediaInfo())
            {
                info.Inputs[0].File = inputFile;

                if (info.Open())
                {
                    PrintStreams(info);

                    info.Close();
                }
                else
                {
                    PrintError(info.Error);
                }
            }
            
            return true;
        }

        static void PrintStreams(MediaInfo mediaInfo)
        {
            foreach (var socket in mediaInfo.Outputs)
            {
                Console.WriteLine("container: {0}", socket.StreamType);
                Console.WriteLine("streams: {0}", socket.Pins.Count);

                for(int streamIndex = 0; streamIndex < socket.Pins.Count; streamIndex++)
                {
                    StreamInfo si = socket.Pins[streamIndex].StreamInfo;
                    Console.WriteLine();
                    Console.WriteLine("stream #{0} {1}", streamIndex, si.MediaType);
                    Console.WriteLine("type: {0}", si.StreamType);
                    Console.WriteLine("subtype: {0}", si.StreamSubType);
                    Console.WriteLine("id: {0}", si.ID);
                    Console.WriteLine("duration: {0:f3}", si.Duration);

                    if (MediaType.Video == si.MediaType)
                    {
                        VideoStreamInfo vsi = si as VideoStreamInfo;
                        PrintVideo(vsi);
                    }
                    else if (MediaType.Audio == si.MediaType)
                    {
                        AudioStreamInfo asi = si as AudioStreamInfo;
                        PrintAudio(asi);
                    }
                    else
                    {
                        Console.WriteLine();
                    }
                }

                Console.WriteLine();
            }
        }

        static void PrintVideo(VideoStreamInfo vsi)
        {
            Console.WriteLine("bitrate: {0} mode: {1}", vsi.Bitrate, vsi.BitrateMode);

            Console.WriteLine("color format: {0}", vsi.ColorFormat);

            Console.WriteLine("display ratio: {0}:{1}", vsi.DisplayRatioWidth, vsi.DisplayRatioHeight);

            Console.WriteLine("frame bottom up: {0}", vsi.FrameBottomUp);
            Console.WriteLine("frame size: {0}x{1}", vsi.FrameWidth, vsi.FrameHeight);
            Console.WriteLine("frame rate: {0:f3}", vsi.FrameRate);

            Console.WriteLine("scan type: {0}", vsi.ScanType);
        }

        static void PrintAudio(AudioStreamInfo asi)
        {
            Console.WriteLine("bitrate: {0} mode: {1}", asi.Bitrate, asi.BitrateMode);

            Console.WriteLine("bits per sample: {0}", asi.BitsPerSample);
            Console.WriteLine("bytes per frame: {0}", asi.BytesPerFrame);

            Console.WriteLine("channel layout: {0:X}", asi.ChannelLayout);
            Console.WriteLine("channels: {0}", asi.Channels);

            Console.WriteLine("flags: {0:X}", asi.PcmFlags);

            Console.WriteLine("sample rate: {0}", asi.SampleRate);
        }

        static void PrintError(ErrorInfo e)
        {
            if (ErrorFacility.Success == e.Facility)
            {
                Console.WriteLine("Success");
                return;
            }

            Console.WriteLine("{0}, facility:{1} code:{2} hint:{3}", 
                                    e.Message ?? "", e.Facility, e.Code, e.Hint ?? "");
        }
    }
}
