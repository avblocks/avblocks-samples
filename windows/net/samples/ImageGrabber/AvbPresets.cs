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

namespace ImageGrabber
{
    class PresetDescriptor
    {
        public string Name;
        public string FileExtension;

        public PresetDescriptor(string presetName, string fileExtension)
        {
            this.Name = presetName;
            this.FileExtension = fileExtension;
        }

        public override string ToString()
        {
            if (this.FileExtension == null)
                return this.Name;

            return string.Format("{0} (.{1})", this.Name, this.FileExtension);
        }
    };

    class AvbTranscoder
    {
        private static PresetDescriptor[] presets = new PresetDescriptor[]
        {
            // custom presets

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
             new PresetDescriptor(Preset.Video.Fast.MP4.H264_AAC,	  "mp4"),
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

        public static PresetDescriptor[] Presets
        {
            get { return presets; }
        }
    }
}