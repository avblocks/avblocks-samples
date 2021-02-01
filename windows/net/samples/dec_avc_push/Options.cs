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
using PrimoSoftware.AVBlocks;

namespace DecAvcPushSample
{
    class Options
    {
        // enc_avc_push command line options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input Access unit(AU) files")]
        public string InputDir { get; set; }

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
            Console.WriteLine("\nUsage: dec_avc_push --input <AU_Folder> --output <outputFile.yuv>");
            Console.WriteLine(GetUsage());
        }

        void ResetOptions()
        {
            Help = false;
            InputDir = null;
            OutputFile = null;
            Error = false;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            InputDir = Path.Combine(exeDir, "..\\assets\\vid\\foreman_qcif.h264.au");
            OutputFile = Path.Combine(exeDir, "decoded_file.yuv");

            Console.WriteLine("Using default options: ");
            Console.Write("dec_avc_push --input " + InputDir);
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
            bool res = true;

            if (InputDir == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }

            if (OutputFile == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }

            return res;
        }
    }
}
