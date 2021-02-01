## dec_avc_pull

The dec_avc_pull sample shows how to use the AVBlocks `Transcoder.Pull` method to decode H.264/AVC compressed file to YUV uncompressed file.   

### Command Line

~~~ shell
dec_avc_pull.net -i <h264_file> -o <yuv_file>
dec_avc_pull.net --input <h264_file> --output <yuv_file>
~~~

###	Examples

The following example decode input file `..\assets\vid\foreman_qcif.h264` into ouput file `foreman_qcif.yuv`:

~~~ shell
.\dec_avc_pull.x64.exe --input ..\assets\vid\foreman_qcif.h264 --output foreman_qcif.yuv
~~~

and using default options:
~~~ shell
.\dec_avc_pull.x64.exe
~~~
***
~~~ shell
Windows PowerShell
Copyright (C) 2016 Microsoft Corporation. All rights reserved.

PS AVBlocksSDK\lib> .\dec_avc_pull.x64.exe
Using default options:
 --input ..\assets\vid\foreman_qcif.h264 --output foreman_qcif.yuv

Transcoder pull: End of stream, facility:Codec code:9 hint:
Frames decoded: 300
Output file: foreman_qcif.yuv

PS AVBlocksSDK\lib>
~~~ 