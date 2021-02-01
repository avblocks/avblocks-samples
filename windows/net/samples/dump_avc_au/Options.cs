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

namespace DumpAvcAu
{
    class Options
    {
        //dump_avc_au command line options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input file (AVC/H.264)")]
        public string InputFile { get; set; }

        [Option('o', "output", HelpText = "output directory")]
        public string OutputDir { get; set; }

        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("\ndump_avc_au--input < h264 - file > --output < folder > ");
            Console.WriteLine(GetUsage());
        }

        void ResetOptions()
        {
            Help = false;
            Error = false;
            InputFile = null;
            OutputDir = null;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

            InputFile = Path.Combine(exeDir, "..\\assets\\vid\\foreman_qcif.h264");
            OutputDir = Path.Combine(exeDir, "foreman_qcif.h264.au");

            Console.WriteLine("Using default options: ");
            Console.WriteLine("--input  " + InputFile);
            Console.WriteLine("--output " + OutputDir);
            Console.WriteLine();
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

            Console.Write("Output directory: ");
            if (OutputDir == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(OutputDir);
            }

            return res;
        }
    }
}
