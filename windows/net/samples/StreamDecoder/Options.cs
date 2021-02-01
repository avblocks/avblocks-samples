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
	streamdecoder --input file.h264 --output output.yuv --frame 352x288 --rate 30 --color yuv420 --streamtype h264

short format:
	streamdecoder -i file.h264 -o output.yuv -f 352x288 -r 30 -c yuv420 -s h264
*/

namespace StreamDecoderSample
{
    class Options
    {
        // StreamDecoder options
        [Option('?', "help", HelpText = "Display this help screen")]
        public bool Help { get; set; }

        [Option('i', "input", HelpText = "input file")]
        public string InputFile { get; set; }

        [Option('o', "output", HelpText = "output YUV file")]
        public string OutputFile { get; set; }

        [Option("colors", HelpText = "list COLOR constants")]
        public bool ListColors { get; set; }

        [Option("streamtypes", HelpText = "list stream type constants")]
        public bool ListStreamTypes { get; set; }

        [Option('r', "rate", HelpText = "output frame rate")]
        public double Fps { get; set; }

        [Option('f', "frame", HelpText = "output frame size")]
        public string FrameSize { get; set; }

        [Option('c', "color", HelpText = "output color format. Use --colors to list all supported color formats.")]
        public string ColorID { get; set; }

        [Option('s', "streamtype", HelpText = "input stream type.  Use --stramtypes to list all supported stream types")]
        public string StreamTypeID { get; set; }

        // The program parses the command line options and sets these properties
        public bool Error { get; private set; }
        public ColorDescriptor YuvColor { get; private set; }
        public StreamTypeDescriptor InputStreamType { get; private set; }
        public int Width { get; private set; }
        public int Height { get; private set; }

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

            if (ListStreamTypes)
            {
                PrintStreamTypes();
                return false;
            }

            ParseFrameSize();
            YuvColor = GetColorByName(ColorID);
            InputStreamType = GetStreamTypeByName(StreamTypeID);
            
            if (!Validate())
            {
                Console.WriteLine("\nRequired options are not set!");
                PrintUsage();
                Error = true;
                return false;
            }

            if (InputStreamType == null)
            {
                Console.WriteLine("Input stream type: [not set]");
                StreamTypeDescriptor streamType = GetStreamTypeByExtension(Path.GetExtension(InputFile).Substring(1));
                if (streamType != null)
                {
                    InputStreamType = streamType;
                    Console.WriteLine("Input stream type by file extension: {0}", InputStreamType.Name);
                }
                else
                {
                    Console.WriteLine("Input stream type cannot be detected by file extension.");
                    Error = true;
                    return false;
                }
            }

            AddFrameSizeToOutputFileName();

            return true;
        }

        string GetUsage()
        {
            return HelpText.AutoBuild(this, (HelpText current) => HelpText.DefaultParsingErrorsHandler(this, current));
        }

        void PrintUsage()
        {
            Console.WriteLine(
                "\nUsage: streamdecoder --input <file> [--streamtype <STREAM TYPE>] --output <file> [--frame <width>x<height>] [--rate <fps>] --color <COLOR>" +
                "\n[--colors] [--streamtypes]");

            Console.WriteLine(GetUsage());
            PrintColors();
            PrintStreamTypes();
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
            ListColors = false;
            ListStreamTypes = false;
            Fps = 0.0;
            FrameSize = null;
            ColorID = null;
            StreamTypeID = null;

            Error = false;
            YuvColor = null;
            InputStreamType = null;
            Width  = 0;
            Height = 0;
        }

        void SetDefaultOptions()
        {
            string exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);

            ColorID = "yuv420";
            InputFile = Path.Combine(exeDir, "..\\assets\\vid\\foreman_qcif.h264");
            OutputFile = "foreman_qcif.yuv";

            Console.WriteLine("Using default options: --input " + InputFile + " --output " + OutputFile + " --color " + ColorID );
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

