#include "stdafx.h"
#include "util.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

class UncompressedAVSplitter
{
public:

	~UncompressedAVSplitter()
	{
		inframe->release();
	}

	UncompressedAVSplitter() :
		inframe(primo::avblocks::Library::createMediaSample()),
		uncompressed_frame_size(0),
		inframe_count(0),
		eos(true),
		media_type(MediaType::Unknown)
	{}

	void init(const MediaSocket* socket, const wstring& filename)
	{
		if (!socket || filename.empty()) {
			return;
		}

		infile.open(filename, ios::in | ios::binary);
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

	MediaSample* get_frame()
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

	private:
	
	void init(VideoStreamInfo* vinfo)
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

	void init(AudioStreamInfo* ainfo)
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

	MediaType::Enum media_type;
	MediaSample* inframe;
	std::ifstream infile;
	int uncompressed_frame_size;
	int inframe_count;
	double frame_rate;
	bool eos;
};

struct TrackState
{
	enum 
	{
		Disabled = -1,
	};

	TrackState() : frame(NULL), index(Disabled), frame_count(0), progress (-1.0) {}

	MediaSample* frame;
	int32_t index;
	int32_t frame_count;
	double progress;
};

TrackState* select_mux_track(TrackState& vtrack, TrackState& atrack)
{
	if (vtrack.index != TrackState::Disabled && atrack.index != TrackState::Disabled)
	{
		if (!vtrack.frame)
			return &vtrack;

		if (!atrack.frame)
			return &atrack;

		return vtrack.frame->startTime() <= atrack.frame->startTime() ? &vtrack : &atrack;
	}
	
	if (vtrack.index != TrackState::Disabled) {
		return &vtrack;
	}

	if (atrack.index != TrackState::Disabled) {
		return &atrack;
	}

	return NULL;
}


/*
mix and encode uncompressed audio and video input files
*/
bool_t av_encode(MediaSocket* vinput, const wstring& vfile, 
				MediaSocket* ainput, const wstring& afile, 
				MediaSocket* output)
{
	bool_t res;

	TrackState vtrack, atrack;
	
	UncompressedAVSplitter vsplit, asplit;
	
	try {
		if (vinput) {
			wcout << "video input file: \"" << vfile << "\"" << endl;
			vsplit.init(vinput, vfile);
			wcout << "OK" << endl;
		}
		if (ainput) {
			wcout << "audio input file: \"" << afile << "\"" << endl;
			asplit.init(ainput, afile);
			wcout << "OK" << endl;
		}
	}
	catch (std::exception& ex) {
		wcout << ex.what() << endl;
		return FALSE;
	}

	// setup transcoder
	auto transcoder = primo::make_ref(primo::avblocks::Library::createTranscoder());
	// in order to use the OEM release for testing (without a valid license) the demo mode must be enabled.
	transcoder->setAllowDemoMode(TRUE);
	
	int track_index = 0; // start index

	if (vinput) {
		transcoder->inputs()->add(vinput);
		vtrack.index = track_index++;
	}

	if (ainput) {
		transcoder->inputs()->add(ainput);
		atrack.index = track_index++;
	}

	transcoder->outputs()->add(output);
	res = transcoder->open();
	printError(L"transcoder open", transcoder->error());
	if (!res)
		return FALSE;

	// transcoding loop
	for(;;)
	{
		if (vtrack.index != TrackState::Disabled && !vtrack.frame) {
			vtrack.frame = vsplit.get_frame();
		}
		
		if (atrack.index != TrackState::Disabled && !atrack.frame) {
			atrack.frame = asplit.get_frame();
		}

		TrackState* track = select_mux_track(vtrack, atrack);

		if (!track)
			break;

		// log
		if (track->frame) {
			if (track->frame->startTime() - track->progress >= 1.0) {
				track->progress = track->frame->startTime();
				wcout << "track " << track->index << " frame #" << track->frame_count << " pts:" << track->frame->startTime() << endl;
			}
		}
		else {
			wcout << "track " << track->index << " eos" << endl;
		}

		if (track->frame) {
			res = transcoder->push(track->index, track->frame);
			if (!res) {
				printError(L"transcoder push frame", transcoder->error());
				return FALSE;
			}
			track->frame = NULL; // clear the muxed frame in order to read to the next one
			track->frame_count++;
		}
		else {
			res = transcoder->push(track->index, NULL);
			if (!res) {
				printError(L"transcoder push eos", transcoder->error());
				return FALSE;
			}
			track->index = TrackState::Disabled; // disable track
		}
	}

	res = transcoder->flush();
	if (!res) {
		printError(L"transcoder flush", transcoder->error());
		return FALSE;
	}

	transcoder->close();

	return TRUE;
}
