## demux_mp4_file

The demux_mp4_file sample shows how to extract the first audio and video elementary streams from MP4 container and save each of them into a separate MP4 file.   

### Command Line

~~~ shell
demux_mp4_file -i <input_mp4_file> -o <output_mp4_file_name_without_extension>
demux_mp4_file --input <input_mp4_file> --output <output_mp4_file_name_without_extension>
~~~

###	Examples

The following example extract audio and video elementary streams from input file `..\assets\mov\big_buck_bunny_trailer_iphone.m4v` into output files starting with `big_buck_bunny_trailer_iphone`:

~~~ shell
.\demux_mp4_file.exe --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output big_buck_bunny_trailer_iphone
~~~

and using default options:
~~~ shell
.\demux_mp4_file.exe
~~~
***
~~~ shell
Windows PowerShell
Copyright (C) 2015 Microsoft Corporation. All rights reserved.

PS AVBlocksSDK\lib> .\demux_mp4_file.exe
Using defaults:
 --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --output big_buck_bunny_trailer_iphone
Output file: big_buck_bunny_trailer_iphone.vid.mp4
Output file: big_buck_bunny_trailer_iphone.aud.mp4

PS AVBlocksSDK\lib>
~~~ 