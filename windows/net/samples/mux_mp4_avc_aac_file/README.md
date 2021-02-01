## mux_mp4_avc_aac_file

The mux_mp4_avc_aac_file sample shows how to multiplex AAC audio and H.264 video into a MP4 container. The sample can:

* Mux audio and video into mp4 without fast-start
* Mux audio and video into mp4 with fast-start
* Remux existing mp4 to a fast-start mp4


### Command Line

	.\mux_mp4_avc_aac_file.x64.exe --input <input audio file>.aac <input video file>.h264 --output <output file>.mp4 [--fast-start]
	
	.\mux_mp4_avc_aac_file.x64.exe --input <input file>.mp4 --output <output file>.mp4 [--fast-start]


###	Examples

The following command remuxes the MP4 file `big_buck_bunny_trailer_iphone.m4v` into a fast-start MP4 file `big_buck_bunny_trailer_fast_start.mp4`: 

	.\mux_mp4_avc_aac_file.x64.exe --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output big_buck_bunny_trailer_fast_start.mp4 --fast-start 


***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.
	
	PS AVBlocks.NET\lib> .\mux_mp4_avc_aac_file.x64.exe --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output big_buck_bunny_trailer_fast_start.mp4 --fast-start
	Muxing video input: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	Muxing audio input: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	Open Transcoder: Success
	Run Transcoder: Success
	PS AVBlocks.NET\lib>