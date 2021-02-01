/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.IO;
using CommandLine;
using CommandLine.Text;
using PrimoSoftware.AVBlocks;

namespace ReEncodeSample
{
    class Options
    {
        // ReEncodeSample command line options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input file")]
        public string InputFile { get; set; }

        [Option('o', "output", HelpText = "output file")]
        public string OutputFile { get; set; }

        [Option("reEncodeAudio", HelpText = "Re-encode audio forced, yes|no")]
        public string ReEncodeAudioStrValue { get; set; }

        [Option("reEncodeVideo", HelpText = "Re-encode video forced, yes|no")]
        public string ReEncodeVideoStrValue { get; set; }

        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        public bool ReEncodeVideo { get { return IsYesNoOptionEnabled(ReEncodeVideoStrValue); } }
        public bool ReEncodeAudio { get { return IsYesNoOptionEnabled(ReEncodeAudioStrValue); } }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        bool IsYesNoOptionEnabled(string val)
        {
            if (string.IsNullOrEmpty(val))
                return true;

            val = val.ToLowerInvariant();
            return (val == "yes") || (val == "y");
        }

        void PrintUsage()
        {
            Console.WriteLine("Usage: Usage: ReEncode [--input inputFile.mp4] [--output outputFile.mp4] [--reEncodeAudio yes|no] [--reEncodeVideo yes|no]");
            Console.WriteLine(GetUsage());
        }

        void ResetOptions()
        {
            Help = false;
            Error = false;
            InputFile = null;
            OutputFile = null;
            ReEncodeAudioStrValue = null;
            ReEncodeVideoStrValue = null;

        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

            InputFile = Path.Combine(exeDir, "..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v");
            OutputFile = Path.Combine(exeDir, "reencode_output.mp4");

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
            if (InputFile == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(InputFile);
            }

            Console.Write("Output file: ");
            if (InputFile == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(OutputFile);
            }

            Console.WriteLine("Re-encode audio forced: " + (ReEncodeAudio ? "yes" : "no"));
            Console.WriteLine("Re-encode video forced: " + (ReEncodeVideo ? "yes" : "no"));

            return res;
        }
    }
}
