## read_av_info_any_file

The read_av_info_any_file sample shows how to use the AVBlocks MediaInfo API to extract audio / video stream information from a media file.   

### Command Line

	 .\read_av_info_any_file.x64.exe <file>
 
###	Examples

The following example lists the audio and video streams of the `big_buck_bunny_trailer_iphone.m4v` movie trailer:
	
	.\read_av_info_any_file.x64.exe ..\assets\mov\big_buck_bunny_trailer_iphone.m4v

and using the default option:
	
	.\read_av_info_any_file.x64.exe

***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.
	
	PS AVBlocks.NET\lib> .\read_av_info_any_file.x64.exe ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	file: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	container: Mp4
	streams: 2
	
	stream #0 Audio
	type: Aac
	subtype: AacMp4
	id: 0
	duration: 33.042
	bitrate: 128000 mode: Unknown
	bits per sample: 16
	bytes per frame: 0
	channel layout: 00000003
	channels: 2
	flags: 00000000
	sample rate: 44100
	
	stream #1 Video
	type: H264
	subtype: Avc1
	id: 0
	duration: 32.480
	bitrate: 0 mode: Unknown
	color format: YV12
	display ratio: 16:9
	frame bottom up: False
	frame size: 480x270
	frame rate: 25.000
	interlace type: Progressive
	
	PS AVBlocks.NET\lib>