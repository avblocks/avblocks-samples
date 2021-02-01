## demux_mp4_avc_aac_file

The demux_mp4_file sample shows how to extract the audio and video elementary streams from MP4 container and save each of them into a separate AAC and H264 file.   

### Command Line

demux_mp4_avc_aac_file  --input <input_mp4_file> --output <output_mp4_file_name_without_extension>

demux_mp4_avc_aac_file -i <input_mp4_file> -o <output_mp4_file_name_without_extension>


###	Examples

The following example extract audio and video elementary streams from input file `../assets/mov/big_buck_bunny_trailer_iphone.m4v` into ouput files starting with `big_buck_bunny_trailer_iphone`:

./demux_mp4_avc_aac_file --input ../assets/mov/big_buck_bunny_trailer_iphone.m4v --output big_buck_bunny_trailer_iphone 

or using the default options:

./demux_mp4_avc_aac_file

***

mac:~/AVBlocksSDK\lib> ./demux_mp4_avc_aac_file
Using defaults:
--input ../assets/mov/big_buck_bunny_trailer_iphone.m4v --output demux_mp4_avc_aac_file
Output file: demux_mp4_avc_aac_file.001.h264
Output file: demux_mp4_avc_aac_file.001.aac

mac:~/AVBlocksSDK/lib>