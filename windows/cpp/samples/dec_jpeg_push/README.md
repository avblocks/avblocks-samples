## dec_jpeg_push

The dec_jpeg_push sample shows how to use the `Transcoder::push` method to decode a jpeg image to a YUV frame.   

### Command Line

	dec_jpeg_push --input <input_file> --frame <image_width>x<image_height> --output <output_file>
 
###	Examples

The following example decodes a 640x480 jpeg image to an YUV frame:
	
	.\dec_jpeg_push.exe --input ..\assets\img\cube0000.jpeg --frame 640x480 --output cube0000.yuv

or using the default options:
	
	.\dec_jpeg_push.exe

***

	Windows PowerShell
	Copyright (C) 2015 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK.windows\lib> .\dec_jpeg_push.exe
 	Using defaults: 
	--input ..\assets\img\cube0000.jpeg --frame 640x480 --output cube0000.yuv
	
	PS AVBlocksSDK.windows\lib>