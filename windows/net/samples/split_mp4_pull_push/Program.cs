/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.Collections.Generic;
using System.Reflection;
using System.IO;
using PrimoSoftware.AVBlocks;

namespace SplitMp4PullPushSample
{
    class Program
    {
        static int Main(string[] args)
        {
            var opt = new Options();

            if (!opt.Prepare(args))
                return opt.Error ? (int)ExitCodes.OptionsError : (int)ExitCodes.Success;

            PrimoSoftware.AVBlocks.Library.Initialize();

            // Set license information. To run AVBlocks in demo mode, comment the next line out
            // Library.SetLicense("<license-string>");

            bool splitResult = SplitFile(opt.InputFile);

            PrimoSoftware.AVBlocks.Library.Shutdown();

            return splitResult ? (int)ExitCodes.Success : (int)ExitCodes.EncodeError;
        }

        enum ExitCodes : int
        {
            Success = 0,
            OptionsError = 1,
            EncodeError = 2,
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
                Console.WriteLine("{0}, facility:{1} code:{2} hint:{3}", e.Message ?? "", e.Facility, e.Code, e.Hint ?? "");
            }
        }

        static string GetExeDir()
        {
            return Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
        }

        static string GenerateOutputFileName(string inputFile, int partNum)
        {
            return string.Format("{0}_{1:000}", Path.GetFileNameWithoutExtension(inputFile), partNum);
        }

        class SplitRecord
        {
            public string FileName;
            public double StartTime;
            public double EndTime;
            public double StartTimeActual;
            public double EndTimeActual;
        };

        static string FormatTime(double t)
        {
	        int totalSeconds = (int)t;
	        int minutes = totalSeconds / 60;
	        int seconds = totalSeconds % 60;
	        int miliseconds = (int)(Math.Floor(t * 1000 + 0.5)) % 1000;

	        return string.Format("{0:00}:{1:00}.{2:000}", minutes, seconds, miliseconds);
        }

