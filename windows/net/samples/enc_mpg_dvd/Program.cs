/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;

namespace EncMpgDvdSample
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

            bool encodeResult = EncodeMpgDvd(opt);

            Library.Shutdown();

            return encodeResult ? (int)ExitCodes.Success : (int)ExitCodes.EncodingError;
        }

        static bool EncodeMpgDvd(Options opt)
        {
            int splitCount = 0;
            double startTime = 0;

            string exeDir = opt.ExeDir;

            double minDuration = StreamDuration.GetMinDuration(opt.InputFile);

            while (true)
            {
                double processedTime = 0;
                Int64 processedSize = 0;
                bool isSplit = false;

                string outFile;
                outFile = String.Format("{0}\\enc_mpg_dvd.{1:000}.mpg", exeDir,++splitCount);

                Console.WriteLine("encoding " + outFile);

                Splitter split = new Splitter();

                bool res = split.TranscodeSplit(opt, outFile, PrimoSoftware.AVBlocks.Preset.Video.DVD.PAL_16x9_MP2, 
                                                   startTime, out processedTime, out processedSize, out isSplit);

                if (!res)
                    return false;

                if (!isSplit)
                    break;

                startTime += processedTime;

                if (startTime >= minDuration)
                    break;
            }

            return true;
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            EncodingError = 2,
        }
    }
}
