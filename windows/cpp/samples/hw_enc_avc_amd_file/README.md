## hw_enc_avc_amd_file

The hw_enc_avc_amd_file sample shows how to convert a raw YUV video file to a compressed H.264 video file using AMD VCE hardware acceleration. 

### Command Line

	hw_enc_avc_amd_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h264>
	[--colors]
	[--help]
 
###	Examples

The following command encodes a raw YUV video from `foreman_qcif.yuv` to a H.264 video in `foreman_qcif.h264`:
	
	.\hw_enc_avc_amd_file.exe --frame 176x144 --rate 30 --color yuv420 --input ..\assets\vid\foreman_qcif.yuv --output foreman_qcif.h264

and using default option:
	
	.\hw_enc_avc_amd_file.exe

***

	Windows PowerShell
	Copyright (C) 2016 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib> .\hw_enc_avc_intel_file.exe 

	Using defaults:
	hw_enc_avc_amd_file --input ..\assets\vid\foreman_qcif.yuv --output foreman_qcif.h264 --rate 30 --frame 176x144 --color yuv420
	Transcoder open: Success
	Transcoder run: Success
	Transcoder close: Success

	PS AVBlocksSDK\lib>
