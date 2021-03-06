#include <iostream>
#include <fstream>
#include <stdexcept>

// memmove in AVBlocks
#include <string.h>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>

#include "util.h"
#include "UncompressedAVSplitter.h"

using namespace primo::codecs;
using namespace primo::avblocks;
using namespace std;

inline void printError(const char* action, const primo::error::ErrorInfo* e)
{
    if (action)
    {
            std::cout << action << ": ";
    }

    if (primo::error::ErrorFacility::Success == e->facility())
    {
            std::cout << "Success" << std::endl;
            return;
    }

    if (e->message())
    {
            std::cout << primo::ustring(e->message()) << " ";
    }

    std::cout << "facility:" << e->facility() << " error:" << e->code() << "" << std::endl;
}

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
bool av_encode(MediaSocket* vinput, const string& vfile,
				MediaSocket* ainput, const string& afile, 
				MediaSocket* output)
{
	bool_t res;

	TrackState vtrack, atrack;
	
	UncompressedAVSplitter vsplit, asplit;
	
	try {
		if (vinput) {
			cout << "video input file: \"" << vfile << "\"" << endl;
			vsplit.init(vinput, vfile);
			cout << "OK" << endl;
		}
		if (ainput) {
			cout << "audio input file: \"" << afile << "\"" << endl;
			asplit.init(ainput, afile);
			cout << "OK" << endl;
		}
	}
	catch (std::exception& ex) {
		cout << ex.what() << endl;
		return false;
	}

	// setup transcoder
	auto transcoder = primo::make_ref(primo::avblocks::Library::createTranscoder());
	// in order to use the OEM release for testing (without a valid license) the demo mode must be enabled.
	transcoder->setAllowDemoMode(1);
	
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
	printError("transcoder open", transcoder->error());
	if (!res)
		return false;

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
				cout << "track " << track->index << " frame #" << track->frame_count << " pts:" << track->frame->startTime() << endl;
			}
		}
		else {
			cout << "track " << track->index << " eos" << endl;
		}

		if (track->frame) {
			res = transcoder->push(track->index, track->frame);
			if (!res) {
				printError("transcoder push frame", transcoder->error());
				return false;
			}
			track->frame = NULL; // clear the muxed frame in order to read to the next one
			track->frame_count++;
		}
		else {
			res = transcoder->push(track->index, NULL);
			if (!res) {
				printError("transcoder push eos", transcoder->error());
				return false;
			}
			track->index = TrackState::Disabled; // disable track
		}
	}

	res = transcoder->flush();
	if (!res) {
		printError("transcoder flush", transcoder->error());
		return false;
	}

	transcoder->close();
    
    return true;
}
