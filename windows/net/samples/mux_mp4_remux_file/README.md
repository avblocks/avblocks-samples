## mux_mp4_remux_file

The mux_mp4_remux_file sample shows how to remux existing mp4 to a fast-start mp4.

### Command Line
	
	mux_mp4_remux_file.x64.exe --input <file.mp4> --output <output.mp4> [--fast-start]


###	Examples

The following command remuxes the MP4 file `big_buck_bunny_trailer_iphone.m4v` into a fast-start MP4 file `big_buck_bunny_trailer_fast_start.mp4`: 

	.\mux_mp4_remux_file.x64.exe --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output big_buck_bunny_trailer_fast_start.mp4 --fast-start 

or using the default options:
	
	.\mux_mp4_remux_file.x64.exe

***

	Windows PowerShell
	Copyright (C) 2015 Microsoft Corporation. All rights reserved.
	
	PS AVBlocks.NET\lib> .\mux_mp4_remux_file.x64.exe --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output big_buck_bunny_trailer_fast_start.mp4 --fast-start
	Input file: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	Output file: mux_mp4_remux_file.mp4
	Fast-start: yes
	Muxing video input: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	Muxing audio input: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	Open Transcoder: Success
	Run Transcoder: Success
	PS AVBlocks.NET\lib>