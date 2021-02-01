## dump_avc_au

The dump_avc_au sample shows how to split an H.264 (AVC) elementary stream to access units (AU). The sample accepts an input H.264 file and an output folder. The sample reads the input file and writes the H.264 access units to the output folder, each AU is written in a separate file. It also displays the NAL units within each access unit.        

### Command Line

	dump_avc_au --input <h264-file> --output <folder>

	dump_avc_au -i <h264-file> -o <folder>
 
###	Examples

By default the sample uses ../assets/vid/foreman_qcif.h264 as input and the <exedir>/foreman_qcif.h264.au folder as output.

The following command extracts the H.264 access units from the `foreman_qcif.h264` video and writes them to the folder `foreman_qcif.h264.au` as separate files (`au_####.h264`):
	
	./dump_avc_au --input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.h264.au

and using the default options:

	dump_avc_au
***

	mac:~/AVBlocksSDK.macosx/lib $ ./dump_avc_au

	Using defaults:
	--input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.h264.au

	transcoder open: Success
	AU #0, 6165 bytes
	  SPS     : LOW
	  PPS     : LOW
	  IDR     : LOW
	AU #1, 1848 bytes
	  AUD     : DISPOSABLE
	  SLICE   : LOW
	AU #2, 2019 bytes
	  AUD     : DISPOSABLE
	  SLICE   : LOW
	...
	AU #299, 1434 bytes
	AUD     : DISPOSABLE
	SLICE   : LOW

    Successfully parsed input file: ../assets/vid/foreman_qcif.h264
    Output directory: foreman_qcif.h264.au

