## dec_avc_pull

The dec_avc_pull sample shows how to use the AVBlocks `Transcoder::pull` to decode H264 compressed file to YUV uncompressed file.    

### Command Line

~~~ shell
dec_avc_pull -i <h264_file> -o <yuv_file>
dec_avc_pull --input <file> --output <yuv_file>
~~~

###	Examples

The following example decode input file `../assets/vid/foreman_qcif.h264` into ouput file `foreman_qcif.yuv`:

~~~ shell
./dec_avc_pull --input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.yuv
~~~

and using default options:
~~~ shell
./dec_avc_pull
~~~
***
~~~ shell
mac:~/AVBlocksSDK.macosx/lib $ ./dec_avc_pull
Using defaults:
 --input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.yuv
Transcoder pull: End of stream, facility:9, error:9, hint:
Frames decoded: 300
Output file: foreman_qcif.yuv

mac:~/AVBlocksSDK.macosx/lib $
~~~ 
