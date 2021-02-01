using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using PrimoSoftware.AVBlocks;
using System.Diagnostics;

namespace CaptureDS
{
    class CompositeTranscoder: Block
    {
        private Transcoder _audioEncoder = new Transcoder();
        private Transcoder _videoEncoder = new Transcoder();
        private Transcoder _mux = new Transcoder();
        private int _audioInputIndex;
        private int _videoInputIndex;
        private int _audioMuxIndex;
        private int _videoMuxIndex;
        private bool _audioEos;
        private bool _videoEos;

        public CompositeTranscoder()
        {
        }

        System.IO.FileStream _audioLog;
        public string AudioLog
        {
            get; set;
        }

        public bool AllowDemoMode 
        { 
            get{ return _videoEncoder.AllowDemoMode; }

            set
            {
                _audioEncoder.AllowDemoMode = value;
                _videoEncoder.AllowDemoMode = value;
                _mux.AllowDemoMode = value;
            }
        }

        private ErrorInfo _error = new ErrorInfo();
        public override ErrorInfo Error
        {
            get { return _error; }
        }

        public override void Close()
        {
            if (_audioLog != null)
                _audioLog.Close();
        }
        
        public override void Dispose()
        {
            _audioEncoder.Dispose();
            _videoEncoder.Dispose();
            _mux.Dispose();
        }

        public override bool EndOfStream(int inputIndex)
        {
            if (inputIndex == _audioInputIndex && !_audioEos) {
                _audioEncoder.EndOfStream(0);
                _audioEos = true;
            }

            if (inputIndex == _videoInputIndex && !_videoEos) {
                _videoEncoder.EndOfStream(0);
                _videoEos = true;
            }

            if (_audioEos && _videoEos)
                return Drain();

            return true;
        }

        public override bool Flush()
        {
            _audioEncoder.EndOfStream(0);
            _videoEncoder.EndOfStream(0);
            return Drain();
        }

        private bool Drain()
        {
            return (EncodeAndMux(_audioEncoder, null, _audioMuxIndex) &&
                    EncodeAndMux(_videoEncoder, null, _videoMuxIndex) &&
                    _mux.Flush());
        }


        private static int SelectInput(MediaSocketList inputs, MediaType mediaType)
        {
            for (int i = 0; i < inputs.Count; i++)
            {
                if (inputs[i].Pins.Count == 1 &&
                    inputs[i].Pins[0].StreamInfo.MediaType == mediaType)
                    return i;
            }

            return -1;
        }

        private static MediaPin SelectPin(MediaSocket socket, MediaType mediaType)
        {
            return socket.Pins.FirstOrDefault(pin => (pin.StreamInfo.MediaType == mediaType));
        }

        private static MediaSocket SocketFromPin(MediaPin pin)
        {
            var socket = new MediaSocket();
            socket.Pins.Add((MediaPin)pin.Clone());
            socket.StreamType = pin.StreamInfo.StreamType;
            socket.StreamSubType = pin.StreamInfo.StreamSubType;
            socket.Params = pin.Params;
            return socket;
        }

        private static bool IsErrorInputNeeded(ErrorInfo error)
        {
            return (error.Facility == ErrorFacility.Transcoder &&
                    error.Code == (int)TranscoderError.InputNeeded);
        }

        private static bool IsErrorInputFull(ErrorInfo error)
        {
            return (error.Facility == ErrorFacility.Transcoder &&
                    error.Code == (int)TranscoderError.InputFull);
        }

