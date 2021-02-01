## dec_avc_push

The dec_avc_push sample shows how to use the `Transcoder.Push` method to decode Access Units (AU) to a YUV file.   

### Command Line

	dec_avc_push.x64.exe --input <directory> [--output <file>]

	dec_avc_push.x64.exe -i <directory> [-o <file>]


###	Examples

The following command extracts the H.264 Access Unit files from the `foreman_qcif.h264.au\` directory and decodes them to raw video frames. By default, the sample saves the decoded raw video to `decoded_WxH.yuv`:
	
	.\dec_avc_push.x64.exe --input ..\assets\vid\foreman_qcif.h264.au\ --output decoded_file.yuv

and using the default options:

	.\dec_avc_push.x64.exe
***

	Windows PowerShell
	Copyright (C) 2015 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib>.\dec_avc_push.x64.exe --input ..\assets\vid\foreman_qcif.h264.au\ --output decoded_file.yuv
	Decoded 300 files.(last decoded file: au_0299.h264)
    Output file: decoded_file.yuv

	PS AVBlocksSDK\lib>