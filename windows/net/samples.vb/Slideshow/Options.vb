'
' *  Copyright (c) 2013 Primo Software. All Rights Reserved.
' *
' *  Use of this source code is governed by a BSD-style license
' *  that can be found in the LICENSE file in the root of the source
' *  tree.  
'
Imports System
Imports System.IO
Imports CommandLine
Imports CommandLine.Text
Imports PrimoSoftware.AVBlocks


Namespace SlideshowSample
    Friend Class Options
        <[Option]("?"c, "help", HelpText:="Display this help screen")>
        Public Property Help() As Boolean

        <[Option]("p"c, "preset", HelpText:="output preset.")>
        Public Property PresetID() As String

        ' The program parses the command line options and sets these properties
        Private private_Error As Boolean
        Public Property [Error]() As Boolean
            Get
                Return private_Error
            End Get
            Private Set(ByVal value As Boolean)
                private_Error = value
            End Set
        End Property
        Private privateFileExtension As String
        Public Property FileExtension() As String
            Get
                Return privateFileExtension
            End Get
            Private Set(ByVal value As String)
                privateFileExtension = value
            End Set
        End Property

        Private Function GetUsage() As String
            Return HelpText.AutoBuild(Me, Sub(current As HelpText) HelpText.DefaultParsingErrorsHandler(Me, current))
        End Function

        Private Sub PrintUsage()
            Console.WriteLine("Usage: Slideshow [-p PRESET]")
            Console.WriteLine(GetUsage())
            PrintPresets()
        End Sub

        Private Sub ResetOptions()
            Help = False
            [Error] = False
            PresetID = Nothing
            FileExtension = Nothing
        End Sub

        Private Sub SetDefaultOptions()
            PresetID = Preset.Video.iPad.H264_720p
            Console.WriteLine("Using default options: -p " & PresetID)
        End Sub

        Public Function Prepare(ByVal args() As String) As Boolean
            ResetOptions()

            If args.Length = 0 Then
                SetDefaultOptions()
            Else
                If Not CommandLine.Parser.Default.ParseArguments(args, Me) Then
                    Console.WriteLine("Syntax error")
                    PrintUsage()
                    [Error] = True
                    Return False
                End If
            End If

            If Help Then
                PrintUsage()
                Return False
            End If

            If Not Validate() Then
                Console.WriteLine("Required options are not set!")
                PrintUsage()
                [Error] = True
                Return False
            End If

            ' get filename extension from preset descriptor
            Dim preset As PresetDescriptor = GetPresetByName(PresetID)
            If preset IsNot Nothing Then
                FileExtension = preset.FileExtension
            Else
                Console.WriteLine("Preset not found!")
                PrintUsage()
                [Error] = True
                Return False
            End If

            Return True
        End Function

        Private Function Validate() As Boolean
            Dim res As Boolean = True

            Console.Write("Output Preset: ")
            If PresetID Is Nothing Then
                Console.WriteLine("[not set]")
                res = False
            Else
                Console.WriteLine(PresetID)
            End If

            Return res
        End Function


        Private Shared AvbPresets() As PresetDescriptor = {
         New PresetDescriptor(Preset.Video.DVD.NTSC_16x9_MP2, "mpg"),
         New PresetDescriptor(Preset.Video.DVD.NTSC_16x9_PCM, "mpg"),
         New PresetDescriptor(Preset.Video.DVD.NTSC_4x3_MP2, "mpg"),
         New PresetDescriptor(Preset.Video.DVD.NTSC_4x3_PCM, "mpg"),
         New PresetDescriptor(Preset.Video.DVD.PAL_16x9_MP2, "mpg"),
         New PresetDescriptor(Preset.Video.DVD.PAL_4x3_MP2, "mpg"),
         New PresetDescriptor(Preset.Video.iPad.H264_576p, "mp4"),
         New PresetDescriptor(Preset.Video.iPad.H264_720p, "mp4"),
         New PresetDescriptor(Preset.Video.iPad.MPEG4_480p, "mp4"),
         New PresetDescriptor(Preset.Video.iPhone.H264_480p, "mp4"),
         New PresetDescriptor(Preset.Video.iPhone.MPEG4_480p, "mp4"),
         New PresetDescriptor(Preset.Video.iPod.H264_240p, "mp4"),
         New PresetDescriptor(Preset.Video.iPod.MPEG4_240p, "mp4"),
         New PresetDescriptor(Preset.Video.Generic.MP4.Base_H264_AAC, "mp4"),
         New PresetDescriptor(Preset.Video.AppleLiveStreaming.H264_480p, "ts"),
         New PresetDescriptor(Preset.Video.AppleLiveStreaming.H264_720p, "ts"),
         New PresetDescriptor(Preset.Video.AndroidPhone.H264_360p, "mp4"),
         New PresetDescriptor(Preset.Video.AndroidPhone.H264_720p, "mp4"),
         New PresetDescriptor(Preset.Video.AndroidTablet.H264_720p, "mpg"),
         New PresetDescriptor(Preset.Video.AndroidTablet.WebM_VP8_720p, "webm"),
         New PresetDescriptor(Preset.Video.VCD.NTSC, "mpg"),
         New PresetDescriptor(Preset.Video.VCD.PAL, "mpg"),
         New PresetDescriptor(Preset.Video.Generic.WebM.Base_VP8_Vorbis, "webm")
        }

        Private Shared Sub PrintPresets()
            Console.WriteLine("PRESETS")
            Console.WriteLine("------------------------")
            For Each preset In AvbPresets
                Console.WriteLine("{0,-30} .{1}", preset.Name, preset.FileExtension)
            Next preset
            Console.WriteLine()
        End Sub

        Private Shared Function GetPresetByName(ByVal presetName As String) As PresetDescriptor
            For Each preset In AvbPresets
                If preset.Name.Equals(presetName, StringComparison.InvariantCultureIgnoreCase) Then
                    Return preset
                End If
            Next preset
            Return Nothing
        End Function
    End Class

    Friend Class PresetDescriptor
        Public Sub New(ByVal name As String, ByVal fileExtension As String)
            Me.Name = name
            Me.FileExtension = fileExtension
        End Sub

        Public Name As String
        Public FileExtension As String
    End Class
End Namespace
