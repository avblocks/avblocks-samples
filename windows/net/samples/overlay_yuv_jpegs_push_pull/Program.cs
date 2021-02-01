/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source tree.  
*/
using System;
using System.IO;
using System.Reflection;
using PrimoSoftware.AVBlocks;

namespace overlay_yuv_jpegs_sample
{
    class Overlay : IDisposable
    {
        private Transcoder t = new Transcoder();
        public ErrorInfo Error 
        { 
            get { return t.Error; }
        }
        public void Close()
        {
            t.Close();
            t.Inputs.Clear();
            t.Outputs.Clear();
        }

        public bool Open(string imageOverlay, StreamType imageType, VideoStreamInfo uncompressedVideo)
        {
            var inSocket = new MediaSocket()
            {
                File = null, // Push
                StreamType = StreamType.UncompressedVideo,
            };

            inSocket.Pins.Add(new MediaPin()
            {
                StreamInfo = uncompressedVideo
            });
            t.Inputs.Add(inSocket);

            var outSocket = new MediaSocket()
            {
                File = null, // Pull
                StreamType = StreamType.UncompressedVideo
            };

            outSocket.Pins.Add(new MediaPin()
            {
                StreamInfo = uncompressedVideo
            });

            SetOverlayParamsToPin(outSocket.Pins[0], imageOverlay, imageType);

            t.Outputs.Add(outSocket);

            return t.Open();
        }
        public bool Pull(out int outputIndex, MediaSample outputSample)
        {
            return t.Pull(out outputIndex, outputSample);
        }
        public bool Push(int inputIndex, MediaSample inputSample)
        {
            return t.Push(inputIndex, inputSample);
        }
        public void Dispose()
        {
            t.Dispose();
        }

        private void SetOverlayParamsToPin(MediaPin pin, string imageOverlay, StreamType imageType)
        {
            var videoInfo = new VideoStreamInfo()
            {
                StreamType = imageType
            };

            pin.Params[Param.Video.Overlay.Mode] = AlphaCompositingMode.Atop;
            pin.Params[Param.Video.Overlay.LocationX] = 0; // left
            pin.Params[Param.Video.Overlay.LocationY] = 0; // top
            pin.Params[Param.Video.Overlay.BackgroundAlpha] = 1.0;
            pin.Params[Param.Video.Overlay.ForegroundBuffer] = new MediaBuffer(File.ReadAllBytes(imageOverlay));
            pin.Params[Param.Video.Overlay.ForegroundBufferFormat] = videoInfo;
            pin.Params[Param.Video.Overlay.ForegroundAlpha] = 1.0;
        }
    }

    enum ExitCodes : int
    {
        Success = 0,
        Error = 1,
        EncoderError = 2,
        DecoderError = 3,
    }

