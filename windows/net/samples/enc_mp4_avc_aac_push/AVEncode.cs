using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using PrimoSoftware.AVBlocks;

namespace EncMp4AvcAacPushSample
{
    class TrackState
    {
        public const int Disabled = -1;
        public MediaSample Frame;
        public int Index = Disabled;
        public int FrameCount = 0;
        public double Progress = -1.0;
    };

    public class UncompressedAVSplitter: IDisposable
    {
        private void Init(VideoStreamInfo vinfo)
        {
            if (vinfo.StreamType == StreamType.UncompressedVideo)
            {
                // YUV 420
                uncompressedFrameSize = vinfo.FrameWidth * vinfo.FrameHeight * 3 / 2;
                inframe.Buffer = new MediaBuffer(new byte[uncompressedFrameSize]);
                inframe.Buffer.SetData(0, 0); // needed because setting the buffer actually sets the data as well
            }
            else
            {
               throw new Exception("Unsupported: video input is compressed");
            }

            frameRate = vinfo.FrameRate;
        }

        private void Init(AudioStreamInfo ainfo)
        {
            if (ainfo.StreamType == StreamType.LPCM)
            {
                if (0 == ainfo.BytesPerFrame)
                {
                    ainfo.BytesPerFrame = ainfo.BitsPerSample / 8 * ainfo.Channels;
                }
                frameRate = 10;
                uncompressedFrameSize = ainfo.BytesPerFrame * ainfo.SampleRate / (int)frameRate;
                inframe.Buffer = new MediaBuffer(new byte[uncompressedFrameSize]);
                inframe.Buffer.SetData(0, 0); // needed because setting the buffer actually sets the data as well
            }
            else
            {
                throw new Exception("Unsupported: audio input is compressed");
            }
        }

        public void Init(MediaSocket socket, string filename)
        {
            if (socket == null || string.IsNullOrEmpty(filename))
            {
                return;
            }

            infile = new System.IO.FileStream(filename, FileMode.Open, FileAccess.Read);

            StreamInfo sinfo = socket.Pins[0].StreamInfo;
            mediaType = sinfo.MediaType;

            switch (mediaType)
            {
                case MediaType.Video:
                    Init(sinfo as VideoStreamInfo);
                    break;

                case MediaType.Audio:
                    Init(sinfo as AudioStreamInfo);
                    break;

                default:
                    throw new Exception("Unsupported: unknown media type");
            }

            eos = false;
        }

        public MediaSample GetFrame()
        {
            if (eos)
                return null;

            if (mediaType == MediaType.Video)
            {
                int bytesRead = infile.Read(inframe.Buffer.Start, 0, uncompressedFrameSize);

                if (bytesRead < uncompressedFrameSize)
                {
                    eos = true;
                }
                else
                {
                    inframe.Buffer.SetData(0, uncompressedFrameSize);
                    inframe.StartTime = inframeCount / frameRate;
                    inframe.EndTime = -1.0;
                    ++inframeCount;
                }
            }
            else if (mediaType == MediaType.Audio)
            {
                if (inframe.Buffer.DataSize == 0)
                {
                    int bytesRead = infile.Read(inframe.Buffer.Start, 0, uncompressedFrameSize);

                    if (0 == bytesRead)
                    {
                        eos = true;
                    }
                    else
                    {
                        inframe.Buffer.SetData(0, bytesRead);
                        inframe.StartTime = inframeCount / frameRate;
                        inframe.EndTime = -1;
                        ++inframeCount;
                    }
                }
            }

            return (eos ? null : inframe);
        }

        public void Dispose()
        {
            if (null != infile)
            {
                infile.Dispose();
            }
        }

        private MediaType mediaType = MediaType.Unknown;
        private MediaSample inframe = new MediaSample();
        private System.IO.FileStream infile;
        private int uncompressedFrameSize;
        private int inframeCount;
        private double frameRate;
        private bool eos = true;
    };

