## Slideshow

The Slideshow sample shows how to make a video clip from a sequence of images. The sample input is a series of JPEG images. The output is configured with an AVBlocks preset.

### Command Line

	.\Slideshow.VB.clr4.x64.exe  [-p PRESET]

###	Encoding Presets

	dvd.pal.4x3.mp2                .mpg
	dvd.pal.16x9.mp2               .mpg
	dvd.ntsc.4x3.mp2               .mpg
	dvd.ntsc.16x9.mp2              .mpg
	appletv.h264.480p              .mp4
	appletv.h264.720p              .mp4
	appletv.mpeg4.480p             .mp4
	apple.ts.h264.480p             .ts
	apple.ts.h264.720p             .ts
	mp4.h264.aac                   .mp4
	ipad.mp4.h264.576p             .mp4
	ipad.mp4.h264.720p             .mp4
	ipad.mp4.mpeg4.480p            .mp4
	iphone.mp4.h264.480p           .mp4
	iphone.mp4.mpeg4.480p          .mp4
	ipod.mp4.h264.240p             .mp4
	ipod.mp4.mpeg4.240p            .mp4
	android-phone.mp4.h264.360p    .mp4
	android-phone.mp4.h264.720p    .mp4
	android-tablet.mp4.h264.720p   .mp4
	android-tablet.webm.vp8.720p   .webm
	vcd.pal                        .mpg
	vcd.ntsc                       .mpg
	webm.vp8.vorbis                .webm

###	Examples

The following example encodes an MP4 / H.264 clip from a sequence of images in the `assets/img` folder:

	.\Slideshow.VB.clr4.x64.exe -p mp4.h264.aac

***

	PS AVBlocks.NET\lib> .\Slideshow.VB.clr4.x64.exe -p mp4.h264.aac
	Load Info: Success
	Open Transcoder: Success
	Flush Transcoder: Success
	output video: "cube.mp4"
	PS AVBlocks.NET\lib>

