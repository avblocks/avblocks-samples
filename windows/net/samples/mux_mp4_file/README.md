## mux_mp4_file

The mux_mp4_file sample shows how to multiplex MP4 files with one stream AAC or H264 into a MP4 container.

### Command Line

	mux_mp4_file --audio <AAC_file>.mp4 --video <H264_file>.mp4 --output <output>.mp4

###	Examples

The following command muxes the MP4 files `big_buck_bunny_trailer_iphone.aud.mp4` and `big_buck_bunny_trailer_iphone.vid.mp4` into a MP4 file `mux_mp4_file.mp4`: 

	.\mux_mp4_file.x64.exe --audio ..\assets\aud\big_buck_bunny_trailer_iphone.aud.mp4 --video ..\assets\aud\big_buck_bunny_trailer_iphone.vid.mp4 --output mux_mp4_file.mp4 

or using the default options:
	
	.\mux_mp4_file.x64.exe

***

	Windows PowerShell
	Copyright (C) 2016 Microsoft Corporation. All rights reserved.
	
	PS AVBlocks.NET\lib> .\mux_mp4_file.x64.exe
	Using default options:
	 --audio ..\assets\aud\big_buck_bunny_trailer_iphone.aud.mp4 --video ..\assets\mov\big_buck_bunny_trailer_iphone.vid.mp4 --output mux_mp4_file.mp4
	Audio files: ..\assets\aud\big_buck_bunny_trailer_iphone.aud.mp4
	Video files: ..\assets\mov\big_buck_bunny_trailer_iphone.vid.mp4
	Output file: D:\Work\avblocksroot\AVBlocks\AVBlocks.NET\AVBlocksSDK\lib\mux_mp4_file.mp4
	Muxing audio input: ..\assets\aud\big_buck_bunny_trailer_iphone.aud.mp4
	Muxing video input: ..\assets\mov\big_buck_bunny_trailer_iphone.vid.mp4
	Output file: mux_mp4_file.mp4
	PS AVBlocks.NET\lib>