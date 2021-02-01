#include <cassert>
#include <fstream>
#include <iostream>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>
#include <string.h>

#include "options.h"
#include "util.h"

using namespace primo::avblocks;

class OutputStream: public primo::Stream
{
    mutable std::ofstream fstream_;
    std::string filename_;
    
public:
    
    OutputStream() {}
    
    ~OutputStream() { close(); }
    
    bool_t setFile(const std::string& filename)
    {
        if (isOpen())
            return true;
        
        filename_ = filename;
        return false;
    }
    
    virtual bool_t open()
    {
        if (isOpen() || filename_.empty())
            return false;
        
        fstream_.open(filename_, std::ios_base::binary);
        return fstream_.is_open();
    }
    
    virtual void close()
    {
        fstream_.close();
    }
    
    virtual bool_t isOpen() const
    {
        return (fstream_.is_open() ? true : false);
    }
    
    virtual bool_t canRead() const { return false; }
    virtual bool_t canWrite() const { return true; }
    virtual bool_t canSeek() const { return true; }
    virtual bool_t read(void* buffer, int32_t bufferSize, int32_t* totalRead) { return false; }
    
    virtual bool_t write(const void* buffer, int32_t dataSize)
    {
        fstream_.write((const char*)buffer, dataSize);
        return fstream_ ? true : false;
    }
    
    virtual int64_t size() const
    {
        return position();
    }
    
    virtual int64_t position() const
    {
        int64_t pos;
        pos = fstream_.tellp();
        
        return pos;
    }
    
    virtual bool_t seek(int64_t position)
    {
        fstream_.seekp(position, std::ios_base::beg);
        return fstream_ ? true : false;
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
            return false;
        }
        
        if(splitTime_ && (splitTime_ <= currentTime))
        {
            isSplit_ = true;
            return false;
        }
        
        return true;
    }
    
    void onProgress(double processedTime, double totalTime)
    {
        using namespace std;
        
        processedTime_ = std::max<double>(processedTime_, processedTime);
        
        if (reportProgress_)
        {
            cout.setf(ios::fixed);
            cout.precision(1);
            cout << "progress: " << processedTime << " sec." << endl;
            cout.precision(2);
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

bool transcodeSplit(const Options& opt, const std::string& outFile, const char* preset,
                    double startTime, double &processedTime, int64_t &processedSize, bool& isSplit)
{
    deleteFile(outFile.c_str());
    
    primo::ref<Transcoder> transcoder(Library::createTranscoder());
    transcoder->setAllowDemoMode(true);
    
    // Input
    primo::ref<MediaSocket> inSocket (Library::createMediaSocket());
    inSocket->setFile(primo::ustring(opt.in_file));
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

double getMinDuration(const std::string& file)
{
    double minDuration = 0;
    primo::ref<MediaInfo> info ( Library::createMediaInfo() );
    info->setInputFile(primo::ustring(file));
    
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