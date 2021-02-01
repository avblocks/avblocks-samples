#include "stdafx.h"
#include "options.h"
#include "util.h"

using namespace primo::avblocks;

class OutputStream: public primo::Stream
{
	mutable std::ofstream fstream_;
	std::wstring filename_;

public:

	OutputStream() {}

	~OutputStream() { close(); }

	bool_t setFile(const std::wstring& filename)
	{
		if (isOpen())
			return TRUE;

		filename_ = filename;
		return FALSE;
	}
		
	virtual bool_t open()
	{
		if (isOpen() || filename_.empty())
			return FALSE;

		fstream_.open(filename_, std::ios_base::binary);
		return fstream_.is_open();
	}

	virtual void close()
	{
		fstream_.close();
	}

	virtual bool_t isOpen() const
	{
		return (fstream_.is_open() ? TRUE : FALSE); 
	}

	virtual bool_t canRead() const { return FALSE; }
	virtual bool_t canWrite() const { return TRUE; }
	virtual bool_t canSeek() const { return TRUE; }
	virtual bool_t read(void* buffer, int32_t bufferSize, int32_t* totalRead) { return FALSE; }

	virtual bool_t write(const void* buffer, int32_t dataSize)
	{
		fstream_.write((const char*)buffer, dataSize);
		return fstream_ ? TRUE : FALSE;
	}

	virtual int64_t size() const
	{
		return position();
	}

	virtual int64_t position() const
	{
		int64_t pos;

		#if _MSC_VER < 1700 
			// works around a bug in VC10 where tellp() is truncated when converted to __int64
			pos = fstream_.tellp().seekpos();
		#else
			pos = fstream_.tellp();
		#endif

		return pos;
	}

	virtual bool_t seek(int64_t position)
	{
		fstream_.seekp(position, std::ios_base::beg);
		return fstream_ ? TRUE : FALSE;
	}
};

class TranscoderSplitCallback: public primo::avblocks::TranscoderCallback
{
public:
	TranscoderSplitCallback():
		outputStream_(NULL), 
		splitTime_(0),
		splitSize_(0),
		reportProgress_(true),
		processedTime_(0),
		isSplit_(false)
	{ }
	
	void setOutputStream(primo::Stream* outputStream) 
	{
		outputStream_ = outputStream;
	}

	void setSplitTime(double seconds)
	{
		splitTime_ = seconds;
	}

	void setSplitSize(int64_t bytes)
	{
		splitSize_ = bytes;
	}

	void setReportProgress(bool reportProgress)
	{
		reportProgress_ = reportProgress;
	}

	bool_t onContinue(double currentTime)
	{
		processedTime_ = std::max<double>(processedTime_, currentTime);

		if(outputStream_ && splitSize_ && 
		   (outputStream_->size() > splitSize_))
		{
			isSplit_ = true;
			return FALSE;
		}

		if(splitTime_ && (splitTime_ <= currentTime))
		{
			isSplit_ = true;
			return FALSE;
		}

		return TRUE;
	}

	void onProgress(double processedTime, double totalTime)
	{
		using namespace std;

		processedTime_ = std::max<double>(processedTime_, processedTime);

		if (reportProgress_)
		{
			wcout.setf(ios::fixed);
			wcout.precision(1);
			wcout << L"progress: " << processedTime << L" sec." << endl;
			wcout.precision(2);
		}
	}

	double processedTime() const { return processedTime_; } 
	int64_t processedSize() const { return outputStream_->size(); }
	bool isSplit() const { return isSplit_; } 

private:
	double processedTime_;
	primo::Stream *outputStream_;
	double splitTime_;
	int64_t splitSize_;
	bool reportProgress_;
	bool isSplit_;
};

bool transcodeSplit(const Options& opt, const std::wstring& outFile, const char* preset,
					double startTime, double &processedTime, int64_t &processedSize, bool& isSplit)
{
	deleteFile(outFile.c_str());

	primo::ref<Transcoder> transcoder(Library::createTranscoder());
	transcoder->setAllowDemoMode(TRUE);

	// Input
	primo::ref<MediaSocket> inSocket (Library::createMediaSocket());
	inSocket->setFile(opt.in_file.c_str());
	inSocket->setTimePosition(startTime);
	transcoder->inputs()->add(inSocket.get());

	// Output
	OutputStream outputStream;
	outputStream.setFile(outFile.c_str());

	primo::ref<MediaSocket> outSocket(Library::createMediaSocket(preset));
	outSocket->setStream(&outputStream);
	transcoder->outputs()->add(outSocket.get());

	TranscoderSplitCallback callback;
	callback.setOutputStream(&outputStream);
	callback.setReportProgress(false);
	
	if (opt.split_time > 0)
		callback.setSplitTime(opt.split_time);

	if (opt.split_size > 0)
		callback.setSplitSize(opt.split_size);
	
	transcoder->setCallback(&callback);

	if (!transcoder->open())
	{
		printError(transcoder->error());
		return false;
	}
		
	if (!transcoder->run())
	{
		printError(transcoder->error());
		return false;
	}

	transcoder->close();

	processedTime = callback.processedTime();
	processedSize = callback.processedSize();
	isSplit = callback.isSplit();
	return true;
}

double getMinDuration(const std::wstring& file)
{
	double minDuration = 0;
	primo::ref<MediaInfo> info ( Library::createMediaInfo() );
	info->setInputFile(file.c_str());

	if (info->load())
	{
		minDuration = (info->streams()->count() > 0) ? info->streams()->at(0)->duration() : -1;

		for (int i=1; i < info->streams()->count(); ++i)
		{

				minDuration = std::min(minDuration, info->streams()->at(i)->duration());
		}
		return minDuration;
	}
	else
	{
		printError(info->error());
	}
	
	return 0;	
}