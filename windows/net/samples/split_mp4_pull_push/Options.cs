/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
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

/*
command line options:

short format:
long format:
	split_mp4_pull_push.exe <inputFile>
*/

namespace SplitMp4PullPushSample
{
    class Options
    {
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input file")]
        public string InputFile { get; set; }

        [ValueList(typeof(List<string>), MaximumElements = 1)]
        public IList<string> NotCapturedItems { get; set; }


        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("\nUsage: split_mp4_pull_push.exe <inputFile>");
            Console.WriteLine(GetUsage());
        }

        void ResetOptions()
        {
            Help = false;
            InputFile = null;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            InputFile = Path.Combine(exeDir, "..\\assets\\mov\\avsync_2min.mp4");

            Console.WriteLine("Using default input file: " + InputFile );
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

                if ((NotCapturedItems.Count > 0) && string.IsNullOrEmpty(InputFile))
                    InputFile = NotCapturedItems[0];
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

            return res;
        }
    }
}
