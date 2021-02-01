using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Threading;

using PrimoSoftware.AVBlocks;

namespace ImageGrabber
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private object sync = new object();
        private Thread encodeThread;
        private PrimoSoftware.AVBlocks.Transcoder transcoder;
        private Queue<MediaSample> samples = new Queue<MediaSample>();
        private bool stopEncodeThread;
        private bool captureRunning;
        private string textCaptureStart = "Start Capture";
        private string textCaptureStop = "Stop Capture";
        private DateTime firstSampleTime;
        private TimeSpan runningTime;
        private int encodedFrames; // including queue
        private int droppedFrames;
        private int captureFramerate;
        private int captureFrameWidth;
        private int captureFrameHeight;
        System.Windows.Threading.DispatcherTimer timer;
        double dpiScaleX, dpiScaleY; // used to convert device independent units to pixels

        public MainWindow()
        {
            InitializeComponent();

            this.Title += (IntPtr.Size == 8 ? " 64-bit" : " 32-bit");

            this.Loaded += MainWindow_Loaded;
            this.Closing += MainWindow_Closing;

            cCapture.Content = textCaptureStart;

            PresetDescriptor selectedPreset = null;

            foreach (var preset in AvbTranscoder.Presets)
            {
                cOutputPreset.Items.Add(preset);

                if (preset.Name == PrimoSoftware.AVBlocks.Preset.Video.Fast.MP4.H264_AAC)
                    selectedPreset = preset;
            }

            if (selectedPreset != null)
            {
                cOutputPreset.SelectedItem = selectedPreset;
            }
            else
            {
                cOutputPreset.SelectedIndex = 0;
            }

            timer = new System.Windows.Threading.DispatcherTimer();
            timer.Tick += timer_Tick;

        }

        void MainWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (captureRunning)
            {
                stopEncodeThread = true;
                encodeThread.Join();
            }
        }

        void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            // get dpi scaling
            PresentationSource source = PresentationSource.FromVisual(this);
            dpiScaleX = source.CompositionTarget.TransformToDevice.M11;
            dpiScaleY = source.CompositionTarget.TransformToDevice.M22;
        }

        void timer_Tick(object sender, EventArgs e)
        {
            //var dbgtime = DateTime.Now;

            var frame = CaptureFrame();

            //var dbgtime2 = DateTime.Now;
            //Debug.WriteLine("Capture Time: {0}", (dbgtime2 - dbgtime).TotalMilliseconds);

            // Save to file
            //string filename = string.Format("Cube_{0}.png", encodedFrames);
            //SaveFrame(filename, frame);

            var sample = new MediaSample();

            if (encodedFrames > 0)
            {
                runningTime = DateTime.Now - firstSampleTime;
                sample.StartTime = runningTime.TotalSeconds;
                sample.EndTime = -1;
            }
            else
            {
                sample.StartTime = 0;
                firstSampleTime = DateTime.Now;
            }

            int stride = frame.PixelWidth * 4;
            sample.Buffer = new MediaBuffer(new byte[stride*frame.PixelHeight]);
            frame.CopyPixels(sample.Buffer.Start, stride, 0);

            cRunningTime.Text = String.Format("{0:d2}:{1:d2}.{2:d3}", runningTime.Minutes, runningTime.Seconds, runningTime.Milliseconds);

            int currentQueueCount = 0;

            lock (sync)
            {
                currentQueueCount = samples.Count;

                if (currentQueueCount < 10)
                {
                    samples.Enqueue(sample);
                    ++encodedFrames;
                }
                else
                {
                    ++droppedFrames;
                }
            }

            cEncodedFrames.Text = encodedFrames.ToString();
            cDroppedFrames.Text = droppedFrames.ToString();
            if (runningTime.TotalMilliseconds > 0)
            {
                cEffectiveFramerate.Text = string.Format("{0:f2}", (double)encodedFrames / runningTime.TotalSeconds);
            }
            cQueue.Text = (currentQueueCount < 10 ? currentQueueCount.ToString() : "MAX");
        }

        private void Capture_Click(object sender, RoutedEventArgs e)
        {
            cCapture.IsEnabled = false;

            if (captureRunning)
            {
                StopCapture();
            }
            else
            {
                StartCapture();
            }

            cCapture.IsEnabled = true;
        }

        private delegate void RenderBitmapDelegate(RenderTargetBitmap bmp);
        private void RenderBitmap(RenderTargetBitmap bmp)
        {
            bmp.Render(this.viewport);
        }


        private void StartCapture()
        {
            stopEncodeThread = false;
            encodedFrames = 0;
            droppedFrames = 0;
            samples.Clear();

            SetupTranscoder();

            if (!transcoder.Open())
            {
                LogError("Transcoder.Open", transcoder.Error);
                MessageBox.Show(transcoder.Error.Message);
                return;
            }

            encodeThread = new Thread(TranscodeWorker);
            encodeThread.Start();

            captureRunning = true;
            cCapture.Content = textCaptureStop;

            int frameIntervalMs = 1000 / captureFramerate;
            timer.Interval = new TimeSpan(0, 0, 0, 0, frameIntervalMs);
            timer.Start();
        }

        private void SetupTranscoder()
        {
            transcoder = new PrimoSoftware.AVBlocks.Transcoder();
            transcoder.AllowDemoMode = true;

            captureFramerate = Convert.ToInt32(cCaptureFramerate.Text);

            /*
             * Important!
             * Make sure the calculated viewport size is in pixels because it's used as video frame size.
             * The rendered viewport size includes the margins.
             */
            var margin = viewport.Margin;
            var frameWidth = (int)(viewport.ActualWidth * dpiScaleX + margin.Left + margin.Right);
            var frameHeight = (int)(viewport.ActualHeight * dpiScaleY + margin.Top + margin.Bottom);
            
            /*
             * Round down to even frame size.
             * This is required by the AVBlocks Transcoder
             */ 
            captureFrameWidth = frameWidth / 2 * 2;
            captureFrameHeight = frameHeight / 2 * 2;
            

            var preset = cOutputPreset.SelectedItem as PresetDescriptor;
            var output = MediaSocket.FromPreset(preset.Name);
            output.File = cOutputFile.Text;
            AdjustVideoOutput(output, captureFramerate);
            transcoder.Outputs.Add(output);

            var input = new MediaSocket()
            {
                StreamType = StreamType.UncompressedVideo
            };

            input.Pins.Add(new MediaPin()
            {
                StreamInfo = new VideoStreamInfo()
                {
                    StreamType = StreamType.UncompressedVideo,
                    FrameWidth = captureFrameWidth,
                    FrameHeight = captureFrameHeight,
                    ColorFormat = ColorFormat.BGR32,
                    ScanType = ScanType.Progressive,
                }
            });

            transcoder.Inputs.Add(input);

            File.Delete(output.File);
        }

        private void StopCapture()
        {
            timer.Stop();

            stopEncodeThread = true;
            encodeThread.Join();

            captureRunning = false;
            cCapture.Content = textCaptureStart;
        }

        private RenderTargetBitmap CaptureFrame()
        {
            /*
             * For proper scaling the bitmap must have the system DPI
             */
            RenderTargetBitmap bmp = new RenderTargetBitmap(captureFrameWidth,
                                                            captureFrameHeight,
                                                            96*dpiScaleX, 96*dpiScaleY,
                                                            PixelFormats.Pbgra32);
            bmp.Render(this.viewport);
            return bmp;
        }

        private void TranscodeWorker()
        {
            while (true)
            {

                MediaSample sample = null;

                lock (sync)
                {
                    if (samples.Count > 0)
                        sample = samples.Dequeue();

                }

                if (sample != null)
                {
                    if (!transcoder.Push(0, sample))
                    {
                        var error = transcoder.Error.Clone() as ErrorInfo;
                        LogError("Transcoder.Push", error);
                        transcoder.Close();
                        transcoder.Dispose();
                        Dispatcher.BeginInvoke(new Action<ErrorInfo>(SingalEncodeError),error);
                        return;
                    }
                }
                else
                {
                    if (stopEncodeThread)
                    {
                        transcoder.Flush();
                        transcoder.Close();
                        transcoder.Dispose();
                        return;
                    }
                    else
                    {
                        Thread.Sleep(1);
                    }
                }
            }
        }

        private static void SaveFrame(string filename, RenderTargetBitmap bmp)
        {
            PngBitmapEncoder png = new PngBitmapEncoder();

            png.Frames.Add(BitmapFrame.Create(bmp));

            using (var stream = File.Create(filename))
            {
                png.Save(stream);
            }
        }

        private static void AdjustVideoOutput(MediaSocket socket, int framerate)
        {
            foreach (MediaPin pin in socket.Pins)
            {
                if (pin.StreamInfo.MediaType == PrimoSoftware.AVBlocks.MediaType.Video)
                {
                    pin.Params[Param.Video.FrameRateConverter.Use] = Use.On;
                    pin.Params[Param.Video.FrameRateConverter.RealTime] = true;

                    if (pin.StreamInfo.StreamType == StreamType.H264)
                    {
                        pin.Params[Param.Encoder.Video.H264.FixedFramerate] = false;
                    }

                    if (framerate > 0)
                    {
                        var video = pin.StreamInfo as VideoStreamInfo;
                        video.FrameRate = framerate;
                    }
                }
            }
        }

        private static string FormatError(ErrorInfo e)
        {
            if (ErrorFacility.Success == e.Facility)
            {
               return "Success";
            }
            else
            {
                return string.Format("{0}, facility:{1} code:{2}", (e.Message ?? ""), e.Facility, e.Code);
            }
        }

        private static void LogError(string action, ErrorInfo e)
        {
            if (action != null)
            {
                Debug.Write("{0}: ", action);
            }

            Debug.WriteLine(FormatError(e));
        }

        private void cOutputPreset_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            var newFileExtension = (cOutputPreset.SelectedItem as PresetDescriptor).FileExtension;
            cOutputFile.Text = System.IO.Path.ChangeExtension(cOutputFile.Text, newFileExtension);
        }

        private void SingalEncodeError(ErrorInfo error)
        {
            StopCapture();
            MessageBox.Show(FormatError(error), "Encoding Error");
        }
    }
}
