/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.Collections.Generic;
using System.Text;
using PrimoSoftware.AVBlocks;

namespace AudioConverter
{
    class PresetDescriptor
    {
        public string Name;
        public bool AudioOnly;
        public string FileExtension;

        public PresetDescriptor(string name, bool audioOnly, string fileExtension)
        {
            this.Name = name;
            this.AudioOnly = audioOnly;
            this.FileExtension = fileExtension;
        }

        public override string ToString()
        {
            if (this.FileExtension == null)
                return this.Name;

            return string.Format("{0} (.{1})", this.Name, this.FileExtension);
            
            //return this.Name.ToUpper();
        }
    };

    class AvbTranscoder
    {
        private static PresetDescriptor[] presets = new PresetDescriptor[]
        {
            // video presets
             new PresetDescriptor(Preset.Video.DVD.NTSC_16x9_MP2,               false, "mpg"),
             new PresetDescriptor(Preset.Video.DVD.NTSC_16x9_PCM,               false, "mpg"),
             new PresetDescriptor(Preset.Video.DVD.NTSC_4x3_MP2,                false, "mpg"),
	         new PresetDescriptor(Preset.Video.DVD.NTSC_4x3_PCM,                false,	"mpg"),
	         new PresetDescriptor(Preset.Video.DVD.PAL_16x9_MP2,                false,	"mpg"),
	         new PresetDescriptor(Preset.Video.DVD.PAL_4x3_MP2,                 false,	"mpg"),
	         new PresetDescriptor(Preset.Video.iPad.H264_576p,                  false,	"mp4"),
	         new PresetDescriptor(Preset.Video.iPad.H264_720p,                  false,	"mp4"),
	         new PresetDescriptor(Preset.Video.iPad.MPEG4_480p,                 false,	"mp4"),
	         new PresetDescriptor(Preset.Video.iPhone.H264_480p,                false,	"mp4"),
	         new PresetDescriptor(Preset.Video.iPhone.MPEG4_480p,               false, "mp4"),
	         new PresetDescriptor(Preset.Video.iPod.H264_240p,                  false,	"mp4"),
	         new PresetDescriptor(Preset.Video.iPod.MPEG4_240p,	                false,	"mp4"),
	         new PresetDescriptor(Preset.Video.AppleLiveStreaming.H264_480p,    false,	"ts"),
	         new PresetDescriptor(Preset.Video.AppleLiveStreaming.H264_720p,    false,	"ts"),
	         new PresetDescriptor(Preset.Video.AndroidPhone.H264_360p,          false, "mp4"),
	         new PresetDescriptor(Preset.Video.AndroidPhone.H264_720p,          false, "mp4"),
	         new PresetDescriptor(Preset.Video.AndroidTablet.H264_720p,         false, "mpg"),
	         new PresetDescriptor(Preset.Video.AndroidTablet.WebM_VP8_720p,     false, "webm"),
	         new PresetDescriptor(Preset.Video.VCD.NTSC,  			            false,	"mpg"),
	         new PresetDescriptor(Preset.Video.VCD.PAL,  			            false,	"mpg"),
	         new PresetDescriptor(Preset.Video.Generic.WebM.Base_VP8_Vorbis,    false,	"webm"),
			
	        // audio presets
	        new PresetDescriptor(Preset.Audio.Generic.AAC,			            true,	"aac"),
	        new PresetDescriptor(Preset.Audio.Generic.M4A.CBR_128kbps,          true,	"m4a"),
	        new PresetDescriptor(Preset.Audio.Generic.M4A.CBR_256kbps,	        true,	"m4a"),
	        new PresetDescriptor(Preset.Audio.DVD.MP2,			                true,	"mp2"),
	        new PresetDescriptor(Preset.Audio.Generic.MP3.CBR_128kbps,	        true,	"mp3"),
	        new PresetDescriptor(Preset.Audio.Generic.MP3.CBR_256kbps,	        true,	"mp3"),
	        new PresetDescriptor(Preset.Audio.Generic.OggVorbis.VBR_Q4,	        true,	"ogg"),
	        new PresetDescriptor(Preset.Audio.Generic.OggVorbis.VBR_Q8,	        true,	"ogg"),
	        new PresetDescriptor(Preset.Audio.AudioCD.WAV,		                true,	"wav"),
	        new PresetDescriptor(Preset.Audio.Generic.WMA.Lossless.CD,	        true,	"wma"),
	        new PresetDescriptor(Preset.Audio.Generic.WMA.Professional.VBR_Q75,	true,	"wma"),
	        new PresetDescriptor(Preset.Audio.Generic.WMA.Professional.VBR_Q90,	true,	"wma"),
	        new PresetDescriptor(Preset.Audio.Generic.WMA.Standard.CBR_128kbps,	true,	"wma"),
        };

        public static PresetDescriptor[] Presets
        {
            get { return presets; }
        }

        private static string FormatErrorMessage(ErrorInfo e) 
        {
            return string.Format("{0} Error, Code: {1} ({2})", e.Facility, e.Code, e.Message ?? "");
        }

        // return error message or null if success
	    public static string ConvertFileWithPreset(string inputFile, string outputFile, string outputPreset,
                                                EventHandler<TranscoderContinueEventArgs> onContinue,
                                                EventHandler<TranscoderProgressEventArgs> onProgress,
                                                EventHandler<TranscoderStatusEventArgs> onStatus
                                                )
        {
            using (MediaInfo mediaInfo = new MediaInfo())
            {
                mediaInfo.Inputs[0].File = inputFile;
                if (!mediaInfo.Open())
                {
                    return FormatErrorMessage(mediaInfo.Error);
                }

                using (Transcoder t = new Transcoder())
                {
                    // In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
                    t.AllowDemoMode = true;

                    if (onContinue != null)
                        t.OnContinue += onContinue;

                    if (onProgress != null)
                        t.OnProgress += onProgress;

                    if (onStatus != null)
                        t.OnStatus += onStatus;

                    MediaSocket insocket = MediaSocket.FromMediaInfo(mediaInfo);
                    t.Inputs.Add(insocket);
                    MediaSocket outsocket = MediaSocket.FromPreset(outputPreset);
                    if (outsocket == null)
                    {
                        return "Cannot create output preset";
                    }
                    outsocket.File = outputFile;
                    t.Outputs.Add(outsocket);

                    if (!t.Open())
                        return FormatErrorMessage(t.Error);

                    if (!t.Run())
                        return FormatErrorMessage(t.Error);

                    t.Close();
                }
            }

            return null; // no error
        }
    }
   
}