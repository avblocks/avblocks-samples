## enc_avc_push

The enc_avc_push sample shows how to convert a raw YUV video file to a compressed H.264 video file with Transcoder.Push.   

### Command Line

	enc_avc_push --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h264>
	[--colors]
	[--help]
 
###	Examples

The following command encodes a raw YUV video from `foreman_qcif.yuv` to a H.264 video in `foreman_qcif.h264`:
	
	.\enc_avc_push.x86.exe --frame 176x144 --rate 30 --color yuv420 --input ..\assets\vid\foreman_qcif.yuv --output foreman_qcif.h264

and using default option:
	
	.\enc_avc_push.x86.exe

***

	Windows PowerShell
	Copyright (C) 2016 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib> .\enc_avc_push.x86.exe

	Using default options:
	enc_avc_push --input ..\assets\vid\foreman_qcif.yuv --output foreman_qcif.h264 --rate 30 --frame  --color yuv420
	Transcoder open: Success
	Transcoder flush: Success
	Transcoder close: Success

	PS AVBlocksSDK\lib>