/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

using System;
using PrimoSoftware.AVBlocks;
using System.IO;

namespace EncMpgDvdSample
{
    class OutputStream : System.IO.Stream
    {
        private System.IO.Stream stream_;
        string filename_;

        public OutputStream(string filename)
        {
            filename_ = filename;
            stream_ = new FileStream(filename, FileMode.Create, FileAccess.Write);

        }

        ~OutputStream()
        {
            Dispose(false);
        }

        protected override void Dispose(bool disposing)
        {
            stream_.Dispose();
        }

        public override bool CanRead
        {
            get { return false; }
        }

        public override bool CanSeek
        {
            get { return true; }
        }

        public override bool CanWrite
        {
            get { return true; }
        }

        public override long Length
        {
            get { return stream_.Length; }
        }

        public override long Position
        {
            get { return stream_.Position; }

            set { stream_.Position = value; }
        }

        public override void Flush()
        {
            stream_.Flush();
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            return stream_.Seek(offset, origin);
        }

        public override void SetLength(long value)
        {
            stream_.SetLength(value);
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            throw new NotSupportedException();
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            stream_.Write(buffer, offset, count);
        }
    }

    class Splitter
    {
        public int SplitTime
        {
            set; get;
        }

        public Int64 SplitSize
        {
            set; get;
        }

        public bool ReportProgress
        {
            set; get;
        }

        public bool TranscodeSplit( Options opt, string outFile, string preset, double startTime, 
                                    out double processedTime, out Int64 processedSize, out bool isSplit)
        {
            DeleteFile(outFile);
            processedSize = 0;
            processedTime = 0;
            isSplit = false;

            using (var transcoder = new Transcoder() { AllowDemoMode = true })
            {
                // Input
                var inSocket = new MediaSocket();
                inSocket.File = opt.InputFile;
                inSocket.TimePosition = startTime;
                transcoder.Inputs.Add(inSocket);
                
                using (var outputStream = new OutputStream(outFile))
                {
                    // Output
                    MediaSocket outSocket = MediaSocket.FromPreset(preset);
                    outSocket.Stream = outputStream;
                    transcoder.Outputs.Add(outSocket);

                    outputStream_ = outputStream;
                    transcoder.OnContinue += Transcoder_OnContinue;
                    transcoder.OnProgress += Transcoder_OnProgress;
                    ReportProgress = false;

                    if (opt.SplitTime > 0)
                        SplitTime = opt.SplitTime;

                    if (opt.SplitSize > 0)
                        SplitSize = opt.SplitSize;

                    if (!transcoder.Open())
                    {
                        PrintError("Transcoder open", transcoder.Error);
                        return false;
                    }

                    if (!transcoder.Run())
                    {
                        PrintError("Transcoder run", transcoder.Error);
                        return false;
                    }

                    transcoder.Close();
                    isSplit = isSplit_;
                    processedSize = outputStream.Length;
                    processedTime = processedTime_;
                }
            }

            return true;
        }

        private void Transcoder_OnProgress(object sender, TranscoderProgressEventArgs args)
        {
            processedTime_ = Math.Max(processedTime_, args.CurrentTime);

            if (ReportProgress)
                Console.WriteLine("progress: {0:F1} sec.", args.CurrentTime);
        }

        private void Transcoder_OnContinue(object sender, TranscoderContinueEventArgs args)
        {
            processedTime_ = Math.Max(processedTime_, args.CurrentTime);

            if (outputStream_ != null && SplitSize > 0 && (outputStream_.Length > SplitSize))
            {
                isSplit_ = true;
                args.Continue = false;
                return;
            }

            if (SplitTime > 0.0 && (SplitTime <= args.CurrentTime))
            {
                isSplit_ = true;
                args.Continue = false;
                return;
            }

            args.Continue = true;
        }

        static void DeleteFile(string filename)
        {
            try
            {
                if (System.IO.File.Exists(filename))
                    System.IO.File.Delete(filename);
            }
            catch { }
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
                Console.WriteLine("{0}, facility:{1} code:{2} hint:{3}", e.Message, (int)e.Facility, e.Code, e.Hint);
            }
        }

        private double processedTime_;
        private OutputStream outputStream_;
        private bool isSplit_;
    }

    class StreamDuration
    {
        public static double GetMinDuration(string file)
        {
            double duration = 0;
            bool  durationInitialized = false;

            using (MediaInfo info = new MediaInfo())
            {
                info.Inputs[0].File = file;

                if (info.Open())
                {
                    foreach(var socket in info.Outputs)
                    {
                        foreach(var pin in socket.Pins)
                        {
                            if(durationInitialized)
                            {
                                duration = Math.Min(duration, pin.StreamInfo.Duration);
                            }
                            else
                            {
                                durationInitialized = true;
                                duration = pin.StreamInfo.Duration;
                            }
                        }
                    }
                }
            }

            return duration;
        }
    }
}
