## dec_avc_au

The dec_avc_au sample shows how to decode a H.264 stream. The sample uses sequence of files to simulate a stream of H.264 Access Units and a Transcoder object to decode the H.264 Access Units to raw YUV video frames.     

### Command Line

	./dec_avc_au --input <directory> [--output <file>] [--frame <width>x<height>] [--rate <fps>] [--color <COLOR>] [--colors]
 
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

###	Examples

The following command extracts the H.264 Access Unit files from the `foreman_qcif.h264.au\` directory and decodes them to raw video frames. By default, the sample saves the decoded raw video to `decoded_WxH.yuv`:
	
	./dec_avc_au --input ../assets/vid/foreman_qcif.h264.au/ --color yuv420

or using the default options:   

	./dec_avc_au
	
***

	mac:~/AVBlocksSDK.macosx/lib$ ./dec_avc_au --input ../assets/vid/foreman_qcif.h264.au/ --color yuv420
	Output file: ~/AVBlocksSDK.linux/lib/decoded_176x144.yuv
	mac:~/AVBlocksSDK.macosx/lib$
	    