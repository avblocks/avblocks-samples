## overlay_yuv_jpegs_push_pull

The overlay_yuv_jpegs_push_pull sample shows how to overlay a sequence of images when encoding video. 

### Command Line

	overlay_yuv_jpegs_push_pull --input <file.m4v> --output <file.mp4> --overlay_input <directory> --overlay_count <count>

###	Examples

The following command encodes the given images as overlay in to the provided video.  
	
	.\overlay_yuv_jpegs_push_pull.exe --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output overlay_yuv_jpeg.m4v 
	--overlay_input ..\assets\overlay\cube --overlay_count 250

or using the default option:

	.\overlay_yuv_jpegs_push_pull.exe
***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib> .\split_mp4_pull_push.exe --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output overlay_yuv_jpeg.m4v 
	--overlay_input ..\assets\overlay\cube --overlay_count 250
	decoder pull: End of stream, facility:9, error:9
	samples decoded/overlayed/encoded: 812/812/812
	output file: overlay_yuv_jpeg.m4v
