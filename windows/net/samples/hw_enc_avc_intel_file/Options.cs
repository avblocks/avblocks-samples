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

namespace HwEncAvcIntelFileSample
{
    class Options
    {
        // hw_enc_avc_intel_file command line options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input YUV file")]
        public string InputFile { get; set; }

        [Option('o', "output", HelpText = "output AVC file")]
        public string OutputFile { get; set; }

        [Option('r', "rate", HelpText = "output frame rate")]
        public double Fps { get; set; }

        [Option('f', "frame", HelpText = "output frame size")]
        public string FrameSize { get; set; }

        [Option('c', "color", HelpText = "output color format. Use --colors to list all supported color formats.")]
        public string ColorName { get; set; }

        [Option("colors", HelpText = "list COLOR constants")]
        public bool ListColors { get; set; }

        public int Width { get; private set; }
        public int Height { get; private set; }
        public ColorDescriptor Color { get; private set; }
        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("\nUsage: hw_enc_avc_intel_file --input <inputFile.yuv> --output <outputFile.h264> --rate 30 --frame 176x144 --color yuv420");
            Console.WriteLine(GetUsage());
        }

        bool ParseFrameSize()
        {
            if (string.IsNullOrEmpty(FrameSize))
                return false;

            Width = 0;
            Height = 0;

            var parts = FrameSize.Split('x');
            if (parts.Length != 2)
                return false;

            try
            {
                Width = Convert.ToInt32(parts[0]);
                Height = Convert.ToInt32(parts[1]);
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
            Fps = 0.0;
            FrameSize = null;
            ColorName = null;
            ListColors = false;
            Error = false;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            InputFile = Path.Combine(exeDir, "..\\assets\\vid\\foreman_qcif.yuv");
            OutputFile = Path.Combine(exeDir, "foreman_qcif.h264");
            Fps = 30.0;

            Width = 176; Height = 144;
            FrameSize = string.Format("{0}x{1}", Width, Height);

            Color = GetColorById(ColorFormat.YUV420);

            Console.WriteLine("Using default options: ");
            Console.Write("hw_enc_avc_amd_file --input " + InputFile);
            Console.Write(" --output " + OutputFile);
            Console.Write(" --rate " + Fps);
            Console.Write(" --frame " + FrameSize);
            Console.Write(" --color " + Color.Name);
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

            if(ListColors)
            {
                PrintColors();
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

            Console.Write("Input frame size: ");
            if (!ParseFrameSize())
            {
                Console.WriteLine("[not set / incorrect]");
                res = false;
            }
            else
            {
                Console.WriteLine(FrameSize);
            }

            Console.Write("Input color format: ");
            if (GetColorByName(ColorName) == null)
            {
                Console.WriteLine("[not set / incorrect]");
                res = false;
            }
            else
            {
                Color = GetColorByName(ColorName);
                Console.WriteLine(ColorName);
            }

            Console.Write("Output frame rate: ");
            if (Fps == 0.0)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(Fps);
            }

            return res;
        }

        static ColorDescriptor[] Colors = {
	        new ColorDescriptor( ColorFormat.YV12,	    "yv12",	    "Planar Y, V, U (4:2:0) (note V,U order!)" ),
	        new ColorDescriptor( ColorFormat.NV12,	    "nv12",	    "Planar Y, merged U->V (4:2:0)" ),
	        new ColorDescriptor( ColorFormat.YUY2,	    "yuy2",	    "Composite Y->U->Y->V (4:2:2)" ),
	        new ColorDescriptor( ColorFormat.UYVY,	    "uyvy",	    "Composite U->Y->V->Y (4:2:2)" ),
	        new ColorDescriptor( ColorFormat.YUV411,	"yuv411",	"Planar Y, U, V (4:1:1)" ),
	        new ColorDescriptor( ColorFormat.YUV420,	"yuv420",	"Planar Y, U, V (4:2:0)" ),
	        new ColorDescriptor( ColorFormat.YUV422,	"yuv422",	"Planar Y, U, V (4:2:2)" ),
	        new ColorDescriptor( ColorFormat.YUV444,	"yuv444",	"Planar Y, U, V (4:4:4)" ),
	        new ColorDescriptor( ColorFormat.Y411,	    "y411",	    "Composite Y, U, V (4:1:1)" ),
	        new ColorDescriptor( ColorFormat.Y41P,	    "y41p",	    "Composite Y, U, V (4:1:1)" ),
	        new ColorDescriptor( ColorFormat.BGR32,	    "bgr32",	"Composite B->G->R" ),
            new ColorDescriptor( ColorFormat.BGRA32,	"bgra32",	"Composite B->G->R->A" ),
	        new ColorDescriptor( ColorFormat.BGR24,	    "bgr24",	"Composite B->G->R" ),
	        new ColorDescriptor( ColorFormat.BGR565,	"bgr565",	"Composite B->G->R, 5 bit per B & R, 6 bit per G" ),
	        new ColorDescriptor( ColorFormat.BGR555,	"bgr555",	"Composite B->G->R->A, 5 bit per component, 1 bit per A" ),
	        new ColorDescriptor( ColorFormat.BGR444,	"bgr444",	"Composite B->G->R->A, 4 bit per component" ),
	        new ColorDescriptor( ColorFormat.GRAY,	    "gray",	    "Luminance component only" ),
	        new ColorDescriptor( ColorFormat.YUV420A,	"yuv420a",	"Planar Y, U, V, Alpha (4:2:0)" ),
	        new ColorDescriptor( ColorFormat.YUV422A,	"yuv422a",	"Planar Y, U, V, Alpha (4:2:2)" ),
	        new ColorDescriptor( ColorFormat.YUV444A,	"yuv444a",	"Planar Y, U, V, Alpha (4:4:4)" ),
	        new ColorDescriptor( ColorFormat.YVU9,	    "yvu9",	    "Planar Y, V, U, 9 bits per sample" ) 
        };

        static void PrintColors()
        {
            Console.WriteLine("\nCOLORS");
            Console.WriteLine("---------");
            foreach (var color in Colors)
            {
                Console.WriteLine("{0,-20} {1}", color.Name, color.Description);
            }
            Console.WriteLine();
        }

        static ColorDescriptor GetColorByName(string colorName)
        {
            foreach (var color in Colors)
            {
                if (color.Name.Equals(colorName, StringComparison.InvariantCultureIgnoreCase))
                    return color;
            }
            return null;
        }

        static ColorDescriptor GetColorById(ColorFormat colorId)
        {
            foreach (var color in Colors)
            {
                if (color.Id == colorId)
                    return color;
            }
            return null;
        }

    }

    class ColorDescriptor
    {
        public ColorDescriptor(ColorFormat id, string name, string description)
        {
            Id = id;
            Name = name;
            Description = description;
        }

        public ColorFormat Id { get; set; }
        public string Name { get; set; }
        public string Description { get; set; }
    }
}
