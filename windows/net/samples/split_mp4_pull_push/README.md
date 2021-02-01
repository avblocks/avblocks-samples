## split_mp4_pull_push

The split_mp4_pull_push sample shows how to split a file to short clips. This sample takes an MP4 file (H.264 / AAC) file for input and splits it to 10 second MPEG-2 PS clips as an output.

### Command Line

	split_mp4_pull_push.x64.exe <inputFile>

###	Examples

The following command splits the file `avsync_2min.mp4` into short clips.  

	.\split_mp4_pull_push.x64.exe ..\assets\mov\avsync_2min.mp4
***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib> .\split_mp4_pull_push.x64.exe ..\assets\mov\avsync_2min.mp4

	avsync_2min_001.mpg start: 00:10.000 end: 00:10.000 act. start: 00:00.000 act. end: 00:10.010
	avsync_2min_002.mpg start: 00:10.000 end: 00:20.000 act. start: 00:10.010 act. end: 00:20.020
	avsync_2min_003.mpg start: 00:20.000 end: 00:30.000 act. start: 00:20.020 act. end: 00:30.030
	avsync_2min_004.mpg start: 00:30.000 end: 00:40.000 act. start: 00:30.030 act. end: 00:40.007
	avsync_2min_005.mpg start: 00:40.000 end: 00:50.000 act. start: 00:40.007 act. end: 00:50.017
	avsync_2min_006.mpg start: 00:50.000 end: 01:00.000 act. start: 00:50.017 act. end: 01:00.027
	avsync_2min_007.mpg start: 01:00.000 end: 01:10.000 act. start: 01:00.027 act. end: 01:10.003
	avsync_2min_008.mpg start: 01:10.000 end: 01:20.000 act. start: 01:10.003 act. end: 01:20.013
	avsync_2min_009.mpg start: 01:20.000 end: 01:30.000 act. start: 01:20.013 act. end: 01:30.023
	avsync_2min_010.mpg start: 01:30.000 end: 01:40.000 act. start: 01:30.023 act. end: 01:39.000
	avsync_2min_011.mpg start: 01:40.000 end: 01:50.000 act. start: 01:39.000 act. end: 01:50.010
	avsync_2min_012.mpg start: 01:50.000 end: 02:00.000 act. start: 01:50.010 act. end: 02:00.020
	avsync_2min_013.mpg start: 02:00.000 end: 02:00.094 act. start: 02:00.020 act. end: 02:00.094

	PS AVBlocksSDK\lib>
