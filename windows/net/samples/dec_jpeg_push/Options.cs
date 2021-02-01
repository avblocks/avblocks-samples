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

/*
command line options:

long format:
	dec_jpeg_push --input input.jpeg --output output.yuv

short format:
	dec_jpeg_push -i input.jpeg -o output.yuv
*/

namespace DecodeJpegPushSample
{
    class Options
    {
        // StreamDecoder options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input file")]
        public string InputFile { get; set; }

        [Option('o', "output", HelpText = "output file")]
        public string OutputFile { get; set; }

        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

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

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("\nUsage: dec_jpeg_push --input <file>.jpeg --output <file>.yuv");
            Console.WriteLine(GetUsage());
        }

        void ResetOptions()
        {
            Help = false;
            InputFile = null;
            OutputFile = null;
            Error = false;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

            InputFile = Path.Combine(exeDir, "..\\assets\\img\\cube0000.jpeg");
            OutputFile = "cube0000.yuv";

            Console.WriteLine("Using default options: --input " + InputFile + " --output " + OutputFile);
        }

        bool Validate()
        {
            bool res = true;

            Console.Write("Input file: ");
            if (string.IsNullOrEmpty(InputFile))
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(InputFile);
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

            return res;
        }
    }
}
