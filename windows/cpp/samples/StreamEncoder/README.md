## StreamEncoder

The StreamEncoder sample shows how to use the MediaSocket Stream API instead of standard file i/o for input and output. The samples takes raw YUV video stream for input and outputs a compressed video stream. The output is configured with an AVBlocks preset.

### Command Line

	StreamEncoder --frame <width>x<height> --rate <fps> --color <COLOR> --input <file> --output <file> 
                  [--preset <PRESET>] 
	              [--colors] 
	              [--presets]

	COLORS:
	-------
	yv12                Planar Y, V, U (4:2:0) (note V,U order!)
	nv12                Planar Y, merged U->V (4:2:0)
	yuy2                Composite Y->U->Y->V (4:2:2)
	uyvy                Composite U->Y->V->Y (4:2:2)
	yuv411              Planar Y, U, V (4:1:1)
	yuv420              Planar Y, U, V (4:2:0)
	yuv422              Planar Y, U, V (4:2:2)
	yuv444              Planar Y, U, V (4:4:4)
	y411                Composite Y, U, V (4:1:1)
	y41p                Composite Y, U, V (4:1:1)
	bgr32               Composite B->G->R
	bgra32              Composite B->G->R->A
	bgr24               Composite B->G->R
	bgr565              Composite B->G->R, 5 bit per B & R, 6 bit per G
	bgr555              Composite B->G->R->A, 5 bit per component, 1 bit per A
	bgr444              Composite B->G->R->A, 4 bit per component
	gray                Luminance component only
	yuv420a             Planar Y, U, V, Alpha (4:2:0)
	yuv422a             Planar Y, U, V, Alpha (4:2:2)
	yuv444a             Planar Y, U, V, Alpha (4:4:4)
	yvu9                Planar Y, V, U, 9 bits per sample

	
	PRESETS (default extension):
	----------------------------
	dvd.pal.4x3.mp2                               (.mpg)
	dvd.pal.16x9.mp2                              (.mpg)
	dvd.ntsc.4x3.mp2                              (.mpg)
	dvd.ntsc.16x9.mp2                             (.mpg)
	appletv.h264.480p                             (.mp4)
	appletv.h264.720p                             (.mp4)
	appletv.mpeg4.480p                            (.mp4)
	wifi.h264.640x480.30p.1200k.aac.96k           (.ts)
	wifi.wide.h264.1280x720.30p.4500k.aac.128k    (.ts)
	mp4.h264.aac                                  (.mp4)
	ipad.mp4.h264.576p                            (.mp4)
	ipad.mp4.h264.720p                            (.mp4)
	ipad.mp4.mpeg4.480p                           (.mp4)
	iphone.mp4.h264.480p                          (.mp4)
	iphone.mp4.mpeg4.480p                         (.mp4)
	ipod.mp4.h264.240p                            (.mp4)
	ipod.mp4.mpeg4.240p                           (.mp4)
	android-phone.mp4.h264.360p                   (.mp4)
	android-phone.mp4.h264.720p                   (.mp4)
	android-tablet.mp4.h264.720p                  (.mp4)
	android-tablet.webm.vp8.720p                  (.webm)
	vcd.pal                                       (.mpg)
	vcd.ntsc                                      (.mpg)
	webm.vp8.vorbis                               (.webm)

###	Examples

The following command encodes a raw YUV video from `foreman_qcif.yuv` to a H.264 video in an MP4 container in `foreman_qcif.mp4`: 

	.\StreamEncoder.exe --frame 176x144 --rate 30 --color yuv420 --input ..\assets\vid\foreman_qcif.yuv --output foreman_qcif.mp4 --preset mp4.h264.aac

alternative using short option names:

	.\StreamEncoder.exe -f 176x144 -r 30 -c yuv420 -i ..\assets\vid\foreman_qcif.yuv -o foreman_qcif.mp4 -p mp4.h264.aac

and using default options:

	.\StreamEncoder.exe
***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib> .\StreamEncoder.exe -f 176x144 -r 30 -c yuv420 -i ..\assets\vid\foreman_qcif.yuv -o foreman_qcif.mp4 -p mp4.h264.aac

	Open Transcoder: Success
	Run Transcoder: Success
	created video: foreman_qcif.mp4

	PS AVBlocksSDK\lib>	
