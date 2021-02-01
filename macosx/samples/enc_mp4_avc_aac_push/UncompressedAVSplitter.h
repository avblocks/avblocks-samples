#pragma once

#include <AVBlocks.h>

using namespace primo::codecs;
using namespace primo::avblocks;


class UncompressedAVSplitter
{
public:

	~UncompressedAVSplitter();
	UncompressedAVSplitter();
    /*
     @param socket  - uncompressed audio or video format (i.e. LPCM, YUV)
     @param filename - uncompressed audio or video file
     */
	void init(const MediaSocket* socket, const std::string& filename);
	MediaSample* get_frame();
	
private:
	
	void init(VideoStreamInfo* vinfo);
	void init(AudioStreamInfo* ainfo);

	MediaType::Enum media_type;
	MediaSample* inframe;
	std::ifstream infile;
	int uncompressed_frame_size;
	int inframe_count;
	double frame_rate;
	bool eos;
};
