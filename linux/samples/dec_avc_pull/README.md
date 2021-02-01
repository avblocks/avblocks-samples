## dec_avc_pull

The dec_avc_pull sample shows how to use the AVBlocks `Transcoder::pull` to decode H264 compressed file to YUV uncompressed file. 

### Command Line

~~~ shell
dec_avc_pull --input <h264-file> --output <yuv-file>

dec_avc_pull -i <h264-file> -o <yuv-file>
~~~
###	Examples


~~~ shell	
./dec_avc_pull.app --input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.h264.yuv
~~~

and using the default options:

~~~ shell
./dec_avc_pull.app
~~~
***
~~~ shell
ubuntu:~/AVBlocksSDK.ubuntu/lib $ ./dec_avc_pull64.app
Using defaults:
 --input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.yuv
Transcoder pull: End of stream, facility:9, error:9, hint:
Frames decoded: 300
Output file: foreman_qcif.yuv

ubuntu:~/AVBlocksSDK.ubuntu/lib $
~~~
	

### Build using CMake

#### Build from command line 

for 32-bit build: 
~~~ shell
# cd to samples/dec_avc_pull
mkdir build32
cd build32
cmake -DCMAKE_BUILD_TYPE=Release32 ..
make
~~~
for 64-bit build:
~~~ shell
# cd to samples/dec_avc_pull
mkdir build64
cd build64
cmake -DCMAKE_BUILD_TYPE=Release64 ..
make
~~~
The output executable 'dec_avc_pull.app' / 'dec_avc_pull64.app' goes in the SDK/lib dir.

#### Build from CLion IDE
The CMake project can be opened directly in the CLion IDE.
It contains 2 targets (dec_avc_pull.app and dec_avc_pull64.app) with Debug and Release configurations for each target.


