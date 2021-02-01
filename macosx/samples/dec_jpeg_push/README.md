## dec_jpeg_push

The dec_jpeg_push sample shows how to use the `Transcoder::push` method to decode a jpeg image to a YUV frame.   

### Command Line

~~~ shell
dec_jpeg_push --input <file.jpeg> --frame <width>x<height> --output <file.yuv>
~~~
 
###	Examples

The following example decodes a 640x480 jpeg image to an YUV frame:
	
~~~ shell	
./dec_jpeg_push --input ../assets/img/cube0000.jpeg --frame 640x480 --output cube0000.yuv
~~~

or using the default options:   
	
~~~ shell	
./dec_jpeg_push
~~~

***

~~~ shell
mac:~/AVBlocksSDK.macosx/lib $ ./dec_jpeg_push
Using defaults:
--input ../assets/img/cube0000.jpeg --frame 640x480 --output cube0000.yuv

mac:~/AVBlocksSDK.macosx/lib $ 
~~~
