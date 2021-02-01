## dec_jpeg_push

The dec_jpeg_push sample shows how to use the `Transcoder.Push` method to decode a jpeg image to a YUV frame.   

### Command Line

	.\dec_jpeg_push.x64.exe --input <input_file> --output <output_file>
 
###	Examples

The following example decodes a 640x480 jpeg image to an YUV frame:
	
	.\dec_jpeg_push.x64.exe --input ..\assets\img\cube0000.jpeg --output cube0000.yuv

and using the default options:
	
	.\dec_jpeg_push.x64.exe

***

	Windows PowerShell
	Copyright (C) 2015 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib> .\dec_jpeg_push.x64.exe
	Using default options: --input ..\assets\img\cube0000.jpeg --output cube0000.yuv
	Input file: ..\assets\img\cube0000.jpeg
	Output file: cube0000.yuv
	Load MediaInfo: Success
	Input frame size: 640x480
	Open Transcoder: Success
	Push Transcoder: Success

	PS AVBlocksSDK\lib>