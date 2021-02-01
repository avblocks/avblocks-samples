/*
 *  Copyright (c) mux_mp4_file Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

using System;
using System.IO;
using System.Collections.Generic;
using CommandLine;
using CommandLine.Text;
using PrimoSoftware.AVBlocks;

namespace MuxMp4FileSample
{
    class Options
    {
        // mux_mp4_file options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [OptionArray('a', "audio", HelpText = "input AAC files. Can be used multiple times")]
        public string[] AudioFiles { get; set; }

        [OptionArray('v', "video", HelpText = "input H264 files. Can be used multiple times")]
        public string[] VideoFiles { get; set; }

        [Option('o', "output", HelpText = "output file")]
        public string OutputFile { get; set; }
        
        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("Usage: mux_mp4_file --audio <input_AAC> --video <input_AVC> --output <output.mp4>");
            Console.WriteLine(GetUsage());
        }

        void ResetOptions()
        {
            Help = false;
            Error = false;
            AudioFiles = null;
            VideoFiles = null;
            OutputFile = null;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            string videoFile = Path.Combine(exeDir, "..\\assets\\mov\\big_buck_bunny_trailer_iphone.vid.mp4");
            string audioFile = Path.Combine(exeDir, "..\\assets\\aud\\big_buck_bunny_trailer_iphone.aud.mp4");

            VideoFiles = new string[] { videoFile };
            AudioFiles = new string[] { audioFile };
            OutputFile = Path.Combine(exeDir, "mux_mp4_file.mp4");

            Console.WriteLine("Using default options:\n --audio " + AudioFiles[0] + " --video " + VideoFiles[0] + " --output " + OutputFile);
        }

        public bool Prepare(string[] args)
        {
            ResetOptions();

            if (args.Length < 3)
            {
                SetDefaultOptions();
            }
            else
            {
                if (!CommandLine.Parser.Default.ParseArguments(args, this))
                {
                    Console.WriteLine("Syntax error");
                    PrintUsage();
                    Error = true;
                    return false;
                }
            }

            if (Help)
            {
                PrintUsage();
                return false;
            }

            if (!Validate())
            {
                Console.WriteLine("\nRequired options are not set!");
                PrintUsage();
                Error = true;
                return false;
            }

            return true;
        }

        bool Validate()
        {
            bool res = true;

            Console.WriteLine("Audio files: ");
            if ((AudioFiles != null) && (AudioFiles.Length > 0))
            {
                foreach (string s in AudioFiles)
                {
                    Console.WriteLine("   " + s);
                }
            }
            else
            {
                Console.WriteLine("[not set]");
                res = false;
            }

            Console.WriteLine("Video files: ");
            if ((VideoFiles != null) && (VideoFiles.Length > 0))
            {
                foreach (string s in VideoFiles)
                {
                    Console.WriteLine("   " + s);
                }
            }
            else
            {
                Console.WriteLine("[not set]");
                res = false;
            }

            Console.Write("Output file: ");
            if (OutputFile == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(OutputFile);
            }

            return res;
        }
    }
}
