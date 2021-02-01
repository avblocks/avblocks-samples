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

namespace DemuxMp4FileSample
{
    class Options
    {
        // demux_mp4_file command line options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input AVC file")]
        public string InputFile { get; set; }

        [Option('o', "output", HelpText = "output YUV file")]
        public string OutputFile { get; set; }

        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("\nUsage: demux_mp4_file -i <input_mp4_file> -o <output_mp4_file_name_without_extension>");
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
            InputFile = Path.Combine(exeDir, "..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v");
            OutputFile = Path.Combine(exeDir, "big_buck_bunny_trailer_iphone");

            Console.WriteLine("Using default options: ");
            Console.Write(" --input " + InputFile);
            Console.Write(" --output " + OutputFile);
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
                Console.WriteLine("\nRequired options are not set!");
                PrintUsage();
                Error = true;
                return false;
            }

            return true;
        }

        bool Validate()
        {
            return (OutputFile == null || InputFile == null) ? false : true;
        }
    }
}