    public static class AVEncode
    {
        private static TrackState SelectMuxTrack(TrackState vtrack, TrackState atrack)
        {
            if (vtrack.Index != TrackState.Disabled && atrack.Index != TrackState.Disabled)
            {
                if (vtrack.Frame == null)
                    return vtrack;

                if (atrack.Frame == null)
                    return atrack;

                return vtrack.Frame.StartTime <= atrack.Frame.StartTime ? vtrack : atrack;
            }

            if (vtrack.Index != TrackState.Disabled)
                return vtrack;

            if (atrack.Index != TrackState.Disabled)
                return atrack;

            return null;
        }

        static void PrintError(string action, ErrorInfo e)
        {
            if (action != null)
            {
                Console.Write("{0}: ", action);
            }

            if (ErrorFacility.Success == e.Facility)
            {
                Console.WriteLine("Success");
                return;
            }
            else
            {
                Console.WriteLine("{0}, facility:{1} code:{2}", e.Message ?? "", e.Facility, e.Code);
            }
        }

        /*
        uncompressed audio and video input
        */
        public static bool Run(MediaSocket vinput, string vfile, MediaSocket ainput, string afile, MediaSocket output)
        {
            bool res;

            TrackState vtrack = new TrackState();
            TrackState atrack = new TrackState();

            using (UncompressedAVSplitter vsplit = new UncompressedAVSplitter())
            using (UncompressedAVSplitter asplit = new UncompressedAVSplitter())
            using (var transcoder = new Transcoder() { AllowDemoMode = true })
            {
                try
                {
                    if (vinput != null)
                    {
                        Console.WriteLine("video input file: \"{0}\"", vfile);
                        vsplit.Init(vinput, vfile);
                        Console.WriteLine("OK");
                    }

                    if (ainput != null)
                    {
                        Console.WriteLine("audio input file: \"{0}\"", afile);
                        asplit.Init(ainput, afile);
                        Console.WriteLine("OK");
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                    return false;
                }

                // setup transcoder
                int trackIndex = 0; // start index

                if (vinput != null)
                {
                    transcoder.Inputs.Add(vinput);
                    vtrack.Index = trackIndex++;
                }

                if (ainput != null)
                {
                    transcoder.Inputs.Add(ainput);
                    atrack.Index = trackIndex++;
                }

                transcoder.Outputs.Add(output);

                res = transcoder.Open();
                PrintError("transcoder open", transcoder.Error);
                if (!res)
                    return false;

                // transcoding loop
                for (;;)
                {
                    if (vtrack.Index != TrackState.Disabled && vtrack.Frame == null)
                    {
                        vtrack.Frame = vsplit.GetFrame();
                    }

                    if (atrack.Index != TrackState.Disabled && atrack.Frame == null)
                    {
                        atrack.Frame = asplit.GetFrame();
                    }

                    TrackState track = SelectMuxTrack(vtrack, atrack);

                    if (track == null)
                        break;

                    // log
                    if (track.Frame != null)
                    {
                        if (track.Frame.StartTime - track.Progress >= 1.0)
                        {
                            track.Progress = track.Frame.StartTime;
                            Console.WriteLine("track {0} frame #{1} pts:{2}", track.Index, track.FrameCount, track.Frame.StartTime);
                        }
                    }
                    else
                    {
                        Console.WriteLine("track {0} eos", track.Index);
                    }

                    if (track.Frame != null)
                    {
                        res = transcoder.Push(track.Index, track.Frame);
                        if (!res)
                        {
                            PrintError("transcoder push frame", transcoder.Error);
                            return false;
                        }
                        track.Frame = null; // clear the muxed frame in order to read to the next one
                        track.FrameCount++;
                    }
                    else
                    {
                        res = transcoder.Push(track.Index, null);
                        if (!res)
                        {
                            PrintError("transcoder push eos", transcoder.Error);
                            return false;
                        }
                        track.Index = TrackState.Disabled; // disable track
                    }
                }

                res = transcoder.Flush();
                if (!res)
                {
                    PrintError("transcoder flush", transcoder.Error);
                    return false;
                }
                transcoder.Close();
            }

            return true;
        }
    }
}
