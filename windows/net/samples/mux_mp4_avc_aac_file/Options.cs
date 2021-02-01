/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
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

/*
command line options:

	mux_mp4_avc_aac_file --input audio.mp4 video.mp4 --output output.mp4 [--fast-start]
*/

namespace MuxMp4AvcAacFileSample
{
    class Options
    {
        // mux_mp4_avc_aac_file options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [OptionArray('i', "input", HelpText = "input file (aac, h264, mp4)")]
        public string[] InputFiles { get; set; }

        [Option('o', "output", HelpText = "output file")]
        public string OutputFile { get; set; }

        [Option('f', "fast-start", HelpText = "mp4 fast start")]
        public bool FastStart { get; set; }

        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("Usage: mux_mp4_avc_aac_file --input audio.mp4 video.mp4 --output output.mp4 [--fast-start]");
            Console.WriteLine(GetUsage());
        }

        void ResetOptions()
        {
            Help = false;
            Error = false;
            InputFiles = null;
            OutputFile = null;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            string inputFile = Path.Combine(exeDir, "..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v");

            InputFiles = new string[] { inputFile };
            OutputFile = Path.Combine(exeDir, "mux_mp4_avc_aac_file.mp4");

            Console.WriteLine("Using default options: --input " + inputFile + " --output " + OutputFile);
        }

        public bool Prepare(string[] args)
        {
            ResetOptions();

            if (args.Length == 0)
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

            Console.WriteLine("Input files: ");
            if ((InputFiles != null) && (InputFiles.Length > 0))
            {
                foreach (string s in InputFiles)
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

            Console.WriteLine("Fast-start: " + (FastStart ? "yes" : "no"));

            return res;
        }
    }
}
