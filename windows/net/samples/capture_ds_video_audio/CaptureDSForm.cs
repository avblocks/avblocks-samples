using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.IO;
using System.Diagnostics;
using System.Threading;
using System.Linq;

using DirectShowLib;
using PrimoSoftware.AVBlocks;

namespace CaptureDS
{
    public partial class CaptureDSForm : Form
    {
        private MediaState ms = new MediaState();

        private bool bIgnoreDeviceSelection;
        private bool bRecording;
        private bool bCmdRecordBusy;
        const string sStartRecording = "Start Recording";
        const string sStopRecording = "Stop Recording";

        private System.Windows.Forms.Timer statsTimer;
        DateTime recStartTime;
        DateTime fpsStartTime; // current fps
        int fpsNotDropped; // current fps

        private SampleGrabberCB m_videoCB = new SampleGrabberCB("VideoGrabberCB");
        private SampleGrabberCB m_audioCB = new SampleGrabberCB("AudioGrabberCB");
        private List<char> m_selDrives = new List<char>();
        private IntPtr m_mainWindow = IntPtr.Zero;

        public CaptureDSForm()
        {
            InitializeComponent();
#if WIN64
            this.Text += " (64-bit)";
#endif

            EnumInputDev(FilterCategory.AudioInputDevice, listAudioDev);
            listAudioDev.Items.Insert(0, new DevItem()
            {
                FriendlyName = "[ No audio input ]",
                //  DisplayName = null
            });

            EnumInputDev(FilterCategory.VideoInputDevice, listVideoDev);

            statsTimer = new System.Windows.Forms.Timer();
            statsTimer.Tick += new EventHandler(UpdateStats);
            statsTimer.Interval = 500;

            bIgnoreDeviceSelection = false;
            bRecording = false;
            cmdRecord.Text = sStartRecording;
            txtRecording.Visible = false;

            ResetStats();

            int h = previewBox.Size.Height;
            int w = previewBox.Size.Width;
            previewBox.Size = new Size(w, w * 3 / 4);

            for (int i = 0; i < AvbTranscoder.Presets.Length; ++i)
            {
                PresetDescriptor preset = AvbTranscoder.Presets[i];

                if (!preset.AudioOnly)
                {
                    comboPresets.Items.Add(preset);
                }
            }

            comboPresets.SelectedIndex = 0;
        }

        void UpdateStats(object sender, EventArgs e)
        {
            DateTime now = DateTime.Now;
            TimeSpan rec = now - recStartTime;
            txtRecTime.Text = string.Format("{0}:{1:d2}:{2:d2}", rec.Hours, rec.Minutes, rec.Seconds);

            if (ms.droppedFrames != null)
            {
                int hr = 0;
                int dropped;
                hr = ms.droppedFrames.GetNumDropped(out dropped);
                if (0 == hr)
                    txtNumDropped.Text = dropped.ToString();

                int notDropped;
                hr = ms.droppedFrames.GetNumNotDropped(out notDropped);

                if (0 == hr)
                {
                    txtNumNotDropped.Text = notDropped.ToString();
                    if (notDropped >= 0)
                    {
                        double averageFPS = (double)notDropped / rec.TotalSeconds;
                        txtAverageFPS.Text = averageFPS.ToString("F3");

                        TimeSpan tsfps = now - fpsStartTime;
                        double fpsElapsed = tsfps.TotalSeconds;
                        if (fpsElapsed > 5.0)
                        {
                            double curFPS = (double)(notDropped - fpsNotDropped) / fpsElapsed;
                            txtCurrentFPS.Text = curFPS.ToString("F3");

                            fpsStartTime = now;
                            fpsNotDropped = notDropped;
                        }
                    }
                }
            }

            txtACallbacks.Text = m_audioCB.SampleIndex.ToString();
            txtAProcessed.Text = m_audioCB.ProcessedSamples.ToString();
            txtADropped.Text = m_audioCB.DroppedSamples.ToString();

            txtVCallbacks.Text = m_videoCB.SampleIndex.ToString();
            txtVProcessed.Text = m_videoCB.ProcessedSamples.ToString();
            txtVDropped.Text = m_videoCB.DroppedSamples.ToString();
        }