    class Program
    {
        static int Main(string[] args)
        {
            Library.Initialize();

            // Set license information. To run AVBlocks in demo mode, comment the next line out
            // Library.SetLicense("<license-string>");

            string inputFile = Path.Combine(ExeDir, @"..\assets\mov\big_buck_bunny_trailer_iphone.m4v");
            string outputFile = "overlay_yuv_jpegs.mp4";
            const int imageOverlayFrames = 250;
            string imageOverlayFiles = Path.Combine(ExeDir, @"..\assets\overlay\cube\cube{0:d4} (128x96).jpg");

            VideoStreamInfo uncompressedVideo = null;

            using (var info = new MediaInfo())
            {
                info.Inputs[0].File = inputFile;

                if (!info.Open())
                {
                    PrintError("load info", info.Error);
                    return (int)ExitCodes.Error;
                }

                foreach (var socket in info.Outputs)
                {
                    foreach (var pin in socket.Pins)
                    {
                        StreamInfo si = pin.StreamInfo;
                        if (si.MediaType == MediaType.Video)
                        {
                            uncompressedVideo = si.Clone() as VideoStreamInfo;
                            break;
                        }
                    }
                }
            }

            uncompressedVideo.StreamType = StreamType.UncompressedVideo;
            uncompressedVideo.ColorFormat = ColorFormat.YUV420;

            var outputVideo = (VideoStreamInfo)uncompressedVideo.Clone();
            outputVideo.StreamType = StreamType.H264;

            try { System.IO.File.Delete(outputFile); }
            catch { }

            int decodedSamples = 0;
            int overlayedSamples = 0;
            int encodedSamples = 0;

            using (var overlay = new Overlay())
            using (var decoder = CreateDecoder(inputFile))
            using (var encoder = CreateEncoder(uncompressedVideo, outputVideo, outputFile))
            {
                if (!decoder.Open())
                {
                    PrintError("decoder open", decoder.Error);
                    return (int)ExitCodes.DecoderError;
                }

                if (!encoder.Open())
                {
                    PrintError("encoder open", encoder.Error);
                    return (int)ExitCodes.EncoderError;
                }

                int outputIndex;
                var decodedSample = new MediaSample();
                var overlayedSample = new MediaSample();

                while (true)
                {
                    if (!decoder.Pull(out outputIndex, decodedSample))
                    {
                        PrintError("decoder pull", decoder.Error);
                        break;
                    }
                    ++decodedSamples;

                    var imageOverlayFile = string.Format(imageOverlayFiles, overlayedSamples % imageOverlayFrames);

                    overlay.Close();
                    if (!overlay.Open(imageOverlayFile, StreamType.Jpeg, uncompressedVideo))
                    {
                        PrintError("overlay open", overlay.Error);
                        break;
                    }

                    if (!overlay.Push(0, decodedSample))
                    {
                        PrintError("overlay push", overlay.Error);
                        break;
                    }

                    if (!overlay.Pull(out outputIndex, overlayedSample))
                    {
                        PrintError("overlay pull", overlay.Error);
                        break;
                    }
                    ++overlayedSamples;


                    if (!encoder.Push(0, overlayedSample))
                    {
                        PrintError("encoder push", encoder.Error);
                        break;
                    }
                    ++encodedSamples;
                };

                decoder.Close();
                overlay.Close();
                encoder.Flush();
                encoder.Close();
            }

            Console.WriteLine("samples decoded/overlayed/encoded: {0}/{1}/{2}",
                               decodedSamples, overlayedSamples, encodedSamples);

            bool success = (decodedSamples > 0 && decodedSamples == encodedSamples);

            if (success)
                Console.WriteLine("output file: {0}", Path.GetFullPath(outputFile));

            Library.Shutdown();

            return success ? (int)ExitCodes.Success : (int)ExitCodes.Error;
        }

        static Transcoder CreateDecoder(string inputFile)
        {
            Transcoder transcoder = new Transcoder()
            {
                AllowDemoMode = true
            };

            var inSocket = new MediaSocket()
            {
                File = inputFile,
            };

            transcoder.Inputs.Add(inSocket);

            var outSocket = new MediaSocket()
            {
                File = null, // Pull
                StreamType = StreamType.UncompressedVideo
            };

            outSocket.Pins.Add(new MediaPin()
            {
                StreamInfo = new VideoStreamInfo()
                {
                    StreamType = StreamType.UncompressedVideo
                }
            });

            transcoder.Outputs.Add(outSocket);

            return transcoder;
        }

        static Transcoder CreateEncoder(VideoStreamInfo inputVideo, VideoStreamInfo outputVideo, string outputFile)
        {
            Transcoder transcoder = new Transcoder()
            {
                AllowDemoMode = true
            };


            var inSocket = new MediaSocket()
            {
                File = null, //Push
                StreamType = StreamType.UncompressedVideo,
            };

            inSocket.Pins.Add(new MediaPin()
            {
                StreamInfo = inputVideo
            });
            transcoder.Inputs.Add(inSocket);

            var outSocket = new MediaSocket()
            {
                File = outputFile,
            };

            outSocket.Pins.Add(new MediaPin()
            {
                StreamInfo = outputVideo
            });

            transcoder.Outputs.Add(outSocket);

            return transcoder;
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
                Console.WriteLine("{0}, facility:{1} code:{2} hint:{3}", e.Message , e.Facility, e.Code, e.Hint);
            }
        }

        static string exedir;
        static string ExeDir
        {
            get
            {
                if (exedir == null)
                    exedir = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

                return exedir;
            }
        }
    }
}