        private bool ConfigurePins(out MediaPin audioPin, out MediaPin videoPin)
        {
            audioPin = null;
            videoPin = null;

            if (Outputs.Count == 0)
                return false;

            audioPin = SelectPin(Outputs[0], MediaType.Audio);
            videoPin = SelectPin(Outputs[0], MediaType.Video);

            if (audioPin == null || videoPin == null)
                return false;

            var audioIn = Inputs[_audioInputIndex].Pins[0].StreamInfo as AudioStreamInfo;
            var audioOut = audioPin.StreamInfo as AudioStreamInfo;

            var videoIn = Inputs[_videoInputIndex].Pins[0].StreamInfo as VideoStreamInfo;
            var videoOut = videoPin.StreamInfo as VideoStreamInfo;

            if (videoOut.ScanType == ScanType.Unknown)
                videoOut.ScanType = videoIn.ScanType;

            if (videoOut.FrameHeight == 0)
                videoOut.FrameHeight = videoIn.FrameHeight;

            if (videoOut.FrameWidth == 0)
                videoOut.FrameWidth = videoIn.FrameWidth;

            if (videoOut.FrameRate == 0)
                videoOut.FrameRate = videoIn.FrameRate;

            if (videoOut.DisplayRatioHeight == 0)
                videoOut.DisplayRatioHeight = videoIn.DisplayRatioHeight;

            if (videoOut.DisplayRatioWidth == 0)
                videoOut.DisplayRatioWidth = videoIn.DisplayRatioWidth;

            if (videoOut.ScanType == ScanType.Unknown)
                videoOut.ScanType = videoIn.ScanType;

            if (audioOut.Channels == 0)
                audioOut.Channels = audioIn.Channels;

            if (audioOut.SampleRate == 0)
                audioOut.SampleRate = audioIn.SampleRate;

            if (audioOut.BitsPerSample == 0)
                audioOut.BitsPerSample = audioIn.BitsPerSample;

            return true;
        }

        public override bool Open()
        {
            _audioInputIndex = _videoInputIndex = -1;
            _audioEos = _videoEos = false;

            _audioInputIndex = SelectInput(Inputs, MediaType.Audio);
            _videoInputIndex = SelectInput(Inputs, MediaType.Video);

            if (_audioInputIndex < 0 || _videoInputIndex < 0)
            {
                _error.Facility = ErrorFacility.Codec;
                _error.Code = (int)CodecError.InvalidParams;
                _error.Message = _audioInputIndex < 0 ? 
                                 "Audio input is not configured" :
                                 "Video input is not configured";
                return false;
            }

            _audioEncoder.Inputs.Add(Inputs[_audioInputIndex]);
            _videoEncoder.Inputs.Add(Inputs[_videoInputIndex]);

            MediaPin apin;
            MediaPin vpin;

            if (!ConfigurePins(out apin, out vpin)) {
                _error.Facility = ErrorFacility.Codec;
                _error.Code = (int)CodecError.InvalidParams;
                _error.Message = "An output is not configured. It must contain 1 audio and 1 video pin";
                return false;
            }
            
            var intermediateAudioSocket = SocketFromPin(apin);
            var intermediateVideoSocket = SocketFromPin(vpin);
            if(Outputs[0].StreamType == StreamType.Mp4)
            {
                if (intermediateAudioSocket.Pins[0].StreamInfo.StreamType == StreamType.Aac)
                    intermediateAudioSocket.Pins[0].StreamInfo.StreamSubType = StreamSubType.AacAdts;

                if (intermediateVideoSocket.Pins[0].StreamInfo.StreamType == StreamType.Avc)
                    intermediateVideoSocket.Pins[0].StreamInfo.StreamSubType = StreamSubType.AvcAnnexB;
            }

            _audioEncoder.Outputs.Add(intermediateAudioSocket);
            _videoEncoder.Outputs.Add(intermediateVideoSocket);

            _mux.Outputs.Add(Outputs[0]);
            _mux.Inputs.Add((MediaSocket)intermediateAudioSocket.Clone());
            _mux.Inputs.Add((MediaSocket)intermediateVideoSocket.Clone());
            _audioMuxIndex = 0;
            _videoMuxIndex = 1;

            if (!_audioEncoder.Open())
            {
                _error = _audioEncoder.Error;
                return false;
            }

            if (!_videoEncoder.Open())
            {
                _error = _videoEncoder.Error;
                return false;
            }

            if (!_mux.Open())
            {
                _error = _mux.Error;
                return false;
            }

            if (!string.IsNullOrEmpty(AudioLog)) 
            {
                _audioLog = System.IO.File.OpenWrite(AudioLog);
            }

            return true;
        }