        void ResetStats()
        {
            string sEmpty = "--";

            txtRecTime.Text = "--:--:--";

            txtNumDropped.Text = sEmpty;
            txtNumNotDropped.Text = sEmpty;
            txtAverageFPS.Text = sEmpty;
            txtCurrentFPS.Text = sEmpty;

            txtACallbacks.Text = sEmpty;
            txtADropped.Text = sEmpty;
            txtAProcessed.Text = sEmpty;

            txtVCallbacks.Text = sEmpty;
            txtVDropped.Text = sEmpty;
            txtVProcessed.Text = sEmpty;
        }

        void EnumInputDev(Guid filterCategory, ComboBox list)
        {
            if (list == null)
                return;

            ICreateDevEnum devEnum = null;
            IEnumMoniker enumCat = null;
            IMoniker[] moniker = new IMoniker[1] { null };
            IPropertyBag propBag = null;

            int hr = 0;
            try
            {

                devEnum = new CreateDevEnum() as ICreateDevEnum;
                if (devEnum == null)
                    throw new COMException("Cannot create CLSID_SystemDeviceEnum");

                // Obtain enumerator for the capture category.
                hr = devEnum.CreateClassEnumerator(filterCategory, out enumCat, 0);
                DsError.ThrowExceptionForHR(hr);

                if (enumCat == null)
                {
                    if (filterCategory == FilterCategory.AudioInputDevice)
                        MessageBox.Show("No audio devices found");
                    else if (filterCategory == FilterCategory.VideoInputDevice)
                        MessageBox.Show("No video devices found");

                    return;
                }

                // Enumerate the monikers.
                IntPtr fetchedCount = new IntPtr(0);
                while (0 == enumCat.Next(1, moniker, fetchedCount))
                {

                    Guid bagId = typeof(IPropertyBag).GUID;
                    object bagObj;
                    moniker[0].BindToStorage(null, null, ref bagId, out bagObj);

                    propBag = bagObj as IPropertyBag;

                    if (propBag != null)
                    {
                        object val;
                        string friendlyName = null;
                        string displayName = null;

                        hr = propBag.Read("FriendlyName", out val, null);
                        if (hr == 0)
                        {
                            friendlyName = val as string;
                        }
                        Util.ReleaseComObject(ref propBag);

                        moniker[0].GetDisplayName(null, null, out displayName);

                        // create an instance of the filter
                        Guid baseFilterId = typeof(IBaseFilter).GUID;
                        object filter;
                        bool addFilter = false;
                        try
                        {
                            moniker[0].BindToObject(null, null, ref baseFilterId, out filter);
                            if (filter != null)
                            {
                                addFilter = true;
                                Util.ReleaseComObject(ref filter);
                            }
                        }
                        catch
                        {
                            System.Diagnostics.Trace.WriteLine("Cannot use input device " + friendlyName);
                        }

                        if (addFilter == true &&
                            friendlyName != null &&
                            displayName != null)
                        {
                            DevItem fi = new DevItem(friendlyName, displayName);
                            list.Items.Add(fi);
                        }
                    } // if IPropertyBag

                    Util.ReleaseComObject(ref moniker[0]);
                } // while enum devices
            }
            catch (COMException ex)
            {
                MessageBox.Show(ex.Message);
            }
            finally
            {
                Util.ReleaseComObject(ref propBag);
                Util.ReleaseComObject(ref moniker[0]);
                Util.ReleaseComObject(ref enumCat);
                Util.ReleaseComObject(ref devEnum);
            }

        } // EnumInputDev

