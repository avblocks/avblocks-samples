## enc_mpg_dvd

The enc_mpg_dvd sample shows how to convert any (supported) video clip to an MPEG-2 PS suitable for DVD-Video authoring.

It also demonstrates automatic input splitting to chunks by time or by size specified in MB where MB is used in the SI sense (1,000,000 bytes). 

### Command Line
~~~ shell
enc_mpg_dvd --input <video-file> [--split-time <seconds>] [--split-size <MBs>]
~~~
 
###	Examples

The following command transcodes the input to MPEG-2 PS chunks roughly limited to 100 MB:

~~~ shell
.\enc_mpg_dvd.exe --input video.mp4 --split-size 100 
~~~

and using default options:

~~~ shell
.\enc_mpg_dvd.exe
~~~

***

~~~ shell
Windows PowerShell
Copyright (C) 2016 Microsoft Corporation. All rights reserved.

PS AVBlocksSDK\lib> .\enc_mpg_dvd.x64.exe
	
Using default options:
 --input ..\assets\mov\big_buck_bunny_trailer_iphone.m4v --split-size 1
encoding enc_mpg_dvd.001.mpg
encoding enc_mpg_dvd.002.mpg
encoding enc_mpg_dvd.003.mpg
encoding enc_mpg_dvd.004.mpg
encoding enc_mpg_dvd.005.mpg
encoding enc_mpg_dvd.006.mpg
encoding enc_mpg_dvd.007.mpg

PS AVBlocksSDK\lib>
~~~