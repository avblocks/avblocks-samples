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
	enc_yuv_preset_file --frame 352x288 --rate 30 --color yuv420 --input file.yuv --output output.mpg --preset ipad.mp4.h264.720p

short format:
	enc_yuv_preset_file -f 352x288 -r 30 -c yuv420 -i file.yuv -o output.mpg -p ipad.mp4.h264.720p
*/

namespace YuvEncoderSample
{
    class Options
    {
        // enc_yuv_preset_file command line options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option("colors", HelpText = "list COLOR constants")]
        public bool ListColors { get; set; }

        [Option("presets", HelpText = "list PRESET constants")]
        public bool ListPresets { get; set; }

        [Option('i', "input", HelpText = "input YUV file")]
        public string YuvFile { get; set; }

        [Option('o', "output", HelpText = "output file")]
        public string OutputFile { get; set; }

        [Option('p', "preset", HelpText = "output preset. Use --presets to list all supported presets.")]
        public string PresetID { get; set; }

        [Option('r', "rate", HelpText = "input frame rate")]
        public double YuvFps { get; set; }

        [Option('f', "frame", HelpText = "input frame size")]
        public string FrameSize { get; set; }

        [Option('c', "color", HelpText = "input color format. Use --colors to list all supported color formats.")]
        public string ColorID { get; set; }


        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }
        public int YuvWidth { get; private set; }
        public int YuvHeight { get; private set; }
        public PresetDescriptor OutputPreset { get; private set; }
        public ColorDescriptor YuvColor { get; private set; }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        bool ParseFrameSize()
        {
            if (string.IsNullOrEmpty(FrameSize))
                return false;

            YuvWidth = 0;
            YuvHeight = 0;

            var parts = FrameSize.Split('x');
            if (parts.Length != 2)
                return false;

            try
            {
                YuvWidth = Convert.ToInt32(parts[0]);
                YuvHeight = Convert.ToInt32(parts[1]);
            }
            catch (Exception)
            {
                return false;
            }
            return true;
        }

        void PrintUsage()
        {
            Console.WriteLine(
                "\nUsage: enc_yuv_preset_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file> --output <file> " +
                "[--preset <PRESET>] [--colors] [--presets]");

            Console.WriteLine(GetUsage());
            PrintPresets();
            PrintColors();
        }

        void ResetOptions()
        {
            Help = false;
            Error = false;
            ListColors = false;
            ListPresets = false;
            OutputFile = null;
            OutputPreset = null;
            YuvFile = null;
            YuvWidth = 0;
            YuvHeight = 0;
            YuvFps = 0.0;
            YuvColor = null;
            ColorID = null;
            PresetID = null;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

            FrameSize = "176x144";
            YuvFps = 30.0;
            ColorID = "yuv420";
            YuvFile = Path.Combine(exeDir, "..\\assets\\vid\\foreman_qcif.yuv");
            OutputFile = "foreman_qcif.mp4";
            PresetID = Preset.Video.iPad.H264_720p;

            Console.WriteLine("Using default options: --frame " + FrameSize + " --rate " + YuvFps + " --color " +
                                    ColorID + " --input " + YuvFile + " --output " + OutputFile + " --preset " + PresetID);
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

            if (ListColors)
            {
                PrintColors();
                return false;
            }

            if (ListPresets)
            {
                PrintPresets();
                return false;
            }

            OutputPreset = GetPresetByName(PresetID);
            YuvColor = GetColorByName(ColorID);
            ParseFrameSize();

            if (!Validate())
            {
                Console.WriteLine("\nRequired options are not set!");
                PrintUsage();
                Error = true;
                return false;
            }

            // fix output file extension
            if (!string.IsNullOrEmpty(OutputFile))
            {
                OutputFile = Path.GetFileNameWithoutExtension(OutputFile) + "." + OutputPreset.Extension;
            }
            
            return true;
        }

        bool Validate()
        {
            bool res = true;

            Console.Write("YUV input file: ");
            if (YuvFile == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(YuvFile);
            }

            Console.Write("YUV frame size: ");
            if (YuvWidth == 0 || YuvHeight == 0)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine("{0}x{1}", YuvWidth, YuvHeight);
            }