            return res;
        }

        bool AddFrameSizeToOutputFileName()
        {
            if (string.IsNullOrEmpty(InputFile) ||
                string.IsNullOrEmpty(OutputFile))
            {
                return false;
            }

            int width = 0;
            int height = 0;

            if ((Width > 0) && (Height > 0))
            {
                width = Width;
                height = Height;
            }
            else
            {
                using (MediaInfo info = new MediaInfo())
                {
                    info.Inputs[0].File = InputFile;
                    info.Inputs[0].StreamType = InputStreamType.Id;

                    if (!info.Open())
                    {
                        Console.WriteLine("MediaInfo Open " + info.Error);
                        return false;
                    }

                    foreach(var socket in info.Outputs)
                    {
                        foreach(var pin in socket.Pins)
                        {
                            StreamInfo si = pin.StreamInfo;
                            if (si.MediaType == MediaType.Video)
                            {
                                VideoStreamInfo vsi = (VideoStreamInfo)si;
                                width = vsi.FrameWidth;
                                height = vsi.FrameHeight;
                                break;
                            }
                        }
                    }
                }
            }

            if ((width <= 0) || (height <= 0))
                return false;

            int pos = OutputFile.LastIndexOf(".");
            if (pos != -1)
            {
                string Name = OutputFile.Substring(0, pos);
                string Ext = OutputFile.Substring(pos);
                OutputFile = Name + "_" + width + "x" + height + Ext;
            }

            return true;
        }

        private static StreamTypeDescriptor[] AvbStreamTypes = new StreamTypeDescriptor[]
        {
            // video presets
            new StreamTypeDescriptor( StreamType.Mp4,           "MP4",              "mp4" ),
            new StreamTypeDescriptor( StreamType.Mp4,           "MP4",              "mov" ),
            new StreamTypeDescriptor( StreamType.Mp4,           "MP4",              "m4v" ),
            new StreamTypeDescriptor( StreamType.Avi,           "AVI",              "avi" ),

            new StreamTypeDescriptor( StreamType.MpegPS,        "MPEG_PS",          "mpg" ),
            new StreamTypeDescriptor( StreamType.MpegTS,        "MPEG_TS",          "ts" ),

            new StreamTypeDescriptor( StreamType.Mpeg2Video,    "MPEG2_Video",      "m2v" ),
            new StreamTypeDescriptor( StreamType.Mpeg2Video,    "MPEG2_Video",      "mpv" ),

            new StreamTypeDescriptor( StreamType.Asf,           "ASF",              "asf" ),
            new StreamTypeDescriptor( StreamType.Asf,           "ASF",              "wmv" ),

            new StreamTypeDescriptor( StreamType.H264,          "H264",             "h264" ),
            new StreamTypeDescriptor( StreamType.WebM,          "WebM",             "webm" ),
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


        static void PrintStreamTypes()
        {
            Console.WriteLine("\nSTREAM TYPES");
            Console.WriteLine("------------------------");
            foreach (var streamType in AvbStreamTypes)
            {
                Console.WriteLine("{0,-20} (.{1})", streamType.Name, streamType.Extension); 
            }
            Console.WriteLine();
        }

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

        static StreamTypeDescriptor GetStreamTypeByName(string streamTypeName)
        {
            foreach (var streamType in AvbStreamTypes)
            {
                if (streamType.Name.Equals(streamTypeName, StringComparison.InvariantCultureIgnoreCase))
                    return streamType;
            }
            return null;
        }

        static StreamTypeDescriptor GetStreamTypeByExtension(string extension)
        {
            foreach (var streamType in AvbStreamTypes)
            {
                if (streamType.Extension.Equals(extension, StringComparison.InvariantCultureIgnoreCase))
                    return streamType;
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

    class StreamTypeDescriptor
    {
        public StreamTypeDescriptor(StreamType id, string name, string fileExtension)
        {
            Id = id;
            Name = name;
            Extension = fileExtension;
        }

        public string Name { get; set; }
        public string Extension { get; set; }
        public StreamType Id { get; set; }
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
