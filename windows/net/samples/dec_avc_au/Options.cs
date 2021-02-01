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

/*
command line options:

long format:
	dec_avc_au --input ..\assets\vid\folder.au\ --pattern au_{0:0000}.h264 --output file.yuv --frame 352x288 --rate 30 --color yuv420 

short format:
	dec_avc_au -i ..\assets\vid\folder.au\ -p au_{0:0000}.h264 -o file.yuv -f 352x288 -r 30 -c yuv420 

TODO: separate syntax from semantic processing.
1. Command Line Parser: get all options and validate syntax (get name->value; or just name; or just value)
2. enc_yuv_preset_file: translate command options to encoder options (parse names and values).
*/

namespace DecAvcAuSample
{
    class Options
    {
        // enc_avc_file command line options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input directory (contains sequence of compressed file)")]
        public string InputDir { get; set; }

        [Option('o', "output", HelpText = "output YUV file")]
        public string OutputFile { get; set; }

        [Option('r', "rate", HelpText = "output frame rate")]
        public double Fps { get; set; }

        [Option('f', "frame", HelpText = "frame size, <width>x<height>")]
        public string FrameSize { get; set; }

        [Option('c', "color", HelpText = "output color format. Use --colors to list all supported color formats")]
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
            Console.WriteLine("\nUsage: dec_avc_au --input <directory> [--output <file>] [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>]");
            Console.WriteLine("[--colors]");
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
                Width  = Convert.ToInt32(parts[0]);
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
            Help       = false;
            InputDir   = null;
            OutputFile = null;
            Fps        = 0.0;
            FrameSize  = null;
            ColorName  = null;
            Color      = null;
            ListColors = false;
            Error      = false;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            InputDir      = Path.Combine(exeDir, "..\\assets\\vid\\foreman_qcif.h264.au");
            //InputPattern  = "au_{0:0000}.h264";
            Color         = GetColorById(ColorFormat.YUV420);

            Console.WriteLine("Using default options: ");
            Console.Write(" --input " + InputDir);
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
            if (InputDir == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(InputDir);
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
