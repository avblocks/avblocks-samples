## StreamDecoder

The StreamDecoder sample shows how to use the MediaSocket Stream API instead of standard file i/o for input and output. The sample takes a compressed raw H.264 video stream for input and outputs raw YUV video. The input and the output sockets are configured manually.

### Command Line

	StreamDecoder --input <file> [--streamtype <STREAM TYPE>] --output <file> 
					[--frame <width>x<height>] 
					[--rate <fps>] [--color <COLOR>] 
					[--colors] 
					[--streamtypes]

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
 
	STREAM TYPE (extension):
	----------------------------
	MP4                  (.mp4)
	MP4                  (.mov)
	MP4                  (.m4v)
	AVI                  (.avi)
	MPEG_PS              (.mpg)
	MPEG_TS              (.ts)
	MPEG2_Video          (.m2v)
	MPEG2_Video          (.mpv)
	ASF                  (.asf)
	ASF                  (.wmv)
	H264                 (.h264)
	WebM                 (.webm)

###	Examples

The following command decodes the H.264 elementary stream video from `foreman_qcif.h264` to a raw YUV/4:2:0 video in `foreman_qcif_176x144.yuv`: 
	
	.\StreamDecoder.exe --input ..\assets\vid\foreman_qcif.h264 --streamtype H264 --output foreman_qcif.yuv --color yuv420 
	
alternative using short option names:

	.\StreamDecoder.exe -i ..\assets\vid\foreman_qcif.h264 -s H264 -o foreman_qcif.yuv -c yuv420

and using default options:

	.\StreamDecoder.exe
***

	Windows PowerShell
	Copyright (C) 2012 Microsoft Corporation. All rights reserved.
	
	PS AVBlocksSDK\lib> .\StreamDecoder.exe -i ..\assets\vid\foreman_qcif.h264 -s H264 -o foreman_qcif.yuv -c yuv420
	
	Input file: ..\assets\vid\foreman_qcif.h264
	YUV output file: foreman_qcif_176x144.yuv
	Open Transcoder: Success
	Run Transcoder: Success
	
	PS AVBlocksSDK\lib>