/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using PrimoSoftware.AVBlocks;

namespace ReadMetaInfoAnyfileSample
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

            bool metaInfoResult = PrintMetadata(opt.InputFile);

            Library.Shutdown();

            return metaInfoResult ? (int)ExitCodes.Success : (int)ExitCodes.MetainfoError;
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            MetainfoError = 2,
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

        static bool PrintMetadata(string inputFile)
        {
            using (MediaInfo info = new MediaInfo())
            {
                info.Inputs[0].File = inputFile;

                if (!info.Open())
                {
                    PrintError("MediaInfo Open", info.Error);
                    return false;
                }

                Metadata meta = null;

                if (info.Outputs.Count > 0)
                    meta = info.Outputs[0].Metadata;

                if (meta == null)
                {
                    Console.WriteLine("Could not find any metadata.");
                    return true;
                }

                PrintMetaAttributes(meta);

                SavePictures(meta, info.Inputs[0].File);
            }

            return true;
        }

        static void PrintMetaAttributes(Metadata meta)
        {
            Console.WriteLine("Metadata\r\n--------");

            Console.WriteLine("{0} attributes:", meta.Attributes.Count);

            foreach (var attrib in meta.Attributes)
            {
                Console.WriteLine("{0,-15}: {1}", attrib.Name, attrib.Value);
            }
            Console.WriteLine();

            Console.WriteLine("{0} pictures:", meta.Pictures.Count);

            int i = 1;
            foreach (var pic in meta.Pictures)
            {
                Console.WriteLine("#{0} {1}, {2} bytes, {3}, {4}",
                                   i++, pic.MimeType, pic.Bytes.Length, pic.PictureType, pic.Description);
            }

            Console.WriteLine();
        }

        static void SavePictures(Metadata meta, string inputFile)
        {
            if (meta == null || meta.Pictures.Count == 0)
                return;

            int i = 1;
            foreach (var pic in meta.Pictures)
            {
                string picName = inputFile + ".pic" + i;
                ++i;
                SavePicture(pic, picName);
            }
        }

        static void SavePicture(MetaPicture pic, string baseFilename)
        {
            string filename;

            if (pic.MimeType == MimeType.Jpeg)
            {
                filename = baseFilename + ".jpg";
            }
            else if (pic.MimeType == MimeType.Png)
            {
                filename = baseFilename + ".png";
            }
            else if (pic.MimeType == MimeType.Gif)
            {
                filename = baseFilename + ".gif";
            }
            else if (pic.MimeType == MimeType.Tiff)
            {
                filename = baseFilename + ".tiff";
            }
            else
            {
                // unexpected picture mime type
                return;
            }

            System.IO.File.WriteAllBytes(filename, pic.Bytes);
        }

    }
}
