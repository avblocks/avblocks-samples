using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using DirectShowLib;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;

namespace CaptureDS
{
    public partial class VideoCapturePropertiesForm : Form
    {
        public VideoCapturePropertiesForm()
        {
            InitializeComponent();
        }

        void SetFormat(int formatIndex, int frameRate)
        {
            int capsCount, capSize;
            int hr = VideoConfig.GetNumberOfCapabilities(out capsCount, out capSize);
            DsError.ThrowExceptionForHR(hr);

            IntPtr pSC = Marshal.AllocHGlobal(capSize);
            AMMediaType mt = null;

            try
            {
                VideoInfoHeader vih = new VideoInfoHeader();

                hr = VideoConfig.GetStreamCaps(formatIndex, out mt, pSC);
                DsError.ThrowExceptionForHR(hr);

                if(frameRate > 0)
                {
                    Marshal.PtrToStructure(mt.formatPtr, vih);
                    vih.AvgTimePerFrame = (long)(10000000.0 / frameRate);
                    Marshal.StructureToPtr(vih, mt.formatPtr, false);
                }

                hr = VideoConfig.SetFormat(mt);
                DsError.ThrowExceptionForHR(hr);
            }
            finally
            {
                DsUtils.FreeAMMediaType(mt);
                Marshal.FreeHGlobal(pSC);
            }
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            ComboboxItem itemFormat = comboBoxFormats.SelectedItem as ComboboxItem;
            if (itemFormat != null)
            {
                int frameRate = -1;
                ComboboxItem itemFPS = comboBoxFrameRate.SelectedItem as ComboboxItem;
                if (null != itemFPS)
                {
                    frameRate = itemFPS.Value;
                }

                try
                {
                    SetFormat(itemFormat.Value, frameRate);
                }
                catch (Exception)
                {
                    MessageBox.Show("Failed to set the selected video format.");
                }
            }

            DialogResult = DialogResult.OK;
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

        public IAMStreamConfig VideoConfig { get; set; }

        struct FormatEntry
        {
            public Guid videoSubType;
            public string Name;

            public FormatEntry(Guid videoSubType, string name)
            {
                this.videoSubType = videoSubType;
                this.Name = name;
            }
        };

        private static FormatEntry[] FormatsTab =
        {
            new FormatEntry(MediaSubType.MJPG,	"MJPG"),

            new FormatEntry(MediaSubType.RGB24,	"RGB24"),
            new FormatEntry(MediaSubType.ARGB32, "ARGB32"),
            new FormatEntry(MediaSubType.RGB32,	"RGB32"),
            new FormatEntry(MediaSubType.RGB565, "RGB565"),
            new FormatEntry(MediaSubType.ARGB1555, "ARGB1555"),
            new FormatEntry(MediaSubType.RGB555, "RGB555"),
            new FormatEntry(MediaSubType.ARGB4444, "ARGB4444"),

            new FormatEntry(MediaSubType.YV12, "YV12"),
            new FormatEntry(MediaSubType.I420, "I420"),
            new FormatEntry(MediaSubType.IYUV, "IYUV"),
            new FormatEntry(MediaSubType.YUY2, "YUY2"),

            new FormatEntry(MediaSubType.NV12, "NV12"),
            new FormatEntry(MediaSubType.UYVY, "UYVY"),
            new FormatEntry(MediaSubType.Y411, "Y411"),
            new FormatEntry(MediaSubType.Y41P, "Y41P"),
            new FormatEntry(MediaSubType.YVU9, "YVU9"),
        };

        string GetSubtypeString(Guid videoSubtype)
        {
            for (int i = 0; i < FormatsTab.Length; i++)
            {
                if (FormatsTab[i].videoSubType == videoSubtype)
                    return FormatsTab[i].Name;
            }

            return null;
        }

        public class ComboboxItem
        {
            public string Text { get; set; }
            public int Value { get; set; }

            public override string ToString()
            {
                return Text;
            }
        }

        private void VideoCapturePropertiesForm_Load(object sender, EventArgs e)
        {
            try
            {
                InitFormatsList();
            }
            catch (Exception)
            {
                MessageBox.Show("Failed to set init the formats list.");
            }
        }

        private void InitFormatsList()
        {
            int hr = 0;

            if (null == VideoConfig)
                throw new Exception("VideoConfig not set");

            // enumerate the capabilities of the video capture device
            int capsCount, capSize;
            hr = VideoConfig.GetNumberOfCapabilities(out capsCount, out capSize);
            DsError.ThrowExceptionForHR(hr);

            VideoInfoHeader vih = new VideoInfoHeader();
            VideoStreamConfigCaps vsc = new VideoStreamConfigCaps();
            IntPtr pSC = Marshal.AllocHGlobal(capSize);

            try
            {
                int videoFormatIndex = -1;
                int minFps = -1;
                int maxFps = -1;

                int currentWidth = 0;
                int currentHeight = 0;
                Guid currentSubType;
                int currentFps = 0;

                {
                    AMMediaType mt = null;
                    hr = VideoConfig.GetFormat(out mt);
                    Marshal.PtrToStructure(mt.formatPtr, vih);
                    DsError.ThrowExceptionForHR(hr);

                    currentFps = (int)(10000000.0 / vih.AvgTimePerFrame);
                    currentWidth = vih.BmiHeader.Width;
                    currentHeight = vih.BmiHeader.Height;
                    currentSubType = mt.subType;

                    DsUtils.FreeAMMediaType(mt);
                }

                for (int i = 0; i < capsCount; ++i)
                {
                    AMMediaType mt = null;

                    // the video format is described in AMMediaType and VideoStreamConfigCaps
                    hr = VideoConfig.GetStreamCaps(i, out mt, pSC);
                    DsError.ThrowExceptionForHR(hr);

                    if (mt.formatType == DirectShowLib.FormatType.VideoInfo)
                    {
                        string formatName = GetSubtypeString(mt.subType);

                        if (!string.IsNullOrEmpty(formatName))
                        {
                            // copy the unmanaged structures to managed in order to check the format
                            Marshal.PtrToStructure(mt.formatPtr, vih);
                            Marshal.PtrToStructure(pSC, vsc);

                            int fps = (int)(10000000.0 / vsc.MaxFrameInterval);
                            if ((minFps < 0) || (minFps > fps))
                                minFps = fps;

                            fps = (int)(10000000.0 / vsc.MinFrameInterval);
                            if ((maxFps < 0) || (maxFps < fps))
                                maxFps = fps;

                            string capline = String.Format("{0} x {1}, min fps {2:0.}, max fps {3:0.}, {4}",
                                    vih.BmiHeader.Width, vih.BmiHeader.Height, 10000000.0 / vsc.MaxFrameInterval, 10000000.0 / vsc.MinFrameInterval, formatName);

                            if ((vih.BmiHeader.Width == currentWidth) &&
                                (vih.BmiHeader.Height == currentHeight) &&
                                (mt.subType == currentSubType))
                            {
                                videoFormatIndex = comboBoxFormats.Items.Count;
                            }

                            ComboboxItem item = new ComboboxItem();
                            item.Text = capline;
                            item.Value = i;

                            comboBoxFormats.Items.Add(item);
                        }
                    }

                    DsUtils.FreeAMMediaType(mt);
                }

                if (videoFormatIndex >= 0)
                    comboBoxFormats.SelectedIndex = videoFormatIndex;

                if ((minFps >= 0) && (maxFps >= 0))
                {
                    for (int i = minFps; i <= maxFps; i++)
                    {
                        ComboboxItem item = new ComboboxItem();
                        item.Text = i.ToString();
                        item.Value = i;

                        comboBoxFrameRate.Items.Add(item);

                        if (currentFps == i)
                            comboBoxFrameRate.SelectedIndex = comboBoxFrameRate.Items.Count - 1;
                    }
                }
            }
            finally
            {
                Marshal.FreeHGlobal(pSC);
            }
        }
    }
}
