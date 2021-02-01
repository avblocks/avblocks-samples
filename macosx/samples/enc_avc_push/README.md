## enc_avc_push

The enc_avc_push sample shows how to convert a raw YUV video file to a compressed H.264 video file with Transcoder::push().  

### Command Line

	enc_avc_file --frame <width>x<height> --rate <fps> --color <COLOR> --input <file.yuv> --output <file.h264>
	[--colors]
	[--help]
 
###	Examples

The following command encodes a raw YUV video from `foreman_qcif.yuv` to a H.264 video in `foreman_qcif.h264`:
	
	./enc_avc_push --frame 176x144 --rate 30 --color yuv420 --input ../assets/vid/foreman_qcif.yuv --output foreman_qcif.h264

and using default option:
	
	./enc_avc_push

***

	mac:~/AVBlocksSDK.macosx/lib$ ./enc_avc_push
	
	Using defaults:
	enc_avc_push --input ../assets/vid/foreman_qcif.yuv --output foreman_qcif.h264 --rate 30 --frame 176x144 --color yuv420
	Transcoder open: Success
	Transcoder flush: Success
	Transcoder close: Success

	mac:~/AVBlocksSDK.macosx/lib$ 