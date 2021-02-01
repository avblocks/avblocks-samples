## ReEncode

This sample takes an MP4 input and re-encodes the audio and video streams back into MP4 output. It shows how to force encoding of individual streams even when it is not needed. 

### Command Line

	.\ReEncode.x64.exe [--input inputFile.mp4] [--output outputFile.mp4] [--reEncodeAudio yes|no] [--reEncodeVideo yes|no]


The following example re-encodes only the video stream in an MP4 / H.264 clip.

	.\ReEncode.x64.exe --reEncodeAudio no --reEncodeVideo yes --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output reencode_output.mp4

***

	PS AVBlocks.NET\lib> .\ReEncode.x64.exe --reEncodeAudio no --reEncodeVideo yes --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output reencode_output.mp4
	Input file: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	Output file: reencode_output.mp4
	Re-encode audio forced: no
	Re-encode video forced: yes
	Open Transcoder: Success
	Run Transcoder: Success
	PS AVBlocks.NET\lib>
	
