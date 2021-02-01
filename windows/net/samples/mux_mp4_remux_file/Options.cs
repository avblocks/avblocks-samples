/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

using System;
using System.IO;
using CommandLine;
using CommandLine.Text;

/*
command line options:

	mux_mp4_remux_file --input audio.mp4 video.mp4 --output output.mp4 [--fast-start]
*/

namespace MuxMp4RemuxFileSample
{
    class Options
    {
        // mux_mp4_avc_aac_file options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input file (mp4)")]
        public string InputFile { get; set; }

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
            Console.WriteLine("Usage: mux_mp4_remux_file --input file.mp4 --output output.mp4 [--fast-start]");
            Console.WriteLine(GetUsage());
        }

        void ResetOptions()
        {
            Help = false;
            Error = false;
            InputFile = null;
            OutputFile = null;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

            InputFile = Path.Combine(exeDir, "..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v");
            OutputFile = Path.Combine(exeDir, "mux_mp4_remux_file.mp4");

            Console.WriteLine("Using default options: --input " + InputFile + " --output " + OutputFile);
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

            Console.Write("Input file: ");
            if (!string.IsNullOrEmpty(InputFile))
            {
                Console.WriteLine(InputFile);
            }
            else
            {
                Console.WriteLine("[not set]");
                res = false;
            }

            Console.Write("Output file: ");
            if (string.IsNullOrEmpty(OutputFile))
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
