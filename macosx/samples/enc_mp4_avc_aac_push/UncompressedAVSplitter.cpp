#include <iostream>
#include <fstream>
#include <stdexcept>

// memmove in AVBlocks
#include <string.h>
#include <AVBlocks.h>
#include <PrimoUString.h>

#include "util.h"
#include "UncompressedAVSplitter.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

UncompressedAVSplitter::~UncompressedAVSplitter()
{
	inframe->release();
}

UncompressedAVSplitter::UncompressedAVSplitter() :
		inframe(primo::avblocks::Library::createMediaSample()),
		uncompressed_frame_size(0),
		inframe_count(0),
		eos(true),
		media_type(MediaType::Unknown)
{}

void UncompressedAVSplitter::init(const MediaSocket* socket, const string& filename)
{
		if (!socket || filename.empty()) {
			return;
		}

		infile.open(filename.c_str(), ios::in | ios::binary);
		if (!infile)
		{
			throw std::logic_error("Cannot open file");
		}

		StreamInfo* sinfo = socket->pins()->at(0)->streamInfo();

		media_type = sinfo->mediaType();
		
		if (sinfo->mediaType() == MediaType::Video) 
		{
			init((VideoStreamInfo*)sinfo);
		}
		else if (sinfo->mediaType() == MediaType::Audio) 
		{
			init((AudioStreamInfo*)sinfo);
		}
		else 
		{
			throw std::logic_error("Unsupported: unknown media type");
		}

		eos = false;
}

MediaSample* UncompressedAVSplitter::get_frame()
{
		if (eos)
			return NULL;

		if (media_type == MediaType::Video)
		{
			infile.read((char*)inframe->buffer()->start(), uncompressed_frame_size);
			int bytes_read = (int)infile.gcount();

			if (bytes_read < uncompressed_frame_size) {
				eos = true;
			}
			else {
				inframe->buffer()->setData(0, uncompressed_frame_size);
				double time = inframe_count / frame_rate;
				inframe->setStartTime(time);
				inframe->setEndTime(-1.0);
				++inframe_count;
			}
		}
		else if (media_type == MediaType::Audio)
		{
			if (inframe->buffer()->dataSize() == 0) {
				infile.read((char*)inframe->buffer()->start(), uncompressed_frame_size);
				int bytes_read = (int)infile.gcount();

				if (0 == bytes_read) {
					eos = true;
				}
				else {
					inframe->buffer()->setData(0, bytes_read);
					double time = inframe_count / frame_rate;
					inframe->setStartTime(time);
					inframe->setEndTime(-1.0);
					++inframe_count;
				}
			}
		}

		return (eos ? NULL : inframe);
}


void UncompressedAVSplitter::init(VideoStreamInfo* vinfo)
{
		if (vinfo->streamType() == StreamType::UncompressedVideo)
		{
			uncompressed_frame_size = vinfo->frameWidth() * vinfo->frameHeight() * 3 / 2; // assume YUV 4:2:0
			MediaBuffer* buffer = (primo::avblocks::Library::createMediaBuffer(uncompressed_frame_size));
			inframe->setBuffer(buffer);
			buffer->release();
		}
		else
		{
			throw std::logic_error("Unsupported: video input is compressed");
		}

		frame_rate = vinfo->frameRate();
}

void UncompressedAVSplitter::init(AudioStreamInfo* ainfo)
{
		if (ainfo->streamType() == StreamType::LPCM)
		{
			if (ainfo->bytesPerFrame() == 0) {
				ainfo->setBytesPerFrame(ainfo->bitsPerSample() / 8 * ainfo->channels());
			}
			
			frame_rate = 10;
			uncompressed_frame_size = ainfo->bytesPerFrame() * ainfo->sampleRate() / (int32_t)frame_rate;
			MediaBuffer* buffer(primo::avblocks::Library::createMediaBuffer(uncompressed_frame_size));
			inframe->setBuffer(buffer);
			buffer->release();
		}
		else
		{
			throw std::logic_error ("Unsupported: audio input is compressed");
		}
}
