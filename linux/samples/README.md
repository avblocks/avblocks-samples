# Samples
***

## dec_avc_au

Decode a H.264 stream. The sample uses sequence of files to simulate a stream of H.264 Access Units and a Transcoder object to decode the H.264 Access Units to raw YUV video frames.    

## dec_avc_push

Use the `Transcoder::push` method to decode Access Units (AU) to a YUV file.   

## dec_avc_file

Use the `Transcoder::run` method to decode a H264 compressed file to a YUV  uncompressed file. 

## dec_avc_pull

Use the `Transcoder::pull` method to decode H264 compressed file to YUV uncompressed file.

## dec_jpeg_push

Use the `Transcoder::push` method to decode a JPEG image to a YUV frame. 

## demux_mp4_avc_aac_file

Extract the audio and video elementary streams from MP4 container and save each of them into a separate AAC and H264 file.   

## dump_avc_au

Split an H.264 (AVC) elementary stream to access units (AU). The sample accepts an input H.264 file and an output folder. The sample reads the input file and writes the H.264 access units to the output folder, each AU is written in a separate file. It also displays the NAL units within each access unit.   

## enc_aac_adif_file

Encode WAV file to AAC file in Audio Data Interchange Format (ADIF) format.

## enc_aac_adts_pull

Encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format with Transcoder::pull.

## demux_mp4_file

Extract the first audio and video elementary streams from MP4 container and save each of them into a separate MP4 file. 

## enc_aac_adts_file

Encode WAV file to AAC file in Audio Data Transport Stream (ADTS) format.

## enc_aac_au_file

Encode WAV file to AAC file in raw data format.

## enc_aac_au_pull

Encode WAV file to AAC file in raw data format with `Transcoder::pull`.

## enc_avc_file

Use the `Transcoder::run` method to encode a raw YUV video file to a compressed H.264 video file. 

## enc_avc_push

Use the `Transcoder::push` method to encode a raw YUV video stream to compressed H.264 video file. 

## enc_mp4_avc_aac_push

Encode and multiplex uncompressed audio and video input files to produce a compressed MP4 file. The sample uses predefined input files.

## enc_mpg_dvd

Convert any (supported) video input to an MPEG-2 PS suitable for DVD-Video authoring.   

## enc_yuv_preset_file

Convert a raw YUV video file and to a compressed video file. The format of the output is configured with an AVBlocks preset.

## mux_mp4_avc_aac_file

Multiplex AAC audio and H.264 video into a MP4 container.

## mux_mp4_file

Multiplex MP4 files with one stream AAC or H264 into a MP4 container.

## mux_mp4_remux_file

Remultiplex existing MP4 file for streaming with or without Fast-Start options.

## overlay_mp4_png_file

Place a watermark on a video file by blending a video file with a PNG image.

## overlay_yuv_jpegs_push_pull

Overlay a sequence of images when encoding video. 

## read_av_info_any_file

Use the MediaInfo API to extract audio / video stream information from a media file.   

## read_metadata_any_file

Use the MediaInfo and Metadata APIs to extract metadata information from a media file.   

## ReEncode

Take an MP4 input and re-encode the audio and video streams back into MP4 output. The sample shows how to force encoding of individual streams even when it is not needed.

## Slideshow

Make a video clip from a sequence of images. The sample input is a series of JPEG images. The output is configured with an AVBLocks preset.

## split_mp4_pull_push

Split a file to short clips.

## StreamDecoder

Use the MediaSocket Stream API instead of standard file i/o for input and output. The sample takes a compressed raw H.264 video stream for input and outputs raw YUV video. The input and the output sockets are configured manually.

## StreamEncoder

Use the MediaSocket Stream API instead of standard file i/o for input and output. The samples takes raw YUV video stream for input and outputs a compressed video stream. The output is configured with an AVBlocks preset.
