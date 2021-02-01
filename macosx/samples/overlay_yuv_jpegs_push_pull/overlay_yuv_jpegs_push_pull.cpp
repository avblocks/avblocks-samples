/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

#include <PrimoUString.h>
#include "util.h"
#include "options.h"

using namespace std;
using namespace primo::avblocks;
using namespace primo::codecs;
using namespace primo::error;

class Overlay
{
public:
    const ErrorInfo* error() const { return t->error(); }
    
	Overlay() : t(Library::createTranscoder()) {}
	
    void close();
    bool open(const string& imageOverlay, StreamType::Enum imageType, VideoStreamInfo *uncompressedVideo);
    bool pull(int &outputIndex, MediaSample *outputSample);
    bool push(int inputIndex, MediaSample *inputSample);
    void setOverlayParamsToPin(MediaPin *pin, const string& imageOverlay, StreamType::Enum imageType);
    
private:
    primo::ref<Transcoder> t;
};

void Overlay::close()
{
    t->close();
}

bool Overlay::open(const string& imageOverlay, StreamType::Enum imageType, VideoStreamInfo * uncompressedVideo)
{
    auto inSocket = primo::make_ref(Library::createMediaSocket());
    inSocket->setStreamType(StreamType::UncompressedVideo);
    
    auto inPin = primo::make_ref(Library::createMediaPin());
    inPin->setStreamInfo(uncompressedVideo);
    
    inSocket->pins()->add(inPin.get());
	
	t->inputs()->clear();
    t->inputs()->add(inSocket.get());
    
    auto outSocket = primo::make_ref(Library::createMediaSocket());
    outSocket->setStreamType(StreamType::UncompressedVideo);
    
    auto outPin = primo::make_ref(Library::createMediaPin());
    outPin->setStreamInfo(uncompressedVideo);
    
    outSocket->pins()->add(outPin.get());
    t->outputs()->add(outSocket.get());
    
    setOverlayParamsToPin(outSocket->pins()->at(0), imageOverlay, imageType);
    
    return t->open();
}

bool Overlay::pull(int & outputIndex, MediaSample * outputSample)
{
    return t->pull(outputIndex, outputSample);
}

bool Overlay::push(int inputIndex, MediaSample * inputSample)
{
    return t->push(inputIndex, inputSample);
}

MediaBuffer* createMediaBufferWithFileBytes(const string &imagePath)
{
    vector<uint8_t> imageData = readFileBytes(imagePath.c_str());
    int imageSize = imageData.size();
    
    if (imageSize <= 0)
    {
        std::wcout << L"Could not load the overlay image." << std::endl;
        return NULL;
    }
    
    MediaBuffer* mediaBuffer = Library::createMediaBuffer();
    mediaBuffer->append(&imageData[0], imageSize);
    
    return mediaBuffer;
}

void Overlay::setOverlayParamsToPin(MediaPin * pin, const string& imageOverlay, StreamType::Enum imageType)
{
    auto videoInfo = primo::make_ref(Library::createVideoStreamInfo());
    videoInfo->setStreamType(imageType);
    
    pin->params()->addInt(Param::Video::Overlay::Mode, primo::codecs::AlphaCompositingMode::Atop);
    pin->params()->addInt(Param::Video::Overlay::LocationX, 0);
    pin->params()->addInt(Param::Video::Overlay::LocationY, 0);
    pin->params()->addFloat(Param::Video::Overlay::BackgroundAlpha, 1.0);
    pin->params()->addFloat(Param::Video::Overlay::ForegroundAlpha, 1.0);
    
    auto buffer = primo::make_ref(createMediaBufferWithFileBytes(imageOverlay));
    pin->params()->addMediaBuffer(Param::Video::Overlay::ForegroundBuffer, buffer.get());
    pin->params()->addVideoStreamInfo(Param::Video::Overlay::ForegroundBufferFormat, videoInfo.get());
}

MediaSocket* configureDecoderInputSocket(Options& opt, VideoStreamInfo *vsi = NULL)
{
    MediaSocket* socket = Library::createMediaSocket();
    socket->setFile(primo::ustring(opt.input_file));
    
    if (vsi != NULL)
    {
        auto pin = primo::make_ref(Library::createMediaPin());
        pin->setStreamInfo(vsi);
        
        socket->pins()->add(pin.get());
        socket->setStreamType(StreamType::UncompressedVideo);
    }
    
    return socket;
}

MediaSocket* configureDecoderOutputSocket(Options& opt)
{
    MediaSocket* socket = Library::createMediaSocket();
    socket->setStreamType(StreamType::UncompressedVideo);
    
    auto vsi = primo::make_ref(Library::createVideoStreamInfo());
    vsi->setStreamType(StreamType::UncompressedVideo);
    
    auto outputVideoPin = primo::make_ref(Library::createMediaPin());
    outputVideoPin->setStreamInfo(vsi.get());
    
    socket->pins()->add(outputVideoPin.get());
    
    return socket;
}

Transcoder* CreateDecoder(Options& opt)
{
    Transcoder* decoder = Library::createTranscoder();
    decoder->setAllowDemoMode(true);
    
    auto inSocket = primo::make_ref(configureDecoderInputSocket(opt));
    decoder->inputs()->add(inSocket.get());
    
    auto outSocket = primo::make_ref(configureDecoderOutputSocket(opt));
    decoder->outputs()->add(outSocket.get());
    
    return decoder;
}

