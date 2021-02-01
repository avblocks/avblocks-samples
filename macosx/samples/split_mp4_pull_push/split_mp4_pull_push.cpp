#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <iomanip>
#include <unistd.h>

// memmove in AVBlocks
#include <string.h>
#include <AVBlocks.h>
#include <PrimoReference++.h>
#include <PrimoUString.h>

#include "util.h"
#include "options.h"


using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;
namespace av = primo::avblocks;
namespace pc = primo::codecs;

void printError(const char* action, const primo::error::ErrorInfo* e)
{
    if (action)
    {
        cout << action << ": ";
    }
    
    if (primo::error::ErrorFacility::Success == e->facility())
    {
        cout << "Success" << endl;
        return;
    }
    
    if (e->message())
    {
        cout << primo::ustring(e->message()) << " ";
    }
    
    cout << "facility:" << e->facility() << " error:" << e->code() << "" << endl;
}

static std::string generateOutputFileName(const std::string &inputFile, int partNum)
{
    std::string fileName = inputFile;
    
    string::size_type pos = fileName.rfind('.');
    if (string::npos != pos)
        fileName = fileName.substr(0, pos);
    
    pos = fileName.rfind('/');
    if (string::npos != pos)
        fileName = fileName.substr(pos + 1);
    
    pos = fileName.rfind('\\');
    if (string::npos != pos)
        fileName = fileName.substr(pos + 1);
    
    ostringstream outputFile;
    outputFile << setfill('0') << setw(3) << partNum;
    
    return fileName + "_"  + outputFile.str();
}

struct SplitRecord
{
    SplitRecord()
    {
        startTime = 0;
        endTime = 0;
        startTimeActual = 0;
        endTimeActual = 0;
    }
    
    std::string fileName;
    double startTime;
    double endTime;
    
    double startTimeActual;
    double endTimeActual;
};

static std::string formatTime(double t)
{
    int totalSeconds = (int)t;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    int miliseconds = int(floor(t * 1000 + 0.5)) % 1000;
    
    ostringstream s;
    s << setfill('0') << setw(2) << minutes << ":" << setw(2) <<seconds << "." << setw(3) << miliseconds;
    
    return s.str();
}

static void disposeSamples(std::list<MediaSample*> &samples)
{
    while(samples.size() > 0)
    {
        samples.front()->release();
        samples.pop_front();
    }
}

