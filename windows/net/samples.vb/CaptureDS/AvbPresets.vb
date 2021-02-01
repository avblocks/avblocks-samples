'
' *  Copyright (c) 2013 Primo Software. All Rights Reserved.
' *
' *  Use of this source code is governed by a BSD-style license
' *  that can be found in the LICENSE file in the root of the source
' *  tree.  
'
Imports System
Imports System.Collections.Generic
Imports System.Text
Imports PrimoSoftware.AVBlocks

Namespace CaptureDS
	Friend Class PresetDescriptor
		Public Name As String
		Public AudioOnly As Boolean
		Public FileExtension As String

		Public Sub New(ByVal presetName As String, ByVal audioOnly As Boolean, ByVal fileExtension As String)
			Me.Name = presetName
			Me.AudioOnly = audioOnly
			Me.FileExtension = fileExtension
		End Sub

		Public Overrides Function ToString() As String
			If Me.FileExtension Is Nothing Then
				Return Me.Name
			End If

			Return String.Format("{0} (.{1})", Me.Name, Me.FileExtension)
		End Function
	End Class

	Friend Class AvbTranscoder
        Private Shared _presets() As PresetDescriptor = {
           New PresetDescriptor("custom-mp4-h264-704x576-25fps-aac", False, "mp4"),
           New PresetDescriptor("custom-mp4-h264-704x576-12fps-aac", False, "mp4"),
           New PresetDescriptor("custom-mp4-h264-352x288-25fps-aac", False, "mp4"),
           New PresetDescriptor("custom-mp4-h264-352x288-12fps-aac", False, "mp4"),
           New PresetDescriptor(Preset.Video.DVD.NTSC_16x9_MP2, False, "mpg"),
           New PresetDescriptor(Preset.Video.DVD.NTSC_16x9_PCM, False, "mpg"),
           New PresetDescriptor(Preset.Video.DVD.NTSC_4x3_MP2, False, "mpg"),
           New PresetDescriptor(Preset.Video.DVD.NTSC_4x3_PCM, False, "mpg"),
           New PresetDescriptor(Preset.Video.DVD.PAL_16x9_MP2, False, "mpg"),
           New PresetDescriptor(Preset.Video.DVD.PAL_4x3_MP2, False, "mpg"),
           New PresetDescriptor(Preset.Video.iPad.H264_576p, False, "mp4"),
           New PresetDescriptor(Preset.Video.iPad.H264_720p, False, "mp4"),
           New PresetDescriptor(Preset.Video.iPad.MPEG4_480p, False, "mp4"),
           New PresetDescriptor(Preset.Video.iPhone.H264_480p, False, "mp4"),
           New PresetDescriptor(Preset.Video.iPhone.MPEG4_480p, False, "mp4"),
           New PresetDescriptor(Preset.Video.iPod.H264_240p, False, "mp4"),
           New PresetDescriptor(Preset.Video.iPod.MPEG4_240p, False, "mp4"),
           New PresetDescriptor(Preset.Video.AppleLiveStreaming.H264_480p, False, "ts"),
           New PresetDescriptor(Preset.Video.AppleLiveStreaming.H264_720p, False, "ts"),
           New PresetDescriptor(Preset.Video.AndroidPhone.H264_360p, False, "mp4"),
           New PresetDescriptor(Preset.Video.AndroidPhone.H264_720p, False, "mp4"),
           New PresetDescriptor(Preset.Video.AndroidTablet.H264_720p, False, "mpg"),
           New PresetDescriptor(Preset.Video.AndroidTablet.WebM_VP8_720p, False, "webm"),
           New PresetDescriptor(Preset.Video.VCD.NTSC, False, "mpg"),
           New PresetDescriptor(Preset.Video.VCD.PAL, False, "mpg"),
           New PresetDescriptor(Preset.Video.Generic.WebM.Base_VP8_Vorbis, False, "webm"),
           New PresetDescriptor(Preset.Audio.Generic.AAC, True, "aac"),
           New PresetDescriptor(Preset.Audio.Generic.M4A.CBR_128kbps, True, "m4a"),
           New PresetDescriptor(Preset.Audio.Generic.M4A.CBR_256kbps, True, "m4a"),
           New PresetDescriptor(Preset.Audio.DVD.MP2, True, "mp2"),
           New PresetDescriptor(Preset.Audio.Generic.MP3.CBR_128kbps, True, "mp3"),
           New PresetDescriptor(Preset.Audio.Generic.MP3.CBR_256kbps, True, "mp3"),
           New PresetDescriptor(Preset.Audio.Generic.OggVorbis.VBR_Q4, True, "ogg"),
           New PresetDescriptor(Preset.Audio.Generic.OggVorbis.VBR_Q8, True, "ogg"),
           New PresetDescriptor(Preset.Audio.AudioCD.WAV, True, "wav"),
           New PresetDescriptor(Preset.Audio.Generic.WMA.Lossless.CD, True, "wma"),
           New PresetDescriptor(Preset.Audio.Generic.WMA.Professional.VBR_Q75, True, "wma"),
           New PresetDescriptor(Preset.Audio.Generic.WMA.Professional.VBR_Q90, True, "wma"),
           New PresetDescriptor(Preset.Audio.Generic.WMA.Standard.CBR_128kbps, True, "wma")
          }

        Public Shared ReadOnly Property Presets() As PresetDescriptor()
            Get
                Return _presets
            End Get
        End Property
	End Class
End Namespace