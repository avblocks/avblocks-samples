/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using CommandLine;
using CommandLine.Text;
using PrimoSoftware.AVBlocks;

namespace EncMpgDvdSample
{
    class Options
    {
        // enc_mpg_dvd command line options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input video file")]
        public string InputFile { get; set; }

        [Option('t', "split-time", HelpText = "split time (seconds)")]
        public int SplitTime { get; set; }

        [Option('s', "split-size", HelpText = "split size (MB)")]
        public Int64 SplitSize { get; set; }

        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        public string ExeDir
        {
            get
            {
                return Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            }
        }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("\nenc_mpg_dvd --input <avfile> [--split-time <seconds>] [--split-size <MBs>]");
            Console.WriteLine(GetUsage());
        }

        void ResetOptions()
        {
            Help = false;
            InputFile = null;
            SplitTime = 0;
            SplitSize = 0L;
            Error = false;
        }

        void SetDefaultOptions()
        {
            string exeDir = ExeDir;
            InputFile = Path.Combine(exeDir, "..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v");
            SplitSize = 3000000; // in bytes

            Console.WriteLine("Using default options: ");
            Console.Write    (" --input " + InputFile);
            Console.Write    (" --split-size " + SplitSize / 1000000);
            Console.WriteLine();
        }

        public bool Prepare(string[] args)
        {
            ResetOptions();

            if (args.Length == 0)
            {
                SetDefaultOptions();
                return true;
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
                PrintUsage();
                Error = true;
                return false;
            }

            return true;
        }

        bool Validate()
        {
            Console.Write("Input file: ");
            if (InputFile == null)
            {
                Console.WriteLine("[not set]");
                return false;
            }
            else
            {
                Console.WriteLine(InputFile);
            }

            if (SplitSize > 0)
            {
                Console.WriteLine("split-size: " + SplitSize);
                SplitSize *= 1000000;
            }
            if (SplitTime > 0.0)
            {
                Console.WriteLine("split-time: " + SplitTime);
            }

            return true;
        }
    }
}