static bool splitFile(const std::string &inputFile)
{
    std::string outputFileExt = ".mpg";
    const char *encodingPreset = Preset::Video::DVD::NTSC_4x3_PCM;
    const double splitPartDuration = 10; // seconds
    
    int audioStreamIndex = -1;
    int videoStreamIndex = -1;
    
    int audioFrameSize = 0;
    int audioSampleRate = 0;
    
    auto transcoder1 = primo::make_ref(Library::createTranscoder());
    // In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
    transcoder1->setAllowDemoMode(1);
    
    auto inputInfo = primo::make_ref(primo::avblocks::Library::createMediaInfo());
    inputInfo->inputs()->at(0)->setFile(primo::ustring(inputFile));
    if (!inputInfo->open())
    {
        printError("Open MediaInfo", inputInfo->error());
        return false;
    }
    
    // Configure transcoder1 input and output
    {
        auto inputSocket = primo::make_ref(Library::createMediaSocket(inputInfo.get()));
        
        transcoder1->inputs()->add(inputSocket.get());
        
        for(int i = 0; i < inputSocket->pins()->count(); i++)
        {
            StreamInfo * inputStreamInfo = inputSocket->pins()->at(i)->streamInfo();
            
            if((inputStreamInfo->mediaType() == MediaType::Video) && videoStreamIndex < 0)
            {
                auto streamInfo = primo::make_ref(Library::createVideoStreamInfo());
                
                VideoStreamInfo* inputVideoStreamInfo = dynamic_cast<VideoStreamInfo*>(inputStreamInfo);
                
                streamInfo->setColorFormat(ColorFormat::YUV420);
                streamInfo->setStreamType(StreamType::UncompressedVideo);
                streamInfo->setScanType(inputVideoStreamInfo->scanType());
                
                streamInfo->setFrameWidth(inputVideoStreamInfo->frameWidth());
                streamInfo->setFrameHeight(inputVideoStreamInfo->frameHeight());
                streamInfo->setDisplayRatioWidth(inputVideoStreamInfo->displayRatioWidth());
                streamInfo->setDisplayRatioHeight(inputVideoStreamInfo->displayRatioHeight());
                
                auto outputPin = primo::make_ref(Library::createMediaPin());
                outputPin->setStreamInfo(streamInfo.get());
                
                auto outputSocket = primo::make_ref(Library::createMediaSocket());
                outputSocket->pins()->add(outputPin.get());
                outputSocket->setStreamType(streamInfo->streamType());
                
                videoStreamIndex = transcoder1->outputs()->count();
                transcoder1->outputs()->add(outputSocket.get());
            }
            
            if((inputStreamInfo->mediaType() == MediaType::Audio) && audioStreamIndex < 0)
            {
                auto streamInfo = primo::make_ref(Library::createAudioStreamInfo());
                
                AudioStreamInfo* inputAudioStreamInfo = dynamic_cast<AudioStreamInfo*>(inputStreamInfo);
                
                streamInfo->setStreamType(StreamType::LPCM);
                
                streamInfo->setPcmFlags(inputAudioStreamInfo->pcmFlags());
                streamInfo->setChannels(inputAudioStreamInfo->channels());
                streamInfo->setSampleRate(inputAudioStreamInfo->sampleRate());
                streamInfo->setBitsPerSample(inputAudioStreamInfo->bitsPerSample());
                
                auto outputPin = primo::make_ref(Library::createMediaPin());
                outputPin->setStreamInfo(streamInfo.get());
                
                auto outputSocket = primo::make_ref(Library::createMediaSocket());
                outputSocket->pins()->add(outputPin.get());
                outputSocket->setStreamType(streamInfo->streamType());
                
                audioStreamIndex = transcoder1->outputs()->count();
                transcoder1->outputs()->add(outputSocket.get());
                
                audioFrameSize = inputAudioStreamInfo->channels() * inputAudioStreamInfo->bitsPerSample() / 8;
                audioSampleRate = inputAudioStreamInfo->sampleRate();
            }
        }
    }
    
    bool_t res = transcoder1->open();
    printError("Open Transcoder1", transcoder1->error());
    if (!res)
        return false;
    
    auto sample = primo::make_ref(Library::createMediaSample());
    primo::ref<Transcoder> transcoder2;
    int32_t outputIndex;
    
    int splitPartNum = 0;
    double splitTime = splitPartDuration;
    double partStartTime = 0;
    
    std::vector<SplitRecord> splitStats;
    std::list<MediaSample*> audioSamplesQueue;
    
    for(;;)
    {
        if((audioSamplesQueue.size() > 0) && (audioSamplesQueue.front()->startTime() < splitTime))
        {
            outputIndex = audioStreamIndex;
            sample.reset(audioSamplesQueue.front());
            audioSamplesQueue.pop_front();
        }
        else
        {
            if(!transcoder1->pull(outputIndex, sample.get()))
                break;
            
            if((outputIndex != audioStreamIndex) &&
               (outputIndex != videoStreamIndex))
            {
                continue;
            }
        }
        
        if(outputIndex == audioStreamIndex)
        {
            double sampleDuration = static_cast<double>(sample->buffer()->dataSize()) / static_cast<double>(audioFrameSize * audioSampleRate);
            if(sample->startTime() >= splitTime)
            {
                audioSamplesQueue.push_back(sample.release());
                sample.reset(Library::createMediaSample());
                continue;
            }
            else if((sample->startTime() + sampleDuration) > splitTime)
            {
                double sample1Duration = splitTime - sample->startTime();
                int sample1BufferSize = static_cast<int>(sample1Duration * audioSampleRate) * audioFrameSize;
                
                if(sample1BufferSize < sample->buffer()->dataSize())
                {
                    int buffer2Size = sample->buffer()->dataSize() - sample1BufferSize;
                    auto buffer2 = primo::make_ref(Library::createMediaBuffer(buffer2Size));
                    buffer2->setData(0, buffer2Size);
                    memcpy(buffer2->data(), sample->buffer()->data() + sample1BufferSize, buffer2Size);
                    
                    auto sample2 = primo::make_ref(Library::createMediaSample());
                    sample2->setStartTime(sample->startTime() + sample1Duration);
                    sample2->setBuffer(buffer2.get());
                    
                    if(sample1BufferSize > 0)
                    {
                        sample->buffer()->setData(sample->buffer()->dataOffset(), sample1BufferSize);
                    }
                    else
                    {
                        sample->buffer()->setData(0, 0);
                    }
                    
                    audioSamplesQueue.push_back(sample2.release());
                }
            }
        }
        
        if((transcoder2.get() == NULL) ||
           ((sample->startTime() + 0.0001 >= splitTime) && (outputIndex == videoStreamIndex)))
        {
            if(transcoder2.get() != NULL)
            {
                transcoder2->flush();
                transcoder2->close();
            }
            
            SplitRecord splitStat;
            splitStat.startTime = splitTime;
            splitStat.startTimeActual = sample->startTime();
            
            splitPartNum += 1;
            splitTime = splitPartNum * splitPartDuration;
            partStartTime = sample->startTime();
            
            transcoder2.reset(Library::createTranscoder());
            transcoder2->setAllowDemoMode(1);
            
            // Configure transcoder2 input and output
            {
                for(int i = 0; i < transcoder1->outputs()->count(); i++)
                {
                    auto streamInfo = primo::make_ref(transcoder1->outputs()->at(i)->pins()->at(0)->streamInfo()->clone());
                    auto pin = primo::make_ref(Library::createMediaPin());
                    pin->setStreamInfo(streamInfo.get());
                    
                    auto socket = primo::make_ref(Library::createMediaSocket());
                    socket->pins()->add(pin.get());
                    socket->setStreamType(streamInfo->streamType());
                    
                    transcoder2->inputs()->add(socket.get());
                }
                
                auto outputSocket = primo::make_ref(Library::createMediaSocket(encodingPreset));
                
                std::string fileName = generateOutputFileName(inputFile, splitPartNum) + outputFileExt;
                std::string filePath = getExeDir() + "//" + fileName;
                
                deleteFile(filePath.c_str());
                
                outputSocket->setFile(primo::ustring(filePath));
                transcoder2->outputs()->add(outputSocket.get());
                
                splitStat.fileName = fileName;
            }
            
            if(splitStats.size() > 0)
            {
                SplitRecord & lastRecord = splitStats[splitStats.size() - 1];
                lastRecord.endTime = splitStat.startTime;
                lastRecord.endTimeActual = splitStat.startTimeActual;
            }
            
            splitStats.push_back(splitStat);
            
            res = transcoder2->open();
            printError("Open Transcoder2", transcoder2->error());
            if (!res)
            {
                disposeSamples(audioSamplesQueue);
                return false;
            }
        }
        
        if((splitStats.size() > 0))
        {
            SplitRecord & lastRecord = splitStats[splitStats.size() - 1];
            lastRecord.endTime = sample->startTime();
            lastRecord.endTimeActual = lastRecord.endTime;
        }
        
        if(sample->startTime() >= 0)
            sample->setStartTime(sample->startTime() - partStartTime);
        
        res = transcoder2->push(outputIndex, sample.get());
        if(!res)
        {
            printError("Push Transcoder2", transcoder2->error());
            disposeSamples(audioSamplesQueue);
            return false;
        }
    }
    
    disposeSamples(audioSamplesQueue);
    
    if((transcoder1->error()->facility() != primo::error::ErrorFacility::Codec) ||
       (transcoder1->error()->code() != primo::codecs::CodecError::EOS))
    {
        printError("Pull Transcoder1", transcoder1->error());
        return false;
    }
    
    transcoder1->close();
    
    transcoder2->flush();
    transcoder2->close();
    
    // print split stats
    std::cout << std::endl;
    for(unsigned int i = 0; i < splitStats.size(); i++)
    {
        SplitRecord & record = splitStats[i];
        
        std::cout << record.fileName << " start: " << formatTime(record.startTime) << " end: " << formatTime(record.endTime) << 
        " act. start: " << formatTime(record.startTimeActual) << " act. end: " << formatTime(record.endTimeActual) << std::endl;
        
    }
    std::cout << std::endl;
    
    return true;
}

int main(int argc, char* argv[])
{
    Options opt;
    
    switch(prepareOptions(opt, argc, argv))
    {
        case Command: return 0;
        case Error:	return 1;
    }
    
    primo::avblocks::Library::initialize();
    
    // set your license string
    // primo::avblocks::Library::setLicense("PRIMO-LICENSE");
    
    bool splitFileResult = splitFile(opt.input_file);
    
    primo::avblocks::Library::shutdown();
    
    return splitFileResult ? 0 : 1;
}