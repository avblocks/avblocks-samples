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

namespace EncAacAdtsPullSample
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

            bool result = Encode(opt);

            Library.Shutdown();

            return result ? (int)ExitCodes.Success : (int)ExitCodes.EncodingError;
        }

        static bool Encode(Options opt)
        {
            DeleteFile(opt.OutputFile);

            MediaSocket inSocket = new MediaSocket();
            inSocket.File = opt.InputFile;

            MediaSocket outSocket = CreateOutputSocket(opt);

            bool success = false;

            // create Transcoder
            using (Transcoder transcoder = new Transcoder())
            {
                transcoder.AllowDemoMode = true;
                transcoder.Inputs.Add(inSocket);
                transcoder.Outputs.Add(outSocket);

                if (!transcoder.Open())
                {
                    PrintError("Transcoder open", transcoder.Error);
                    return false;
                }

                using (FileStream outputFile = File.OpenWrite(opt.OutputFile))
                {
                    MediaSample outputSample = new MediaSample();
                    int outputIndex = 0;
                    
                    while (transcoder.Pull(out outputIndex, outputSample))
                    {
                        MediaBuffer buffer = outputSample.Buffer;
                        outputFile.Write(buffer.Start, buffer.DataOffset, buffer.DataSize);
                    }

                    ErrorInfo error = transcoder.Error;
                    PrintError("Transcoder pull", error);

                    if ((error.Code == (int)CodecError.EOS) &&
                                (error.Facility == ErrorFacility.Codec))
                    {
                        // ok
                        success = true;
                    }
                }

                transcoder.Close();
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

        static MediaSocket CreateOutputSocket(Options opt)
        {
            MediaSocket socket = new MediaSocket();

            socket.StreamType = StreamType.Aac;
            socket.StreamSubType = StreamSubType.AacAdts;

            MediaPin pin = new MediaPin();
            socket.Pins.Add(pin);
            AudioStreamInfo asi = new AudioStreamInfo();
            pin.StreamInfo = asi;

            asi.StreamType = StreamType.Aac;
            asi.StreamSubType = StreamSubType.AacAdts;

            // You can change the sampling rate and the number of the channels
            //asi.Channels = 1;
            //asi.SampleRate = 44100;

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
            EncodingError = 2,
        }
    }
}
