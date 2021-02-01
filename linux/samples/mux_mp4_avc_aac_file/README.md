## mux_mp4_avc_aac_file

The mux_mp4_avc_aac_file sample shows how to multiplex AAC audio and H.264 video into a MP4 container. The sample can:

* Mux audio and video into mp4 without fast-start
* Mux audio and video into mp4 with fast-start
* Remux existing mp4 to a fast-start mp4


### Command Line

	mux_mp4_avc_aac_file --input <file>.aac --input <file>.h264 --output output.mp4 [--fast-start]
	
	mux_mp4_avc_aac_file --input <file>.mp4 --output output.mp4 [--fast-start]


###	Examples

The following command remuxes the MP4 file `big_buck_bunny_trailer_iphone.m4v` into a fast-start MP4 file `big_buck_bunny_trailer_fast_start.mp4`: 

	./mux_mp4_avc_aac_file.app --input ../assets/mov/big_buck_bunny_trailer_iphone.m4v --output big_buck_bunny_trailer_fast_start.mp4 --fast-start 

or using the default options:

	./mux_mp4_avc_aac_file.app

***
    ubuntu:~/AVBlocksSDK.linux/lib$ ./mux_mp4_avc_aac_file.app --input ../assets/mov/big_buck_bunny_trailer_iphone.m4v --output big_buck_bunny_trailer_fast_start.mp4 --fast-start
	Muxing audio input: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	Muxing video input: ..\assets\mov\big_buck_bunny_trailer_iphone.m4v
	Open Transcoder: Success
	Run Transcoder: Success	

    ubuntu:~/AVBlocksSDK.linux/lib$
