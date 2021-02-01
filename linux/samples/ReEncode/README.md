## ReEncode

This sample takes an MP4 input and re-encodes the audio and video streams back into MP4 output. It shows how to force encoding of individual streams even when it is not needed. 

### Command Line

	./ReEncode.app [--input inputFile.mp4] [--output outputFile.mp4] [--reEncodeAudio yes|no] [--reEncodeVideo yes|no]


The following example re-encodes only the video stream in an MP4 / H.264 clip.

	./ReEncode.app --reEncodeAudio no --reEncodeVideo yes --input ../assets/mov/big_buck_bunny_trailer_iphone.m4v --output reencode_output.mp4

or using the default options:

	.\ReEncode.app
***
	
	ubuntu:~/AVBlocksSDK.linux/lib$ ./ReEncode.app --reEncodeAudio no --reEncodeVideo yes --input ../assets/mov/big_buck_bunny_trailer_iphone.m4v --output reencode_output.mp4
	Input file: ../assets/mov/big_buck_bunny_trailer_iphone.m4v
	Output file: reencode_output.mp4
	Re-encode audio forced: no
	Re-encode video forced: yes
	Load Info: Success
	Open Transcoder: Success
	Run Transcoder: Success