MediaSocket* configureEncoderInputSocket(Options& opt, VideoStreamInfo *inputVideo)
{
    MediaSocket* socket = Library::createMediaSocket();
    socket->setStreamType(StreamType::UncompressedVideo);
    
    auto pin = primo::make_ref(Library::createMediaPin());
    pin->setStreamInfo(inputVideo);
    
    socket->pins()->add(pin.get());
    
    return socket;
}

MediaSocket* configureEncoderOutputSocket(Options& opt, VideoStreamInfo *outputVideo)
{
    MediaSocket* socket = Library::createMediaSocket();
    socket->setFile(primo::ustring(opt.output_file));
    
    auto pin = primo::make_ref(Library::createMediaPin());
    pin->setStreamInfo(outputVideo);
    
    socket->pins()->add(pin.get());
    
    return socket;
}

Transcoder* CreateEncoder(Options& opt, VideoStreamInfo *in, VideoStreamInfo *out)
{
    Transcoder* encoder = Library::createTranscoder();
    encoder->setAllowDemoMode(true);
    
    auto inSocket = primo::make_ref(configureEncoderInputSocket(opt, in));
    encoder->inputs()->add(inSocket.get());
    
    auto outSocket = primo::make_ref(configureEncoderOutputSocket(opt, out));
    encoder->outputs()->add(outSocket.get());
    
    return encoder;
}

static bool applyOverlay(Options& opt)
{
    auto info = primo::make_ref(Library::createMediaInfo());
    info->inputs()->at(0)->setFile(primo::ustring(opt.input_file));
    
    if (!info->open())
    {
        printError("load info", info->error());
        return false;
    }
    
    primo::ref<VideoStreamInfo> inputVSI;
    for (int s = 0; s < info->outputs()->count(); s++)
    {
        auto socket = info->outputs()->at(s);
        for (int p = 0; p < socket->pins()->count(); p++)
        {
            auto pin = socket->pins()->at(p);
            StreamInfo *si = pin->streamInfo();
            if (si->mediaType() == MediaType::Video)
            {
                inputVSI.reset((VideoStreamInfo *)si->clone());
                break;
            }
        }
    }
    
    inputVSI->setStreamType(StreamType::UncompressedVideo);
    inputVSI->setColorFormat(ColorFormat::YUV420);
    
    auto decoder = CreateDecoder(opt);
    if (!decoder->open())
    {
        printError("decoder open", decoder->error());
        return false;
    }
    
    deleteFile(opt.output_file.c_str());
    
    auto outputVSI = primo::make_ref(inputVSI->clone());
    outputVSI->setStreamType(StreamType::H264);
    
    auto encoder = CreateEncoder(opt, inputVSI.get(), outputVSI.get());
    if (!encoder->open())
    {
        printError("encoder open", encoder->error());
        return false;
    }
    
    int outputIndex;
    auto decodedSample   = primo::make_ref(Library::createMediaSample());
    auto overlayedSample = primo::make_ref(Library::createMediaSample());
    int decodedSamples   = 0;
    int overlayedSamples = 0;
    int encodedSamples   = 0;
    
    Overlay overlay;
    
    while (true)
    {
        if (!decoder->pull(outputIndex, decodedSample.get()))
        {
            printError("decoder pull", decoder->error());
            break;
        }
        ++decodedSamples;
        
        char imgFile[PATH_MAX];
        string pattern = opt.overlay_image + "/cube%04d (128x96).jpg";
        sprintf(imgFile, primo::ustring(pattern), overlayedSamples % opt.overlay_count);
        
        overlay.close();
        if (!overlay.open(imgFile, StreamType::JPEG, inputVSI.get()))
        {
            printError("overlay open", overlay.error());
            break;
        }
        
        if (!overlay.push(0, decodedSample.get()))
        {
            printError("overlay push", overlay.error());
            break;
        }
        
        if (!overlay.pull(outputIndex, overlayedSample.get()))
        {
            printError("overlay pull", overlay.error());
            break;
        }
        ++overlayedSamples;
        
        if (!encoder->push(0, overlayedSample.get()))
        {
            printError("encoder push", encoder->error());
            break;
        }
        ++encodedSamples;
    }
    
    decoder->close();
    encoder->flush();
    encoder->close();
    
    cout << "samples decoded/overlayed/encoded: " << decodedSamples   <<
    "/" << overlayedSamples <<
    "/" << encodedSamples   << std::endl;
    
    bool success = (decodedSamples > 0 && decodedSamples == encodedSamples);
    
    if (success)
        cout << "output file: " << opt.output_file << endl;
    
    return success;
}

int main(int argc, char* argv[])
{
    Options opt;
    
    switch(prepareOptions( opt, argc, argv))
    {
        case Command: return 0;
        case Error: return 1;
    }
    
    primo::avblocks::Library::initialize();
    
    // set your license string
    // primo::avblocks::Library::setLicense("PRIMO-LICENSE");
    
    bool overlayResult = applyOverlay(opt);
    
    primo::avblocks::Library::shutdown();
    
    return overlayResult ? 0 : 1;
}

