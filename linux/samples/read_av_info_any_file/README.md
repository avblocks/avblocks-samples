## read_av_info_any_file

The read_av_info_any_file sample shows how to use the AVBlocks MediaInfo API to extract audio / video stream information from a media file.   

### Command Line

	./read_av_info_any_file.app -i <file>
 
###	Examples

The following example lists the audio and video streams of the `big_buck_bunny_trailer_iphone.m4v` movie trailer:
	
	./read_av_info_any_file.app -i ../assets/mov/big_buck_bunny_trailer_iphone.m4v

and using default option:
    
    ./read_av_info_any_file.app
    
***
    	ubuntu-32:~/AVBlocksSDK.linux/lib$ ./read_av_info_any_file.app
	Using defaults:
	 --input ../assets/mov/big_buck_bunny_trailer_iphone.m4v
	file: ../assets/mov/big_buck_bunny_trailer_iphone.m4v
	container: MP4
	streams: 2

	stream #0 Video
	type: H264, subtype: AVC1
	id: 1
	duration: 32.48
	frame size: 480x270
	display ratio: 16:9
	frame rate: 25
	color format: YUV420
	bitrate: 0, mode: Unknown
	scan type: Progressive
	frame bottom up: 0

	stream #1 Audio
	type: AAC, subtype: AAC_MP4
	id: 2
	duration: 33.042
	sample rate: 44100
	channels: 2
	bits per sample: 16
	bytes per frame: 0
	bitrate: 128000, mode: Unknown
	channel layout: 3
	flags: 0

	ubuntu:~/AVBlocksSDK.linux/lib$
