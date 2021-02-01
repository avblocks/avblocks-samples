## dec_jpeg_push

The dec_jpeg_push sample shows how to use the `Transcoder::push` method to decode a jpeg image to a YUV frame.

### Command Line

~~~ shell
dec_jpeg_push --input <input_file_jpeg> --frame <image_width>x<image_height> --output <output_file_yuv>
~~~
 
###	Examples

The following example decodes a 640x480 jpeg image to an YUV frame:
	
~~~ shell	
./dec_jpeg_push.app --input ../assets/img/cube0000.jpeg --frame 640x480 --output cube0000.yuv
~~~

or using the default options:
	
~~~ shell	
./dec_jpeg_push.app
~~~

***

~~~ shell
ubuntu:~/AVBlocksSDK.linux/lib$ ./dec_jpeg_push.app 
Using defaults:
 --input ../assets/img/cube0000.jpeg --frame 640x480 --output cube0000.yuv
    
ubuntu:~/AVBlocksSDK.linux/lib$ 
~~~
    
