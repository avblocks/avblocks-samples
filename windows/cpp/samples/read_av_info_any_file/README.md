## read_av_info_any_file

The read_av_info_any_file sample shows how to use the AVBlocks MediaInfo API to extract audio / video stream information from a media file.   

### Command Line
	
	read_av_info_any_file -i <file>
	read_av_info_any_file --input <file>
 
###	Examples

The following example lists the audio and video streams of the `big_buck_bunny_trailer_iphone.m4v` movie trailer:
	
	.\read_av_info_any_file.exe --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v

and using default option:
	
	.\read_av_info_any_file.exe

***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib> .\read_av_info_any_file.exe --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	file: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	container: MP4
	streams: 2

	stream #0 Video
	type: H264, subtype: AVC1
	id: 1
	duration: 32.48
	bitrate: 0, mode: Unknown
	color format: YUV420
	display ratio: 16:9
	frame bottom up: 0
	frame size: 480x270
	frame rate: 25
	scan type: Progressive
	
	stream #1 Audio
	type: AAC, subtype: AAC_MP4
	id: 2
	duration: 33.042
	bitrate: 128000, mode: Unknown
	bits per sample: 16
	bytes per frame: 0
	channel layout: 3
	channels: 2
	flags: 0
	sample rate: 44100
	
	PS AVBlocksSDK\lib>
