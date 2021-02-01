## dec_avc_file

   

### Command Line

~~~ shell
dec_avc_file --input <h264-file> --output <yuv-file>

dec_avc_file -i <h264-file> -o <yuv-file>
~~~
###	Examples


~~~ shell	
./dec_avc_file.app --input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.h264.au
~~~

and using the default options:

~~~ shell
./dec_avc_file.app
~~~
***
~~~ shell
ubuntu:~/AVBlocksSDK.ubuntu/lib $ ./dec_avc_file64.app
Using defaults:
 --input ../assets/vid/foreman_qcif.h264 --output foreman_qcif.yuv
Output: foreman_qcif.yuv

ubuntu:~/AVBlocksSDK.ubuntu/lib $
~~~
	

### Build using CMake

#### Build from command line 

for 32-bit build: 
~~~ shell
# cd to samples/dec_avc_file
mkdir build32
cd build32
cmake -DCMAKE_BUILD_TYPE=Release32 ..
make
~~~
for 64-bit build:
~~~ shell
# cd to samples/dec_avc_file
mkdir build64
cd build64
cmake -DCMAKE_BUILD_TYPE=Release64 ..
make
~~~
The output executable 'dec_avc_file.app' / 'dec_avc_file64.app' goes in the SDK/lib dir.

#### Build from CLion IDE
The CMake project can be opened directly in the CLion IDE.
It contains 2 targets (dec_avc_file.app and dec_avc_file64.app) with Debug and Release configurations for each target.