            Console.Write("YUV color: ");
            if (YuvColor == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(YuvColor.Name);
            }

            Console.Write("YUV frame rate: ");
            if (YuvFps == 0.0)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(YuvFps);
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

            Console.Write("Output Preset: ");
            if (OutputPreset == null)
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(OutputPreset.Name);
            }

            return res;
        }


        private static PresetDescriptor[] AvbPresets = new PresetDescriptor[]
        {
            // video presets
             new PresetDescriptor(Preset.Video.DVD.NTSC_16x9_MP2,       "mpg"),
             new PresetDescriptor(Preset.Video.DVD.NTSC_16x9_PCM,       "mpg"),
             new PresetDescriptor(Preset.Video.DVD.NTSC_4x3_MP2,        "mpg"),
	         new PresetDescriptor(Preset.Video.DVD.NTSC_4x3_PCM,  	    "mpg"),
	         new PresetDescriptor(Preset.Video.DVD.PAL_16x9_MP2,      	"mpg"),
	         new PresetDescriptor(Preset.Video.DVD.PAL_4x3_MP2,       	"mpg"),
	         new PresetDescriptor(Preset.Video.iPad.H264_576p,			"mp4"),
	         new PresetDescriptor(Preset.Video.iPad.H264_720p,			"mp4"),
	         new PresetDescriptor(Preset.Video.iPad.MPEG4_480p,       	"mp4"),
	         new PresetDescriptor(Preset.Video.iPhone.H264_480p,		"mp4"),
	         new PresetDescriptor(Preset.Video.iPhone.MPEG4_480p,		"mp4"),
	         new PresetDescriptor(Preset.Video.iPod.H264_240p,			"mp4"),
	         new PresetDescriptor(Preset.Video.iPod.MPEG4_240p,			"mp4"),
             new PresetDescriptor(Preset.Video.Generic.MP4.Base_H264_AAC,"mp4"),
	         new PresetDescriptor(Preset.Video.AppleLiveStreaming.H264_480p,  "ts"),
	         new PresetDescriptor(Preset.Video.AppleLiveStreaming.H264_720p,  "ts"),
	         new PresetDescriptor(Preset.Video.AndroidPhone.H264_360p,	"mp4"),
	         new PresetDescriptor(Preset.Video.AndroidPhone.H264_720p,  "mp4"),
	         new PresetDescriptor(Preset.Video.AndroidTablet.H264_720p,	"mpg"),
	         new PresetDescriptor(Preset.Video.AndroidTablet.WebM_VP8_720p, "webm"),
	         new PresetDescriptor(Preset.Video.VCD.NTSC,  			        "mpg"),
	         new PresetDescriptor(Preset.Video.VCD.PAL,  			        "mpg"),
	         new PresetDescriptor(Preset.Video.Generic.WebM.Base_VP8_Vorbis,  	"webm"),
        };

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
	        new ColorDescriptor( ColorFormat.YVU9,	    "yvu9",	    "Planar Y, V, U, 9 bits per sample" ) };

        static void PrintPresets()
        {
            Console.WriteLine("\nPRESETS");
            Console.WriteLine("-------");
            foreach (var preset in AvbPresets)
            {
                Console.WriteLine("{0,-30} .{1}", preset.Name, preset.Extension);
            }
            Console.WriteLine();
        }

        static void PrintColors()
        {
            Console.WriteLine("\nCOLORS");
            Console.WriteLine("------");
            foreach (var color in Colors)
            {
                Console.WriteLine("{0,-20} {1}", color.Name, color.Description);
            }
            Console.WriteLine();
        }

        static PresetDescriptor GetPresetByName(string presetName)
        {
            foreach (var preset in AvbPresets)
            {
                if (preset.Name.Equals(presetName, StringComparison.InvariantCultureIgnoreCase))
                    return preset;
            }
            return null;
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
    }

    class PresetDescriptor
    {
        public PresetDescriptor(string name, string fileExtension)
        {
            Name = name;
            Extension = fileExtension;
        }

        public string Name { get; set; }
        public string Extension { get; set; }
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
