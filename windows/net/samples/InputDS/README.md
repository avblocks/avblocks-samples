## InputDS

The InputDS sample uses DirectShow for decoding and the AVBlocks Transcoder API for encoding. The input is any format supported by DirectShow. The output is configured with an AVBlocks preset.

### Command Line

	InputDS --input inputFile --output outputFile --preset encodingPreset

	PRESETS
	----------------------------
	dvd.ntsc.16x9.mp2              .mpg
	dvd.ntsc.16x9.pcm              .mpg
	dvd.ntsc.4x3.mp2               .mpg
	dvd.ntsc.4x3.pcm               .mpg
	dvd.pal.16x9.mp2               .mpg
	dvd.pal.4x3.mp2                .mpg
	ipad.mp4.h264.576p             .mp4
	ipad.mp4.h264.720p             .mp4
	ipad.mp4.mpeg4.480p            .mp4
	iphone.mp4.h264.480p           .mp4
	iphone.mp4.mpeg4.480p          .mp4
	ipod.mp4.h264.240p             .mp4
	ipod.mp4.mpeg4.240p            .mp4
	apple.ts.h264.480p             .ts
	apple.ts.h264.720p             .ts
	mp4.h264.aac                   .mp4
	android-phone.mp4.h264.360p    .mp4
	android-phone.mp4.h264.720p    .mp4
	android-tablet.mp4.h264.720p   .mpg
	android-tablet.webm.vp8.720p   .webm
	vcd.ntsc                       .mpg
	vcd.pal                        .mpg
	webm.vp8.vorbis                .webm

###	Examples

The following example encodes AVI video (DV + LPCM) to MP4 video (H.264 + AAC):

	.\InputDS.x86.exe --input ..\assets\mov\bars_100.avi --output bars_100.mp4 --preset ipad.mp4.h264.720p

***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.

	PS AVBlocksSDK\lib> .\InputDS.x86.exe --input ..\assets\mov\bars_100.avi --output bars_100.mp4 --preset ipad.mp4.h264.720p
	Initializing DirectShow graph.
	Open Transcoder: Success
	Running DirectShow graph.
	DirectShow graph is stopped.
	Closing transcoder.
	
	PS AVBlocksSDK\lib>