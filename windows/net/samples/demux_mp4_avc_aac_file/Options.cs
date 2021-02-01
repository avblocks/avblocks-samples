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

namespace DemuxMp4AvcAacFileSample
{
    class Options
    {
        string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

        // mux_mp4_avc_aac_file options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [OptionArray('i', "input", HelpText = "input file (mp4)")]
        public string InputFile { get; set; }

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
            Console.WriteLine("Usage: demux_mp4_avc_aac_file -i <input_mp4_file> -o <output_file_name_without_extension>");
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
            InputFile = Path.Combine(exeDir, "..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v");

            OutputFile = "demux_mp4_avc_aac_file";

            Console.WriteLine("Using default options: ");
            Console.WriteLine("--input " + InputFile);
            Console.WriteLine("--output " + OutputFile);
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

            InputFile = Path.Combine(exeDir, InputFile);
            OutputFile = Path.Combine(exeDir, OutputFile);

            return true;
        }

        bool Validate()
        {
            if (InputFile == null)
            {
                return false;
            }

            if (OutputFile == null)
            {
                return false;
            }

            return true;
        }
    }
}
