## overlay_mp4_png_file

The overlay_mp4_png_file sample shows how to place a watermark on a video file by blending a video file with a PNG image.

### Command Line

~~~ shell
overlay_mp4_png_file -i <input video file> -w <PNG file> -p <x>:<y> -a <transparency> -o <output video file>
overlay_mp4_png_file --input <input video file> --watermark <PNG file> --position <x>:<y> --alpha <transparency> --output <output video file>
~~~

###	Examples

The following example blends a watermark `../assets/overlay/smile_icon.png` with input file `../assets/mov/big_buck_bunny_trailer_iphone.m4v` at top left position `50:50` with transparency `0.5` into output file `overlay_mp4_png_file.m4v`:

~~~ shell
./overlay_mp4_png_file --input ../assets/mov/big_buck_bunny_trailer_iphone.m4v --watermark ../assets/overlay/smile_icon.png --position 50:50 --alpha 50 --output overlay_mp4_png_file.m4v
~~~

and using default options:
~~~ shell
./overlay_mp4_png_file
~~~
***
~~~ shell
mac:~/AVBlocksSDK.macosx/lib $ ./overlay_mp4_png_file
Using defaults:
 --input ../assets/mov/big_buck_bunny_trailer_iphone.m4v --watermark ../assets/overlay/smile_icon.png --position 50:50 --alpha 0.5 --output overlay_mp4_png_file.m4v

Output file: overlay_mp4_png_file.m4v
mac:~/AVBlocksSDK.macosx/lib $
~~~ 