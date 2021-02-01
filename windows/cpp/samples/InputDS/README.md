## InputDS

The InputDS sample uses DirectShow for decoding and the AVBlocks Transcoder API for encoding. The input is any format supported by DirectShow. The output is configured with an AVBlocks preset.

### Command Line

	InputDS --preset <encodingPreset> --input <inputFile> --output <outputFile>

	PRESETS
	-------------------------------------------------------
	dvd.pal.4x3.mp2                                    .mpg
	dvd.pal.16x9.mp2                                   .mpg
	dvd.ntsc.4x3.mp2                                   .mpg
	dvd.ntsc.16x9.mp2                                  .mpg
	appletv.h264.480p                                  .mp4
	appletv.h264.720p                                  .mp4
	appletv.mpeg4.480p                                 .mp4
	wifi.h264.640x480.30p.1200k.aac.96k                .ts
	wifi.wide.h264.1280x720.30p.4500k.aac.128k         .ts
	mp4.h264.aac                                       .mp4
	ipad.mp4.h264.576p                                 .mp4
	ipad.mp4.h264.720p                                 .mp4
	ipad.mp4.mpeg4.480p                                .mp4
	iphone.mp4.h264.480p                               .mp4
	iphone.mp4.mpeg4.480p                              .mp4
	ipod.mp4.h264.240p                                 .mp4
	ipod.mp4.mpeg4.240p                                .mp4
	android-phone.mp4.h264.360p                        .mp4
	android-phone.mp4.h264.720p                        .mp4
	android-tablet.mp4.h264.720p                       .mp4
	android-tablet.webm.vp8.720p                       .webm
	vcd.pal                                            .mpg
	vcd.ntsc                                           .mpg
	webm.vp8.vorbis                                    .webm

###	Examples

The following example encodes AVI video (DV + LPCM) to MP4 video (H.264 + AAC):

	.\InputDS.exe --preset mp4.h264.aac --input ..\assets\mov\bars_100.avi --output bars_100.mp4

or using the default options:
	
	.\InputDS.exe

***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.

	PS AVBlocksSDK\lib> .\InputDS.exe --preset mp4.h264.aac --input ..\assets\mov\bars_100.avi --output bars_100.mp4
	Input file: ..\assets\mov\bars_100.avi
	Output file: bars_100.mp4
	Preset: mp4.h264.aac
	Initializing DirectShow graph.
	Open Transcoder:Success
	Running DirectShow graph.
	DirectShow graph is stopped.
	Closing transcoder.
	PS AVBlocksSDK\lib>