        private int InitInputDev(MediaState ms, DevItem videoItem, DevItem audioItem)
        {
            int hr = 0;
            // create Filter Graph Manager
            if (ms.graph == null)
            {
                //ms.graph = new FilterGraph() as IGraphBuilder;
                ms.graph = new FilterGraph() as IFilterGraph2;
                if (ms.graph == null)
                {
                    throw new COMException("Cannot create FilterGraph");
                }

                ms.captureGraph = new CaptureGraphBuilder2() as ICaptureGraphBuilder2;
                if (ms.captureGraph == null)
                {
                    throw new COMException("Cannot create CaptureGraphBuilder2");
                }

                hr = ms.captureGraph.SetFiltergraph(ms.graph);
                DsError.ThrowExceptionForHR(hr);
            }


            if (audioItem != null)
            {
                // remove the old audio input
                if (ms.audioInput != null)
                {
                    hr = ms.graph.RemoveFilter(ms.audioInput);
                    Util.ReleaseComObject(ref ms.audioInput);
                    DsError.ThrowExceptionForHR(hr);
                }

                // create audio input
                if (!audioItem.Disabled)
                {
                    // using BindToMoniker
                    ms.audioInput = Marshal.BindToMoniker(audioItem.DisplayName) as IBaseFilter;

                    // add audio input to the graph
                    hr = ms.graph.AddFilter(ms.audioInput, audioItem.FriendlyName);
                    DsError.ThrowExceptionForHR(hr);
                }
            }


            if (videoItem != null)
            {
                // remove the old video input
                if (ms.videoInput != null)
                {
                    hr = ms.graph.RemoveFilter(ms.videoInput);
                    Util.ReleaseComObject(ref ms.videoInput);
                    DsError.ThrowExceptionForHR(hr);
                }

                // create video input

                // Using BindToMoniker
                ms.videoInput = Marshal.BindToMoniker(videoItem.DisplayName) as IBaseFilter;

                // add video input to the graph
                hr = ms.graph.AddFilter(ms.videoInput, videoItem.FriendlyName);
                DsError.ThrowExceptionForHR(hr);
            }

            return hr;
        }