        public override bool Pull(out int outputIndex, MediaSample outputSample)
        {
            throw new NotImplementedException();
        }

        public override bool PullUnmanaged(out int outputIndex, MediaSample outputSample)
        {
            throw new NotImplementedException();
        }

        private void AppendSampleToFile(System.IO.FileStream fileStream, MediaSample sample)
        {
            fileStream.Write(sample.Buffer.Start, 
                             sample.Buffer.DataOffset, 
                             sample.Buffer.DataSize);
        }

        private bool EncodeAndMux(Transcoder encoder, MediaSample inputSample, int muxIndex)
        {
            int outputIndex;
            MediaSample outputSample = new MediaSample();
            bool res;

            string enc = (encoder == _audioEncoder ? "audio" : "video");

            while (true) {// encode and mux until the inputSample is fully consumed
                while (true) {// pull as much as possible from encoder and mux it

                    Trace.Write(string.Format("{0}, pull, ", enc));

                    res = encoder.Pull(out outputIndex, outputSample);

                    if (res) {

                        Trace.WriteLine(string.Format("{0}", outputSample.Buffer.DataSize));

                        if (_audioLog != null && encoder == _audioEncoder) {
                            AppendSampleToFile(_audioLog, outputSample);
                        }

                        lock (_mux) {

                            Trace.Write(string.Format("{0}, mux, ", enc));

                            res = _mux.Push(muxIndex, outputSample);

                            if (!res) {
                                _error = _mux.Error;
                                Trace.WriteLine(string.Format("Error {0} {1}", _error.Message, _error.Hint));
                                return false;
                            }

                            Trace.WriteLine("success");
                        }
                    }
                    else {
                        if (IsErrorInputNeeded(encoder.Error)) {
                            System.Diagnostics.Trace.WriteLine("InputNeeded");

                            break;
                        }

                        _error = encoder.Error;
                        System.Diagnostics.Trace.WriteLine(
                                    string.Format("Error {0} {1}", _error.Message, _error.Hint));
                        return false;
                    }
                }

                if (Util.IsSampleEmpty(inputSample))
                    break;

                System.Diagnostics.Trace.Write(
                    string.Format("{0}, push {1}, ", enc, inputSample.Buffer.DataSize));

                res = inputSample.UnmanagedBuffer != null ?
                        encoder.PushUnmanaged(0, inputSample) :
                        encoder.Push(0, inputSample);

                if (res) {
                    // the input sample is fully or partially consumed
                    System.Diagnostics.Trace.WriteLine(
                        string.Format("success:{0}", inputSample.Buffer.DataSize));

                    if (inputSample.Buffer.DataSize == 0)
                        break;
                }
                else if (IsErrorInputFull(encoder.Error)) {
                    // cannot take more input
                    Trace.WriteLine(string.Format("BufferFull:{0}", inputSample.Buffer.DataSize));
                }
                else {
                    _error = encoder.Error;
                    Trace.WriteLine(string.Format("Error {0} {1}", _error.Message, _error.Hint));
                    return false;
                }
                
            }
            return true;
        }

        public override bool Push(int inputIndex, MediaSample inputSample)
        {
            if (inputSample == null) {
                return EndOfStream(inputIndex);
            }

            if (inputIndex == _audioInputIndex) {
                return EncodeAndMux(_audioEncoder, inputSample, _audioMuxIndex);
            }
            else if (inputIndex == _videoInputIndex) {
                return EncodeAndMux(_videoEncoder, inputSample, _videoMuxIndex);
            }

            return false;
        }

        public override bool PushUnmanaged(int inputIndex, MediaSample inputSample)
        {
            return Push(inputIndex, inputSample);
        }
        
        private MediaSocketList _inputs = new MediaSocketList();
        
        public override MediaSocketList Inputs
        {
            get { return _inputs; }
        }

        private MediaSocketList _outputs = new MediaSocketList();

        public override MediaSocketList Outputs
        {
            get { return _outputs; }
        } 
        
    }
}
