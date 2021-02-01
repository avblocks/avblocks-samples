## dec_avc_file

The dec_avc_file sample shows how to use the AVBlocks `Transcoder.Run` method to decode H.264/AVC compressed file to YUV uncompressed file.   

### Command Line

~~~ shell
dec_avc_file -i <h264_file> -o <yuv_file>
dec_avc_file --input <h264_file> --output <yuv_file>
~~~

###	Examples

The following example decode input file `..\assets\vid\foreman_qcif.h264` into ouput file `foreman_qcif.yuv`:

~~~ shell
.\dec_avc_file.x64.exe --input ..\assets\vid\foreman_qcif.h264 --output foreman_qcif.yuv
~~~

and using default options:
~~~ shell
.\dec_avc_file.x64.exe
~~~
***
~~~ shell
Windows PowerShell
Copyright (C) 2016 Microsoft Corporation. All rights reserved.

PS AVBlocksSDK\lib> .\dec_avc_file.x64.exe
Using default options:
 --input ..\assets\vid\foreman_qcif.h264 --output foreman_qcif.yuv
Transcoder open: Success
Transcoder run: Success
Transcoder close: Success

PS AVBlocksSDK\lib>
~~~ 