## dec_avc_file

The dec_avc_file sample shows how to use the AVBlocks `Transcoder::run()` to decode H264 compressed file to YUV uncompressed file.   

### Command Line

~~~ shell
dec_avc_file -i <h264_file> -o <yuv_file>
dec_avc_file --input <file> --output <yuv_file>
~~~

###	Examples

The following example decode input file `../assets/vid/foreman_qcif.h264` into ouput file `foreman_qcif.yuv`:

~~~ shell
./dec_avc_file --input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.yuv
~~~

and using default options:
~~~ shell
./dec_avc_file
~~~
***
~~~ shell
mac:~/AVBlocksSDK.macosx/lib $ ./dec_avc_file
Using defaults:
 --input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.yuv
Output: foreman_qcif.yuv

mac:~/AVBlocksSDK.macosx/lib $
~~~ 