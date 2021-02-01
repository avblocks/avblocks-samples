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
using System.Linq;
using CommandLine;
using CommandLine.Text;
using PrimoSoftware.AVBlocks;

namespace OverlayMp4PngFileSample
{
    class Options
    {
        // overlay_mp4_png_file command line options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input video file")]
        public string InputFile { get; set; }

        [Option('o', "output", HelpText = "overlayed output file")]
        public string OutputFile { get; set; }

        [Option('w', "watermark", HelpText = "overlay PNG image")]
        public string Watermark { get; set; }

        [Option('a', "alpha", HelpText = "watermark transparency from 0(transparent) to 1(opaque)")]
        public double Alpha { get; set; }

        [Option('p', "position", HelpText = "the top left position of the watermark in the video frame. The top left position of the video frame is origin for this coordinate system.")]
        public string Position { get; set; }

        public int PositionX { get; private set; }
        public int PositionY { get; private set; }

        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("\nUsage: -i <input video file> -w <PNG file> -p <x>:<y> -a <transparency> -o <output video file>");
            Console.WriteLine(GetUsage());
        }

        bool ParsePostion()
        {
            if (string.IsNullOrEmpty(Position))
                return false;

            PositionX = 0;
            PositionY = 0;

            var parts = Position.Split(':');
            if (parts.Length != 2)
                return false;

            try
            {
                PositionX = Convert.ToInt32(parts[0]);
                PositionY = Convert.ToInt32(parts[1]);
            }
            catch (Exception)
            {
                return false;
            }
            return true;
        }

        void ResetOptions()
        {
            Help = false;
            InputFile = null;
            OutputFile = null;
            Position = null;
            Alpha = 0.0;
            Watermark = null;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            InputFile = Path.Combine(exeDir, "..\\assets\\mov\\big_buck_bunny_trailer_iphone.m4v");
            Watermark = Path.Combine(exeDir, "..\\assets\\overlay\\smile_icon.png");
            OutputFile = Path.Combine(exeDir, "overlay_mp4_png_file.m4v");
            Alpha = 0.5;
            PositionX = 50;
            PositionY = 50;

            Console.WriteLine("Using default options: ");
            Console.Write(" --input " + InputFile);
            Console.Write(" --watermark " + Watermark);
            Console.Write(" --alpha " + Alpha);
            Console.Write(" --position " + PositionX + ":" + PositionY);
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
            if (OutputFile == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(OutputFile);
            }

            Console.Write("Watermark file: ");
            if (Watermark == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(OutputFile);
            }

            Console.Write("Position: ");
            if (!ParsePostion())
            {
                Console.WriteLine("[not set / incorrect]");
                res = false;
            }
            else
            {
                Console.WriteLine(Position);
            }

            Console.Write("Alpha: ");
            if (Alpha < 0 || Alpha > 1)
            {
                Console.WriteLine("[incorrect]");
                res = false;
            }
            else
            {
                Console.WriteLine(Alpha);
            }

            return res;
        }
    }
}