        static bool SplitFile(string inputFile)
        {
	        string outputFileExt = ".mpg";
            string encodingPreset = Preset.Video.DVD.NTSC_4x3_PCM;
	        const double splitPartDuration = 10; // seconds

	        int audioStreamIndex = -1;
	        int videoStreamIndex = -1;

            int audioFrameSize = 0;
            int audioSampleRate = 0;

            using(var transcoder1 = new Transcoder())
            {
                // In order to use the OEM release for testing (without a valid license) the transcoder demo mode must be enabled.
                transcoder1.AllowDemoMode = true;

                using(var inputInfo = new MediaInfo())
                {
                    inputInfo.Inputs[0].File = inputFile;
                    if(!inputInfo.Open())
                    {
		                PrintError("Open MediaInfo", inputInfo.Error);
		                return false;
                    }

	                // Configure transcoder1 input and output
		            var inputSocket = MediaSocket.FromMediaInfo(inputInfo);
		            transcoder1.Inputs.Add(inputSocket);

		            for(int i = 0; i < inputSocket.Pins.Count; i++)
		            {
			            StreamInfo inputStreamInfo = inputSocket.Pins[i].StreamInfo;

			            if((inputStreamInfo.MediaType == MediaType.Video) && videoStreamIndex < 0)
			            {
				            var streamInfo = new VideoStreamInfo();

				            VideoStreamInfo inputVideoStreamInfo = inputStreamInfo as VideoStreamInfo;

				            streamInfo.ColorFormat = ColorFormat.YUV420;
				            streamInfo.StreamType = StreamType.UncompressedVideo;
				            streamInfo.ScanType = inputVideoStreamInfo.ScanType;

				            streamInfo.FrameWidth = inputVideoStreamInfo.FrameWidth;
				            streamInfo.FrameHeight = inputVideoStreamInfo.FrameHeight;
				            streamInfo.DisplayRatioWidth = inputVideoStreamInfo.DisplayRatioWidth;
				            streamInfo.DisplayRatioHeight = inputVideoStreamInfo.DisplayRatioHeight;

				            var outputPin = new MediaPin();
				            outputPin.StreamInfo = streamInfo;

				            var outputSocket = new MediaSocket();
				            outputSocket.Pins.Add(outputPin);
				            outputSocket.StreamType = streamInfo.StreamType;

				            videoStreamIndex = transcoder1.Outputs.Count;
				            transcoder1.Outputs.Add(outputSocket);
			            }
			
			            if((inputStreamInfo.MediaType == MediaType.Audio) && audioStreamIndex < 0)
			            {
				            var streamInfo = new AudioStreamInfo();

				            AudioStreamInfo inputAudioStreamInfo = inputStreamInfo as AudioStreamInfo;

				            streamInfo.StreamType = StreamType.LPCM;

				            streamInfo.PcmFlags = inputAudioStreamInfo.PcmFlags;
				            streamInfo.Channels = inputAudioStreamInfo.Channels;
				            streamInfo.SampleRate = inputAudioStreamInfo.SampleRate;
				            streamInfo.BitsPerSample = inputAudioStreamInfo.BitsPerSample;

				            var outputPin = new MediaPin();
				            outputPin.StreamInfo = streamInfo;

                            var outputSocket = new MediaSocket();
				            outputSocket.Pins.Add(outputPin);
				            outputSocket.StreamType = streamInfo.StreamType;

				            audioStreamIndex = transcoder1.Outputs.Count;
				            transcoder1.Outputs.Add(outputSocket);

                            audioFrameSize = inputAudioStreamInfo.Channels * inputAudioStreamInfo.BitsPerSample / 8;
                            audioSampleRate = inputAudioStreamInfo.SampleRate;
			            }
		            }
	            }

	            bool res = transcoder1.Open();
	            PrintError("Open Transcoder1", transcoder1.Error);
	            if (!res)
		            return false;

	            var sample = new MediaSample();
	            int outputIndex;

	            int splitPartNum = 0;
                double splitTime = splitPartDuration;
	            double partStartTime = 0;
                Transcoder transcoder2 = null;

                List<SplitRecord> splitStats = new List<SplitRecord>();

                List<MediaSample> audioSamplesQueue = new List<MediaSample>();

                try
                {
                    for (; ;)
                    {
                        if ((audioSamplesQueue.Count > 0) && (audioSamplesQueue[0].StartTime < splitTime))
                        {
                            outputIndex = audioStreamIndex;
                            sample = audioSamplesQueue[0];
                            audioSamplesQueue.RemoveAt(0);
                        }
                        else
                        {
                            if (!transcoder1.Pull(out outputIndex, sample))
                                break;

                            if ((outputIndex != audioStreamIndex) &&
                                (outputIndex != videoStreamIndex))
                            {
                                continue;
                            }
                        }

		                if(outputIndex == audioStreamIndex)
		                {
                            
			                double sampleDuration = (double)(sample.Buffer.DataSize) / (double)(audioFrameSize * audioSampleRate);
			                if(sample.StartTime >= splitTime)
			                {
				                audioSamplesQueue.Add(sample);
				                sample = new MediaSample();
				                continue;
			                } 
			                else if((sample.StartTime + sampleDuration) > splitTime)
			                {
				                double sample1Duration = splitTime - sample.StartTime;
				                int sample1BufferSize = (int)(sample1Duration * audioSampleRate) * audioFrameSize;

				                if(sample1BufferSize < sample.Buffer.DataSize)
				                {
					                int buffer2Size = sample.Buffer.DataSize - sample1BufferSize;
					                var buffer2 = new MediaBuffer(new byte[buffer2Size]);
                                    buffer2.SetData(0, buffer2Size);

                                    Array.Copy(sample.Buffer.Start, sample1BufferSize, buffer2.Start, 0, buffer2Size);

                                    var sample2 = new MediaSample();
					                sample2.StartTime = sample.StartTime + sample1Duration;
					                sample2.Buffer = buffer2;

					                if(sample1BufferSize > 0)
					                {
						                sample.Buffer.SetData(sample.Buffer.DataOffset, sample1BufferSize);
					                }
					                else
					                {
                                        sample.Buffer.SetData(0, 0);
					                }

					                audioSamplesQueue.Add(sample2);
				                }
			                }
		                }


                        if ((transcoder2 == null) ||
                            ((sample.StartTime + 0.0001 >= splitTime) && (outputIndex == videoStreamIndex)))
                        {
                            if (transcoder2 != null)
                            {
                                transcoder2.Flush();
                                transcoder2.Close();
                                transcoder2.Dispose();
                            }

                            SplitRecord splitStat = new SplitRecord();
                            splitStat.StartTime = splitTime;
                            splitStat.StartTimeActual = sample.StartTime;

                            splitPartNum += 1;
                            splitTime = splitPartNum * splitPartDuration;
                            partStartTime = sample.StartTime;

                            transcoder2 = new Transcoder();
                            transcoder2.AllowDemoMode = true;

                            // Configure transcoder2 input and output
                            {
                                for (int i = 0; i < transcoder1.Outputs.Count; i++)
                                {
                                    var streamInfo = transcoder1.Outputs[i].Pins[0].StreamInfo.Clone() as StreamInfo;
                                    var pin = new MediaPin();
                                    pin.StreamInfo = streamInfo;

                                    var socket = new MediaSocket();
                                    socket.Pins.Add(pin);
                                    socket.StreamType = streamInfo.StreamType;

                                    transcoder2.Inputs.Add(socket);
                                }

                                var outputSocket = MediaSocket.FromPreset(encodingPreset);

                                string fileName = GenerateOutputFileName(inputFile, splitPartNum) + outputFileExt;
                                string filePath = Path.Combine(GetExeDir(), fileName);

                                try
                                {
                                    File.Delete(filePath);
                                }
                                catch { }

                                outputSocket.File = filePath;
                                transcoder2.Outputs.Add(outputSocket);

                                splitStat.FileName = fileName;
                            }

                            if (splitStats.Count > 0)
                            {
                                SplitRecord lastRecord = splitStats[splitStats.Count - 1];
                                lastRecord.EndTime = splitStat.StartTime;
                                lastRecord.EndTimeActual = splitStat.StartTimeActual;
                            }

                            splitStats.Add(splitStat);

                            res = transcoder2.Open();
                            PrintError("Open Transcoder2", transcoder2.Error);
                            if (!res)
                                return false;
                        }

                        if ((splitStats.Count > 0))
                        {
                            SplitRecord lastRecord = splitStats[splitStats.Count - 1];
                            lastRecord.EndTime = sample.StartTime;
                            lastRecord.EndTimeActual = lastRecord.EndTime;
                        }

                        if (sample.StartTime >= 0)
                            sample.StartTime = sample.StartTime - partStartTime;

                        res = transcoder2.Push(outputIndex, sample);
                        if (!res)
                        {
                            PrintError("Push Transcoder2", transcoder2.Error);
                            return false;
                        }
                    }
                }
                finally
                {
                    if (transcoder2 != null)
                    {
                        transcoder2.Flush();
                        transcoder2.Close();
                        transcoder2.Dispose();
                        transcoder2 = null;
                    }
                }

	            if((transcoder1.Error.Facility != ErrorFacility.Codec) ||
			            (transcoder1.Error.Code != (int)CodecError.EOS))
	            {
		            PrintError("Pull Transcoder1", transcoder1.Error);
		            return false;
	            }

	            transcoder1.Close();

	            // print split stats
                Console.WriteLine();
                foreach(var record in splitStats)
	            {
                    Console.WriteLine("{0} start: {1} end: {2} act. start: {3} act. end: {4}", record.FileName,
                        FormatTime(record.StartTime), FormatTime(record.EndTime), FormatTime(record.StartTimeActual), FormatTime(record.EndTimeActual));
	            }
	            Console.WriteLine();
            }
	        
            return true;
        }
    }
}
