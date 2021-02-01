/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.IO;
using PrimoSoftware.AVBlocks;

namespace DumpAvcAu
{
    class Program
    {
        /*
        Access Unit: one picture (parameters + all slices)
        The elementary stream may contain optional access unit delimiters (AUD).
        A picture may consist of one or more slices.

        */

        // Network Abstraction Layer Unit Definitions per H.264/AVC spec
        enum NALUType
        {
            UNSPEC = 0,    // Unspecified
            SLICE = 1,     // Coded slice of a non-IDR picture
            DPA = 2,       // Coded slice data partition A
            DPB = 3,       // Coded slice data partition B
            DPC = 4,       // Coded slice data partition C
            IDR = 5,       // Coded slice of an IDR picture
            SEI = 6,       // Supplemental enhancement information
            SPS = 7,       // Sequence parameter set
            PPS = 8,       // Picture parameter set
            AUD = 9,       // Access unit delimiter
            EOSEQ = 10,    // End of sequence                                               
            EOSTREAM = 11, // End of stream
            FILL = 12      // Filler data
        };

        enum NALUPriority
        {
            DISPOSABLE = 0,
            LOW = 1,
            HIGH = 2,
            HIGHEST = 3,
        };

        static NALUType nal_unit_type(byte b)
        {
            return (NALUType)(b & 0x1f);
        }

        static NALUPriority nal_unit_ref_idc(byte b)
        {
            return (NALUPriority)((b & 0x60) >> 5);
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

        static void PrintNaluHeader(byte data)
        {
            Console.Write("".PadLeft(8) +
                          nal_unit_type(data) + ": " +
                          nal_unit_ref_idc(data));

            Console.WriteLine();
        }

        static void PrintNalus(MediaBuffer buffer)
        {
            // This parsing code assumes that MediaBuffer contains 
            // a single Access Unit of one or more complete NAL Units
            while (buffer.DataSize > 1)
            {
                int dataOffset = buffer.DataOffset;
                int dataSize = buffer.DataSize;

                // is this a NALU with a 3 byte start code prefix
                if (dataSize >= 3 &&
                    0x00 == buffer.Start[dataOffset + 0] &&
                    0x00 == buffer.Start[dataOffset + 1] &&
                    0x01 == buffer.Start[dataOffset + 2])
                {
                    PrintNaluHeader(buffer.Start[dataOffset + 3]);

                    // advance in the buffer
                    buffer.SetData(dataOffset + 3, dataSize - 3);
                }
                // OR is this a NALU with a 4 byte start code prefix
                else if (dataSize >= 4 &&
                         0x00 == buffer.Start[dataOffset + 0] &&
                         0x00 == buffer.Start[dataOffset + 1] &&
                         0x00 == buffer.Start[dataOffset + 2] &&
                         0x01 == buffer.Start[dataOffset + 3])
                {
                    PrintNaluHeader(buffer.Start[dataOffset + 4]);

                    // advance in the buffer
                    buffer.SetData(dataOffset + 4, dataSize - 4);
                }
                else
                {
                    // advance in the buffer
                    buffer.SetData(dataOffset + 1, dataSize - 1);
                }

                // NOTE: Some NALUs may have a trailing zero byte. The `while` 
                // condition `buffer->dataSize() > 1` will effectively 
                // skip the trailing zero byte.
            }
        }

        static void WriteAuFile(string outputDir, int au_index, MediaBuffer buffer)
        {
            string filePath = outputDir + "/au_" +
                              String.Format("{0:0000}", au_index) + ".h264";

            using (BinaryWriter file = new BinaryWriter(File.Open(filePath, FileMode.Append)))
            {
                file.BaseStream.Write(buffer.Start, buffer.DataOffset, buffer.DataSize);
            }
        }

        static int au_index = 0;
        static bool parse_h264_stream(Options opt)
        {
            DeleteDirectory(opt.OutputDir);

            MediaSocket inSocket = new MediaSocket();
            inSocket.File = opt.InputFile;
            inSocket.StreamType = StreamType.H264;

            MediaSocket outSocket = CreateOutputSocket(opt);

            using (Transcoder transcoder = new Transcoder())
            {
                transcoder.Inputs.Add(inSocket);
                transcoder.Outputs.Add(outSocket);

                var res = transcoder.Open();
                PrintError("transcoder open", transcoder.Error);

                if (!res)
                    return false;

                int inputIndex = 0;
                MediaSample accessUnit = new MediaSample();

                if (!MakeDir(opt.OutputDir))
                {
                    Console.WriteLine("cannot create output directory: " + opt.OutputDir);
                    return false;
                }

                while (transcoder.Pull(out inputIndex, accessUnit))
                {
                    // Each call to Transcoder::pull returns one Access Unit. 
                    // The Access Unit may contain one or more NAL units.
                    var au_buffer = accessUnit.Buffer;
                    Console.WriteLine("AU #" + au_index + ", " + au_buffer.DataSize + " bytes");
                    WriteAuFile(opt.OutputDir, au_index, au_buffer);
                    PrintNalus(au_buffer);
                    ++au_index;
                }
            }

            return true;
        }

        static void DeleteDirectory(string OutputDir)
        {
            try
            {
                if (Directory.Exists(OutputDir))
                    Directory.Delete(OutputDir, true);
            }
            catch { }
        }

        static MediaSocket CreateOutputSocket(Options opt)
        {
            MediaPin pin = new MediaPin();

            MediaSocket socket = new MediaSocket();
            socket.Pins.Add(pin);

            VideoStreamInfo vsi = new VideoStreamInfo();
            vsi.ScanType = ScanType.Progressive;

            pin.StreamInfo = vsi;

            return socket;
        }

        static bool MakeDir(string dir)
        {
            try
            {
                if (!System.IO.Directory.Exists(dir))
                {
                    System.IO.Directory.CreateDirectory(dir);
                }

                return true;
            }
            catch { }

            return false;
        }

        static int Main(string[] args)
        {
            var opt = new Options();

            if (!opt.Prepare(args))
                return opt.Error ? (int)ExitCodes.OptionsError : (int)ExitCodes.Success;

            Library.Initialize();

            // set your license string
            // Library::setLicense("avblocks-license-xml");

            bool parseResult = parse_h264_stream(opt);

            Library.Shutdown();

            if (!parseResult)
                return 1;

            Console.WriteLine("Successfully parsed input file: " + opt.InputFile);
            Console.WriteLine("Output directory: " + opt.OutputDir);

            return 0;
        }

    }
}
