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
	InputDS --input <file> --output <file> --preset <PRESET>

short format:
	InputDS -i <input file> -o <output file> -p <PRESET>
*/

namespace InputDS
{
    class Options
    {
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input file")]
        public string InputFile { get; set; }

        [Option('o', "output", HelpText = "output file")]
        public string OutputFile { get; set; }

        [Option('p', "preset", HelpText = "output preset")]
        public string OutputPreset { get; set; }

        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine("\nUsage: InputDS --input <file> --output <file> --preset <PRESET>");
            Console.WriteLine(GetUsage());
            PrintPresets();
        }

        void ResetOptions()
        {
            Help = false;
            InputFile = null;
            OutputFile = null;
            OutputPreset = null;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

            InputFile = Path.Combine(exeDir, "..\\assets\\mov\\bars_100.avi");
            OutputFile = "bars_100.mp4";
            OutputPreset = Preset.Video.iPad.H264_720p;

            Console.WriteLine("Using default options: --input " + InputFile + 
                                                    " --output " + OutputFile +
                                                    " --preset " + OutputPreset);
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
            if (string.IsNullOrEmpty(InputFile))
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(InputFile);
            }

            Console.Write("Output file: ");
            if (string.IsNullOrEmpty(OutputFile))
            {
                Console.WriteLine("[not set]");
                res = false;
            }
            else
            {
                Console.WriteLine(OutputFile);
            }


            Console.Write("Output Preset: ");
            if (GetPresetByName(OutputPreset) == null)
            {
                if (string.IsNullOrEmpty(OutputPreset))
                {
                    Console.WriteLine("[not set]");
                }
                else
                {
                    Console.WriteLine("[not found]  " + OutputPreset);
                }
                
                res = false;
            }
            else
            {
                Console.WriteLine(OutputPreset);
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


        public static void PrintPresets()
        {
            Console.WriteLine("\nPRESETS");
            Console.WriteLine(  "-----------------------------------");
            foreach (var preset in AvbPresets)
            {
                Console.WriteLine("{0,-30} .{1}", preset.Name, preset.Extension);
            }
            Console.WriteLine();
        }

        public static PresetDescriptor GetPresetByName(string presetName)
        {
            foreach (var preset in AvbPresets)
            {
                if (preset.Name.Equals(presetName, StringComparison.InvariantCultureIgnoreCase))
                    return preset;
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
}