        private void RecorderForm_Load(object sender, EventArgs e)
        {
            try
            {
                bIgnoreDeviceSelection = true;

                listAudioDev.SelectedIndex = (listAudioDev.Items.Count > 1) ? 1 : 0;

                if (listVideoDev.Items.Count > 0)
                    listVideoDev.SelectedIndex = 0;

                bIgnoreDeviceSelection = false;

                DevItem videoItem = (DevItem)listVideoDev.SelectedItem;
                DevItem audioItem = (DevItem)listAudioDev.SelectedItem;

                int hr = InitInputDev(ms, videoItem, audioItem);
                DsError.ThrowExceptionForHR(hr);

                if (hr != 0)
                    Trace.WriteLine("Cannot use the selected capture devices");

                BuildGraph();

                ms.rot = new DsROTEntry(ms.graph);

                m_mainWindow = this.Handle;
            }
            catch (COMException ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void listAudioDev_SelectedIndexChanged(object sender, EventArgs e)
        {
            DevItem item = (DevItem)listAudioDev.SelectedItem;
            int hr;
            if (item != null)
            {
                if (!bIgnoreDeviceSelection)
                {
                    ClearGraph();

                    try
                    {
                        hr = InitInputDev(ms, null, item);
                        DsError.ThrowExceptionForHR(hr);
                    }
                    catch (COMException ex)
                    {
                        MessageBox.Show(ex.ToString());
                        return;
                    }

                    BuildGraph();
                }
            }
        }

        private void listVideoDev_SelectedIndexChanged(object sender, EventArgs e)
        {
            DevItem item = (DevItem)listVideoDev.SelectedItem;
            int hr;
            if (item != null)
            {
                if (!bIgnoreDeviceSelection)
                {
                    ClearGraph();

                    try
                    {
                        hr = InitInputDev(ms, item, null);
                        DsError.ThrowExceptionForHR(hr);
                    }
                    catch (COMException ex)
                    {
                        MessageBox.Show(ex.ToString());
                        return;
                    }

                    BuildGraph();
                }
            }
        }

        private void cmdAudioDevProp_Click(object sender, EventArgs e)
        {
            if (!CheckInputDevice(ms.audioInput))
                return;

            ClearGraph();

            ShowPropPages(ms.audioInput);

            BuildGraph();
        }

        private bool CheckInputDevice(IBaseFilter inputDevice)
        {
            if (inputDevice == null)
            {
                MessageBox.Show("No input device is selected!");
                return false;
            }
            return true;
        }

        private void ShowPropPages(object obj)
        {
            ISpecifyPropertyPages specPropPages = null;

            try
            {
                specPropPages = obj as ISpecifyPropertyPages;
                if (null == specPropPages)
                {
                    MessageBox.Show("Property pages not available");
                    return;
                }

                DsCAUUID cauuid;
                int hr = specPropPages.GetPages(out cauuid);
                DsError.ThrowExceptionForHR(hr);

                if (hr == 0 && cauuid.cElems > 0)
                {
                    // show property pages
                    hr = WinAPI.OleCreatePropertyFrame(this.Handle,
                        30, 30, null, 1,
                        ref obj, cauuid.cElems,
                        cauuid.pElems, 0, 0, IntPtr.Zero);

                    Marshal.FreeCoTaskMem(cauuid.pElems);

                }
            }
            catch (COMException ex)
            {
                MessageBox.Show(ex.ToString());
            }

            //do not release interfaces obtained with a cast (as), the primary interface is also released
        }

        private void cmdVideoDevProp_Click(object sender, EventArgs e)
        {
            if (!CheckInputDevice(ms.videoInput))
                return;

            ClearGraph();

            ShowPropPages(ms.videoInput);

            BuildGraph();
        }

        private void cmdVideoCaptureProp_Click(object sender, EventArgs e)
        {
            if (!CheckInputDevice(ms.videoInput))
                return;

            int hr = 0;
            Guid streamConfigId = typeof(IAMStreamConfig).GUID;
            object streamConfigObj = null;

            ClearGraph();

            try
            {
                hr = ms.captureGraph.FindInterface(PinCategory.Capture, DirectShowLib.MediaType.Video,
                                                    ms.videoInput, streamConfigId, out streamConfigObj);

                DsError.ThrowExceptionForHR(hr);


                using (VideoCapturePropertiesForm form = new VideoCapturePropertiesForm())
                {
                    form.VideoConfig = streamConfigObj as IAMStreamConfig;
                    form.ShowDialog();
                }
            }
            catch (COMException ex)
            {
                MessageBox.Show(ex.ToString());
            }
            finally
            {
                Util.ReleaseComObject(ref streamConfigObj);
            }

            BuildGraph();
        }

        private bool BuildGraph()
        {
            int hr = 0;
            bool cleanup = true;
            object audioConfigObj = null;
            IAMStreamConfig audioConfig = null;

            cmdRecord.Enabled = false;

            try
            {
                if (ms.audioInput != null)
                {
                    // Create the Audio Sample Grabber.
                    ms.audioSampleGrabber = new SampleGrabber() as IBaseFilter;
                    if (ms.audioSampleGrabber == null)
                    {
                        throw new COMException("Cannot create SampleGrabber");
                    }

                    hr = ms.graph.AddFilter(ms.audioSampleGrabber, "Audio SampleGrabber");
                    DsError.ThrowExceptionForHR(hr);

                    ms.audioGrabber = ms.audioSampleGrabber as ISampleGrabber;
                    if (ms.audioGrabber == null)
                    {
                        throw new COMException("Cannot obtain ISampleGrabber");
                    }

                    // Create and add the audio null renderer in the graph
                    ms.audioNullRenderer = new NullRenderer() as IBaseFilter;
                    if (ms.audioNullRenderer == null)
                    {
                        throw new COMException("Cannot create NullRenderer");
                    }

                    hr = ms.graph.AddFilter(ms.audioNullRenderer, "Audio NullRenderer");
                    DsError.ThrowExceptionForHR(hr);
                    
                    // manually connect the filters
                    hr = Util.ConnectFilters(ms.graph, ms.audioInput, ms.audioSampleGrabber);
                    DsError.ThrowExceptionForHR(hr);

                    hr = Util.ConnectFilters(ms.graph, ms.audioSampleGrabber, ms.audioNullRenderer);
                    DsError.ThrowExceptionForHR(hr);
                }

                ms.videoSampleGrabber = new SampleGrabber() as IBaseFilter;
                if (ms.videoSampleGrabber == null)
                {
                    throw new COMException("Cannot create SampleGrabber");
                }

                hr = ms.graph.AddFilter(ms.videoSampleGrabber, "Video SampleGrabber");
                DsError.ThrowExceptionForHR(hr);

                ms.videoGrabber = ms.videoSampleGrabber as ISampleGrabber;
                if (ms.videoGrabber == null)
                {
                    throw new COMException("Cannot obtain ISampleGrabber");
                }

                // Create and add the video null renderer in the graph
                ms.videoNullRenderer = new NullRenderer() as IBaseFilter;
                if (ms.videoNullRenderer == null)
                {
                    throw new COMException("Cannot create NullRenderer");
                }

                hr = ms.graph.AddFilter(ms.videoNullRenderer, "Video NullRenderer");
                DsError.ThrowExceptionForHR(hr);

                // add the smart tee if preview is required
                if (ms.smartTee == null)
                {
                    ms.smartTee = new SmartTee() as IBaseFilter;
                    hr = ms.graph.AddFilter(ms.smartTee, "Smart Tee");
                    DsError.ThrowExceptionForHR(hr);
                }

                if (ms.smartTee != null)
                {
                    // connect the video input to the smart tee
                    Util.ConnectFilters(ms.graph, ms.videoInput, ms.smartTee);

                    // connect smart tee capture to video grabber
                    IPin capturePin;
                    hr = Util.GetPin(ms.smartTee, PinDirection.Output, "Capture", out capturePin);
                    DsError.ThrowExceptionForHR(hr);

                    IPin videoGrabberPin;
                    hr = Util.GetUnconnectedPin(ms.videoSampleGrabber, PinDirection.Input, out videoGrabberPin);
                    DsError.ThrowExceptionForHR(hr);

                    hr = ms.graph.ConnectDirect(capturePin, videoGrabberPin, null);
                    DsError.ThrowExceptionForHR(hr);

                    // connect smart tee preview to video renderer
                    ms.previewRenderer = new VideoRendererDefault() as IBaseFilter;
                    hr = ms.graph.AddFilter(ms.previewRenderer, "Preview Renderer");
                    DsError.ThrowExceptionForHR(hr);

                    IPin previewPin;
                    hr = Util.GetPin(ms.smartTee, PinDirection.Output, "Preview", out previewPin);
                    DsError.ThrowExceptionForHR(hr);

                    IPin videoRendererPin;
                    hr = Util.GetUnconnectedPin(ms.previewRenderer, PinDirection.Input, out videoRendererPin);
                    DsError.ThrowExceptionForHR(hr);

                    hr = ms.graph.Connect(previewPin, videoRendererPin);
                    DsError.ThrowExceptionForHR(hr);
                }
                else
                {
                    hr = Util.ConnectFilters(ms.graph, ms.videoInput, ms.videoSampleGrabber);
                    DsError.ThrowExceptionForHR(hr);
                }

                hr = Util.ConnectFilters(ms.graph, ms.videoSampleGrabber, ms.videoNullRenderer);
                DsError.ThrowExceptionForHR(hr);

                ms.mediaControl = ms.graph as IMediaControl;
                if (null == ms.mediaControl)
                {
                    throw new COMException("Cannot obtain IMediaControl");
                }

                if (ms.audioInput != null)
                {
                    // configure input audio
                    Guid streamConfigId = typeof(IAMStreamConfig).GUID;
                    hr = ms.captureGraph.FindInterface(PinCategory.Capture, null, ms.audioInput,
                        streamConfigId, out audioConfigObj);
                    DsError.ThrowExceptionForHR(hr);

                    audioConfig = audioConfigObj as IAMStreamConfig;
                    if (null == audioConfig)
                        throw new COMException("Cannot obtain IAMStreamConfig");

                    AMMediaType audioType;
                    hr = audioConfig.GetFormat(out audioType);
                    DsError.ThrowExceptionForHR(hr);

                    WaveFormatEx wfx = new WaveFormatEx();
                    Marshal.PtrToStructure(audioType.formatPtr, wfx);

                    // set audio capture parameters
                    wfx.nSamplesPerSec = 48000;
                    wfx.nChannels = 2;
                    wfx.wBitsPerSample = 16;
                    wfx.nBlockAlign = (short)(wfx.nChannels * wfx.wBitsPerSample / 8);
                    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
                    //wfx.wFormatTag = 1; // PCM
                    Marshal.StructureToPtr(wfx, audioType.formatPtr, false);
                    hr = audioConfig.SetFormat(audioType);
                    DsUtils.FreeAMMediaType(audioType);
                    DsError.ThrowExceptionForHR(hr);

                    // Store the audio media type for later use.
                    hr = ms.audioGrabber.GetConnectedMediaType(ms.audioType);
                    DsError.ThrowExceptionForHR(hr);
                }
                else
                {
                    // There's no audio media type because the audio input is disabled
                    ms.audioType.majorType = DirectShowLib.MediaType.Null;
                }

                try
                {
                    ms.droppedFrames = ms.videoInput as IAMDroppedFrames;
                    //the video capture device may not support IAMDroppedFrames
                }
                catch { }


                // Store the video media type for later use.
                hr = ms.videoGrabber.GetConnectedMediaType(ms.videoType);
                DsError.ThrowExceptionForHR(hr);

                IVideoWindow ivw = ms.graph as IVideoWindow;
                try
                {
                    hr = ivw.put_Owner(previewBox.Handle);
                    DsError.ThrowExceptionForHR(hr);

                    hr = ivw.put_WindowStyle(WindowStyle.Child | WindowStyle.ClipChildren | WindowStyle.ClipSiblings);
                    DsError.ThrowExceptionForHR(hr);

                    hr = ivw.put_Visible(OABool.True);
                    DsError.ThrowExceptionForHR(hr);

                    Rectangle rc = previewBox.ClientRectangle;
                    hr = ivw.SetWindowPosition(0, 0, rc.Right, rc.Bottom);
                    DsError.ThrowExceptionForHR(hr);

                }
                catch { }

                hr = ms.mediaControl.Run();
                DsError.ThrowExceptionForHR(hr);

                cleanup = false;
            }
            catch (COMException ex)
            {
                MessageBox.Show(ex.ToString());
                return false;
            }
            finally
            {
                Util.ReleaseComObject(ref audioConfigObj);
                if (cleanup)
                {
                    ms.Reset(false);
                    m_audioCB.Reset();
                    m_videoCB.Reset();
                }
            }

            cmdRecord.Enabled = true;

            return true;
        }

        private void ClearGraph()
        {
            ms.Reset(false); // leave the input devices in the graph
        }

        private bool StartRecording()
        {
            if (null == ms.videoInput)
            {
                MessageBox.Show("No video input!");
                return false;
            }

	        if (txtOutput.Text.Length == 0)
	        {
                MessageBox.Show("Please choose output file.");
		        return false;
	        }

            if (System.IO.File.Exists(txtOutput.Text)) {

                //string prompt = string.Format("{0} already exists. Do you want to replace the file?", txtOutput.Text);
                //if (DialogResult.Yes != MessageBox.Show(prompt, "Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
                //    return false;

                try {
                    File.Delete(txtOutput.Text);
                }
                catch { }
            }

            if (txtAudioLog.Text.Length > 0) {
                try {
                    File.Delete(txtAudioLog.Text);
                }
                catch { }
            }

            int hr = 0;
            try
            {

                hr = ms.mediaControl.Stop();
                DsError.ThrowExceptionForHR(hr);

                if (ms.audioInput != null)
                {
                    hr = ms.audioGrabber.SetCallback(m_audioCB, (int)CBMethod.Sample);
                    DsError.ThrowExceptionForHR(hr);
                }

                hr = ms.videoGrabber.SetCallback(m_videoCB, (int)CBMethod.Sample);
                DsError.ThrowExceptionForHR(hr);

                m_audioCB.Reset();
                m_videoCB.Reset();

                // pass the media state to the callbacks
                m_audioCB.MediaState = ms;
                m_videoCB.MediaState = ms;
                m_audioCB.MainWindow = m_mainWindow;
                m_videoCB.MainWindow = m_mainWindow;

                if (!ConfigureTranscoder())
                    return false;

                hr = ms.mediaControl.Pause();
                DsError.ThrowExceptionForHR(hr);

                System.Threading.Thread.Sleep(300);

                hr = ms.mediaControl.Run();
                DsError.ThrowExceptionForHR(hr);

                ResetStats();

                recStartTime = DateTime.Now;
                fpsStartTime = recStartTime;
                fpsNotDropped = 0;
                statsTimer.Start();
            }
            catch (COMException ex)
            {
                MessageBox.Show(ex.ToString());
                return false;
            }

            return true;
        }


        private static void SetRealTimeVideoMode(MediaSocket socket)
        {
            foreach(MediaPin pin in socket.Pins)
            {
                if(pin.StreamInfo.MediaType == PrimoSoftware.AVBlocks.MediaType.Video)
                {
                    pin.Params[Param.Video.FrameRateConverter.Use] = Use.On;
                    pin.Params[Param.Video.FrameRateConverter.RealTime] = true;

                    if(pin.StreamInfo.StreamType == StreamType.H264)
                    {
                        pin.Params[Param.Encoder.Video.H264.FixedFramerate] = false;
                    }
                    pin.Params[Param.HardwareEncoder] = HardwareEncoder.Auto;
                }
            }
        }

        private bool ConfigureTranscoder()
        {
            ms.transcoder = new CompositeTranscoder();
            ms.transcoder.AllowDemoMode = true;

            // set audio input pin if audio is not disabled
            if (ms.audioInput != null)
            {
                if ((ms.audioType.majorType != DirectShowLib.MediaType.Audio) ||
                    (ms.audioType.formatType != DirectShowLib.FormatType.WaveEx))
                {
                    return false;
                }

                WaveFormatEx wfx = new WaveFormatEx();
                Marshal.PtrToStructure(ms.audioType.formatPtr, wfx);

                if (wfx.wFormatTag != 1) // WAVE_FORMAT_PCM
                    return false;

                AudioStreamInfo audioInfo = new AudioStreamInfo();
                audioInfo.BitsPerSample = wfx.wBitsPerSample;
                audioInfo.Channels = wfx.nChannels;
                audioInfo.SampleRate = wfx.nSamplesPerSec;
                audioInfo.StreamType = StreamType.LPCM;

                MediaSocket inputSocket = new MediaSocket();
                MediaPin inputPin = new MediaPin();
                inputPin.StreamInfo = audioInfo;
                inputSocket.Pins.Add(inputPin);
                inputSocket.StreamType = StreamType.LPCM;

                m_audioCB.StreamNumber = ms.transcoder.Inputs.Count;
                ms.transcoder.Inputs.Add(inputSocket);
            }

            // set video input pin
            {
                if ((ms.videoType.majorType != DirectShowLib.MediaType.Video) ||
                    (ms.videoType.formatType != DirectShowLib.FormatType.VideoInfo))
                {
                    return false;
                }

                VideoStreamInfo videoInfo = new VideoStreamInfo();
                VideoInfoHeader vih = (VideoInfoHeader)Marshal.PtrToStructure(ms.videoType.formatPtr, typeof(VideoInfoHeader));

                if (vih.AvgTimePerFrame > 0)
                    videoInfo.FrameRate = (double)10000000 / vih.AvgTimePerFrame;

                videoInfo.Bitrate = 0; //vih.BitRate;
                videoInfo.FrameHeight = Math.Abs(vih.BmiHeader.Height);
                videoInfo.FrameWidth = vih.BmiHeader.Width;
                videoInfo.DisplayRatioWidth = videoInfo.FrameWidth;
                videoInfo.DisplayRatioHeight = videoInfo.FrameHeight;
                videoInfo.ScanType = ScanType.Progressive;
                videoInfo.Duration = 0;

                if (ms.videoType.subType == MediaSubType.MJPG)
                {
                    videoInfo.StreamType = StreamType.Mjpeg;
                    videoInfo.ColorFormat = ColorFormat.YUV422;
                }
                else
                {
                    videoInfo.StreamType = StreamType.UncompressedVideo;
                    videoInfo.ColorFormat = Util.GetColorFormat(ref ms.videoType.subType);
                }

                // unsupported capture format
                if (videoInfo.ColorFormat == ColorFormat.Unknown)
                    return false;

                switch (videoInfo.ColorFormat)
                {
                    case ColorFormat.BGR32:
                    case ColorFormat.BGRA32:
                    case ColorFormat.BGR24:
                    case ColorFormat.BGR444:
                    case ColorFormat.BGR555:
                    case ColorFormat.BGR565:
                        videoInfo.FrameBottomUp = (vih.BmiHeader.Height > 0);
                        break;
                }

                MediaSocket inputSocket = new MediaSocket();
                MediaPin inputPin = new MediaPin();
                inputPin.StreamInfo = videoInfo;
                inputSocket.Pins.Add(inputPin);
                inputSocket.StreamType = StreamType.UncompressedVideo;

                m_videoCB.StreamNumber = ms.transcoder.Inputs.Count;
                ms.transcoder.Inputs.Add(inputSocket);
            }

            PresetDescriptor preset = comboPresets.SelectedItem as PresetDescriptor;
            MediaSocket outputSocket;

            outputSocket = Util.MediaSocketFromPreset(preset.Name);
            outputSocket.File = txtOutput.Text;

            SetRealTimeVideoMode(outputSocket);

            ms.transcoder.Outputs.Add(outputSocket);

            ms.transcoder.AudioLog = txtAudioLog.Text;

            if (!ms.transcoder.Open()) {
                MessageBox.Show(ms.transcoder.Error.Message + "\n" + ms.transcoder.Error.Hint,
                    "Transcoder Open Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return false;
            }

            return true;
        }

        private void StopRecording()
        {
            statsTimer.Stop();

            ClearGraph();

            m_audioCB.Reset();
            m_videoCB.Reset();

            BuildGraph();
        }

        private void cmdRecord_Click(object sender, EventArgs e)
        {
            if (bCmdRecordBusy)
                return;

            bCmdRecordBusy = true;
            cmdRecord.Enabled = false;

            if (bRecording)
            {
                // stop recording
                StopRecording();

                txtRecording.Visible = false;
                cmdRecord.Text = sStartRecording;
                EnableCommandUI(true);
                bRecording = false;
            }
            else
            {
                // start recording
                try
                {
                    EnableCommandUI(false);
                    if (StartRecording())
                    {
                        txtRecording.Visible = true;
                        cmdRecord.Text = sStopRecording;

                        bRecording = true;
                    }
                    else
                    {
                        EnableCommandUI(true);
                    }
                }
                catch (COMException ex)
                {
                    MessageBox.Show(ex.ToString());
                }
            }

            cmdRecord.Enabled = true;
            bCmdRecordBusy = false;
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == Util.WM_STOP_CAPTURE)
            {
                System.Diagnostics.Trace.WriteLine("WM_STOP_CAPTURE WParam:" + m.WParam.ToInt32().ToString());

                cmdRecord_Click(null, null);

                int stopReason = m.WParam.ToInt32();

                if (stopReason >= 0)
                {
                    MessageBox.Show(this,
                        "An error occurred encoding captured data. The recording has been stopped.", "AVBlocks",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    MessageBox.Show(this,
                        "An error occurred while recording. The recording has been stopped.",
                        "Unexpected Error",
                        MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                base.WndProc(ref m);
            }
        }

        private void RecorderForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (bRecording)
            {
                statsTimer.Stop();

                ms.Reset(true);

                m_audioCB.Reset();
                m_videoCB.Reset();

                Thread.Sleep(300);
            }
            else
            {
                ms.Reset(true);
            }
        }

        class DevItem
        {
            public DevItem() { }
            public DevItem(string friendlyName, string displayName)
            {
                FriendlyName = friendlyName;
                DisplayName = displayName;
            }

            public string FriendlyName;
            public string DisplayName; // null designates disabled input

            public override string ToString()
            {
                return FriendlyName;
            }

            public bool Disabled
            {
                get { return DisplayName == null; }
            }
        };

        private void EnableCommandUI(bool enable)
        {
            listAudioDev.Enabled = enable;
            cmdAudioDevProp.Enabled = enable;
            listVideoDev.Enabled = enable;
            cmdVideoDevProp.Enabled = enable;
            cmdVideoCaptureProp.Enabled = enable;
            comboPresets.Enabled = enable;
            btnChooseOutput.Enabled = enable;
        }

        private void btnChooseOutput_Click(object sender, EventArgs e)
        {
            PresetDescriptor preset = comboPresets.SelectedItem as PresetDescriptor;

            SaveFileDialog dlg = new SaveFileDialog();

            string filter = string.Empty;

            if (!string.IsNullOrEmpty(preset.FileExtension))
            {
                dlg.DefaultExt = preset.FileExtension;
                filter = string.Format("(*.{0})|*.{0}|", preset.FileExtension);
            }

            filter += "All files (*.*)|*.*";
            dlg.Filter = filter;
            dlg.OverwritePrompt = true;

            if (txtOutput.Text.Length > 0)
            {
                dlg.FileName = System.IO.Path.GetFileName(txtOutput.Text);
                dlg.InitialDirectory = System.IO.Path.GetDirectoryName(txtOutput.Text);
            }

            if (DialogResult.OK != dlg.ShowDialog())
                return;

            txtOutput.Text = dlg.FileName;
            txtAudioLog.Text = Util.AudioLogForVideoFile(dlg.FileName, preset);
        }

        private void comboPresets_SelectedIndexChanged(object sender, EventArgs e)
        {
            PresetDescriptor preset = comboPresets.SelectedItem as PresetDescriptor;

            if (string.IsNullOrEmpty(preset.FileExtension) || txtOutput.Text.Length == 0)
                return;

            string newExt = "." + preset.FileExtension;
            string oldExt = System.IO.Path.GetExtension(txtOutput.Text);

            if (oldExt != newExt) {
                string newFile = System.IO.Path.ChangeExtension(txtOutput.Text, newExt);
                txtOutput.Text = newFile;
            }

            txtAudioLog.Text = Util.AudioLogForVideoFile(txtOutput.Text, preset);
        }

    }
}
