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

namespace EncMp4AvcAacPushSample
{
    class Program
    {
        static int Main(string[] args)
        {
            Library.Initialize();

            // Set license information. To run AVBlocks in demo mode, comment the next line out
            // Library.SetLicense("<license-string>");

            string exeDir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            string videoInputFile = Path.Combine(exeDir, "..\\assets\\vid\\avsync_240x180_29.97fps.yuv");
            string audioInputFile = Path.Combine(exeDir, "..\\assets\\aud\\avsync_44100_s16_2ch.pcm");

            // Video Input
            var vinput = new MediaSocket();
            vinput.Pins.Add(new MediaPin()
            {
                StreamInfo = new VideoStreamInfo
                {
                    FrameRate = 29.97,
                    FrameWidth = 240,
                    FrameHeight = 180,
                    ColorFormat = ColorFormat.YUV420,
                    ScanType = ScanType.Progressive,
                    StreamType = StreamType.UncompressedVideo
                }
            });

            // Audio Input
            var ainput = new MediaSocket();
            ainput.Pins.Add(new MediaPin()
            {
                StreamInfo = new AudioStreamInfo
                {
                    StreamType = StreamType.LPCM,
                    BitsPerSample = 16,
                    Channels = 2,
                    SampleRate = 44100,
                    BytesPerFrame = 4,
                }
            });


            // Output
            var output = new MediaSocket()
            {
                StreamType = StreamType.Mp4,
                File = "avsync.mp4"
            };

            // Video Pin
            output.Pins.Add(new MediaPin()
            {
                StreamInfo = new VideoStreamInfo()
                {
                    // keep input frame rate
                    Bitrate = 4 * 1000 * 1000,
                    StreamType = StreamType.H264,
                    StreamSubType = StreamSubType.Avc1,
                }
            });

            // Audio Pin
            output.Pins.Add(new MediaPin()
            {
                StreamInfo = new AudioStreamInfo()
                {
                    StreamType = StreamType.Aac,
                    SampleRate = 48000,
                    Bitrate = 128000,
                }
            });

            try { File.Delete(output.File); }
            catch (Exception) { }

            bool encodeResult = AVEncode.Run(vinput, videoInputFile, ainput, audioInputFile, output);

            Library.Shutdown();

            return encodeResult ? (int)ExitCodes.Success : (int)ExitCodes.EncodeError;
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            EncodeError = 2,
        }
    }
}
