/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using DirectShowLib;
using System.Diagnostics;
using PrimoSoftware.AVBlocks;

namespace InputDS
{
    static class Util
    {
        struct ColorSpaceEntry
        {
            public Guid videoSubType;
            public ColorFormat colorFormat;

            public ColorSpaceEntry(Guid videoSubType, ColorFormat colorFormat)
            {
                this.videoSubType = videoSubType;
                this.colorFormat = colorFormat;
            }
        };

        private static ColorSpaceEntry[] ColorSpaceTab =
        {
            new ColorSpaceEntry(MediaSubType.RGB24,	ColorFormat.BGR24),
            new ColorSpaceEntry(MediaSubType.ARGB32,ColorFormat.BGRA32), // with alpha
            new ColorSpaceEntry(MediaSubType.RGB32,	ColorFormat.BGR32),
            new ColorSpaceEntry(MediaSubType.RGB565,ColorFormat.BGR565),
            new ColorSpaceEntry(MediaSubType.ARGB1555,ColorFormat.BGR555),// with alpha
            new ColorSpaceEntry(MediaSubType.RGB555,ColorFormat.BGR555),// with alpha
            new ColorSpaceEntry(MediaSubType.ARGB4444,ColorFormat.BGR444),// with alpha

            // tested mappings
            new ColorSpaceEntry(MediaSubType.YV12, ColorFormat.YV12),
            new ColorSpaceEntry(MediaSubType.I420, ColorFormat.YUV420),
            new ColorSpaceEntry(MediaSubType.IYUV, ColorFormat.YUV420),
            new ColorSpaceEntry(MediaSubType.YUY2, ColorFormat.YUY2),

            // possible mappings
            new ColorSpaceEntry(MediaSubType.NV12, ColorFormat.NV12),
            new ColorSpaceEntry(MediaSubType.UYVY, ColorFormat.UYVY),
            new ColorSpaceEntry(MediaSubType.Y411, ColorFormat.Y411),
            new ColorSpaceEntry(MediaSubType.Y41P, ColorFormat.Y41P),
            new ColorSpaceEntry(MediaSubType.YVU9, ColorFormat.YVU9),
        };
        
        public static void ReleaseComObject<T>(ref T comObject)
        {
            if (comObject != null)
            {
                Marshal.ReleaseComObject(comObject);

                comObject = default(T);
            }
        }

        public static void DisposeObject<T>(ref T obj) where T:IDisposable
        {
            if (obj != null)
            {
                obj.Dispose();
                obj = default(T);
            }
        }

        public static int GetPin(IBaseFilter filter, PinDirection pinDir, string name, out IPin ppPin)
        {
            ppPin = null;

            IEnumPins enumPins = null;
            IPin[] pins = new IPin[1] { null };

            try
            {
                int hr = filter.EnumPins(out enumPins);
                DsError.ThrowExceptionForHR(hr);

                while (enumPins.Next(1, pins, IntPtr.Zero) == 0)
                {
                    PinInfo pi;
                    hr = pins[0].QueryPinInfo(out pi);

                    bool found = false;
                    if (hr == 0 && pi.dir == pinDir && pi.name == name)
                    {
                        found = true;
                        
                        ppPin = pins[0];
                        
                        DsUtils.FreePinInfo(pi);
                    }

                    if (found)
                        return 0;
                   
                    Util.ReleaseComObject(ref pins[0]);
                }

                // Did not find a matching pin.
            }
            catch (COMException)
            {
            }
            finally
            {
                Marshal.ReleaseComObject(enumPins);
            }

            return WinAPI.E_FAIL;
        }

        public static int GetUnconnectedPin(IBaseFilter filter, PinDirection pinDir, out IPin ppPin)
        {
            ppPin = null;

            IEnumPins enumPins = null;
            IPin[] pins = new IPin[1] { null };
            IPin tmpPin = null;

            try
            {
                int hr = filter.EnumPins(out enumPins);
                DsError.ThrowExceptionForHR(hr);

                while (enumPins.Next(1, pins, IntPtr.Zero) == 0)
                {
                    PinDirection thisPinDir;
                    hr = pins[0].QueryDirection(out thisPinDir);

                    if (hr == 0 && thisPinDir == pinDir)
                    {
                        hr = pins[0].ConnectedTo(out tmpPin);
                        if (tmpPin != null)  // Already connected, not the pin we want.
                        {
                            Util.ReleaseComObject(ref tmpPin);
                        }
                        else  // Unconnected, this is the pin we want.
                        {
                            ppPin = pins[0];
                            return 0;
                        }
                    }

                    Util.ReleaseComObject(ref pins[0]);
                }

                // Did not find a matching pin.
            }
            catch (COMException)
            {
            }
            finally
            {
                Marshal.ReleaseComObject(enumPins);
            }

            return WinAPI.E_FAIL;
        }


        public static int ConnectFilters(IGraphBuilder graph, IBaseFilter src, IBaseFilter dest)
        {
            if ((graph == null) || (src == null) || (dest == null))
            {
                return WinAPI.E_FAIL;
            }

            // Find an output pin on the upstream filter.
            IPin pinOut = null;
            IPin pinIn = null;

            try
            {
                int hr = GetUnconnectedPin(src, PinDirection.Output, out pinOut);
                DsError.ThrowExceptionForHR(hr);

                // Find an input pin on the downstream filter.

                hr = GetUnconnectedPin(dest, PinDirection.Input, out pinIn);
                DsError.ThrowExceptionForHR(hr);

                // Try to connect them.
                hr = graph.ConnectDirect(pinOut, pinIn, null);
                DsError.ThrowExceptionForHR(hr);

                return 0;
            }
            catch (COMException)
            {
            }
            finally
            {
                Util.ReleaseComObject(ref pinIn);
                Util.ReleaseComObject(ref pinOut);
            }

            return WinAPI.E_FAIL;
        }

        public static int ConnectFilters(IGraphBuilder graph, IPin pinOut, IBaseFilter dest)
        {
            if ((graph == null) || (pinOut == null) || (dest == null))
            {
                return WinAPI.E_FAIL;
            }

            IPin pinIn = null;

            try
            {
                // Find an input pin on the downstream filter.
                int hr = GetUnconnectedPin(dest, PinDirection.Input, out pinIn);
                DsError.ThrowExceptionForHR(hr);

                // Try to connect them.
                hr = graph.Connect(pinOut, pinIn);
                DsError.ThrowExceptionForHR(hr);

                return 0;
            }
            catch (COMException)
            {
            }
            finally
            {
                Util.ReleaseComObject(ref pinIn);
            }

            return WinAPI.E_FAIL;
        }


        // Tear down everything downstream of a given filter
        public static int NukeDownstream(IFilterGraph graph, IBaseFilter filter)
        {
            if (filter == null)
                return WinAPI.E_FAIL;

            IEnumPins enumPins = null;
            IPin[] pins = new IPin[1] { null };

            try
            {
                int hr = filter.EnumPins(out enumPins);
                DsError.ThrowExceptionForHR(hr);
                enumPins.Reset(); // start at the first pin

                while (enumPins.Next(1, pins, IntPtr.Zero) == 0)
                {
                    if (pins[0] != null)
                    {
                        PinDirection pindir;
                        pins[0].QueryDirection(out pindir);
                        if (pindir == PinDirection.Output)
                        {
                            IPin pTo = null;
                            pins[0].ConnectedTo(out pTo);
                            if (pTo != null)
                            {
                                PinInfo pi;
                                hr = pTo.QueryPinInfo(out pi);

                                if (hr == 0)
                                {
                                    NukeDownstream(graph, pi.filter);

                                    graph.Disconnect(pTo);
                                    graph.Disconnect(pins[0]);
                                    graph.RemoveFilter(pi.filter);

                                    Util.ReleaseComObject(ref pi.filter);
                                    DsUtils.FreePinInfo(pi);
                                }
                                Marshal.ReleaseComObject(pTo);
                            }
                        }
                        Marshal.ReleaseComObject(pins[0]);
                    }
                }

                return 0;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.ToString());
            }
            finally
            {
                Marshal.ReleaseComObject(enumPins);
            }

            return WinAPI.E_FAIL;

        }


        public static ColorFormat GetColorFormat(ref Guid videoSubtype)
        {
            for (int i = 0; i < ColorSpaceTab.Length; i++)
            {
                if (ColorSpaceTab[i].videoSubType == videoSubtype)
                    return ColorSpaceTab[i].colorFormat;
            }

            return ColorFormat.Unknown;
        }

        public static IBaseFilter CreateCaptureDevice(Guid filterCategory, string captureDeviceName)
        {
            // FilterCategory.VideoInputDevice, FilterCategory.AudioInputDevice

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
                    return null;

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
                        try
                        {
                            moniker[0].BindToObject(null, null, ref baseFilterId, out filter);
                            if (filter != null)
                            {
                                if ((captureDeviceName == null) || (captureDeviceName == friendlyName))
                                {
                                    return filter as IBaseFilter;
                                }

                                Util.ReleaseComObject(ref filter);
                            }
                        }
                        catch
                        {
                            System.Diagnostics.Trace.WriteLine("Cannot use input device " + friendlyName);
                        }
                    } // if IPropertyBag

                    Util.ReleaseComObject(ref moniker[0]);
                } // while enum devices
            }
            catch (COMException ex)
            {
                System.Diagnostics.Debug.WriteLine(ex.ToString());
            }
            finally
            {
                Util.ReleaseComObject(ref propBag);
                Util.ReleaseComObject(ref moniker[0]);
                Util.ReleaseComObject(ref enumCat);
                Util.ReleaseComObject(ref devEnum);
            }

            return null;
        } // EnumInputDev

        public static IBaseFilter CreateFilter(Guid clsid)
        {
            Type comType = Type.GetTypeFromCLSID(clsid);

            if (comType == null)
                return null;

            return (IBaseFilter)Activator.CreateInstance(comType);
        }

        static string GetErrorText(int hr)
        {
            return String.Format("Error: 0x{0:x8} ({1})", hr, DsError.GetErrorText(hr));
        }

        public static string DumpAMMediaTypeInfo(AMMediaType mt)
        {
            StringBuilder sb = new StringBuilder();

            sb.AppendLine(">>>>> AMMediaType Info");

            sb.AppendLine(DsToString.AMMediaTypeToString(mt));

            sb.AppendLine(string.Format("majorType: {0}  subType:{1}   formatType:{2}", mt.majorType, mt.subType, mt.formatType));


            if((mt.majorType == DirectShowLib.MediaType.Video) && 
                (mt.formatType == DirectShowLib.FormatType.VideoInfo))
            {
                VideoInfoHeader vih = (VideoInfoHeader)Marshal.PtrToStructure(mt.formatPtr, typeof(VideoInfoHeader));

                sb.AppendLine("VideoInfoHeader fields");
                sb.AppendLine("AvgTimePerFrame: " + vih.AvgTimePerFrame.ToString());
                sb.AppendLine("BitErrorRate: " + vih.BitErrorRate.ToString());
                sb.AppendLine("BitRate: " + vih.BitRate.ToString());

                BitmapInfoHeader bmi = vih.BmiHeader;
                sb.AppendLine("BitmapInfoHeader fields");
                sb.AppendLine("BitCount: " + bmi.BitCount.ToString());
                sb.AppendLine("ClrImportant: " + bmi.ClrImportant.ToString());
                sb.AppendLine("ClrUsed: " + bmi.ClrUsed.ToString());
                sb.AppendLine("Compression: " + bmi.Compression.ToString());
                sb.AppendLine("Height: " + bmi.Height.ToString());
                sb.AppendLine("Width: " + bmi.Width.ToString());
                sb.AppendLine("ImageSize: " + bmi.ImageSize.ToString());
                sb.AppendLine("Planes: " + bmi.Planes.ToString());
                sb.AppendLine("Size: " + bmi.Size.ToString());
                sb.AppendLine("XPelsPerMeter: " + bmi.XPelsPerMeter.ToString());
                sb.AppendLine("YPelsPerMeter: " + bmi.YPelsPerMeter.ToString());
            }


            if ((mt.majorType == DirectShowLib.MediaType.Audio) ||
                (mt.formatType == DirectShowLib.FormatType.WaveEx))
            {
                WaveFormatEx wfx = (WaveFormatEx)Marshal.PtrToStructure(mt.formatPtr, typeof(WaveFormatEx));

                sb.AppendLine("WaveFormatEx fields");
                sb.AppendLine("wFormatTag: " + wfx.wFormatTag.ToString());
                sb.AppendLine("cbSize: " + wfx.cbSize.ToString());
                sb.AppendLine("nAvgBytesPerSec: " + wfx.nAvgBytesPerSec.ToString());
                sb.AppendLine("nBlockAlign: " + wfx.nBlockAlign.ToString());
                sb.AppendLine("nChannels: " + wfx.nChannels.ToString());
                sb.AppendLine("nSamplesPerSec: " + wfx.nSamplesPerSec.ToString());
                sb.AppendLine("wBitsPerSample: " + wfx.wBitsPerSample.ToString());
                sb.AppendLine("cbSize: " + wfx.cbSize.ToString());
            }

            sb.AppendLine("<<<<< AMMediaType Info");

            return sb.ToString();
        }

        public static string DumpPinInfo(IPin pin)
        {
            StringBuilder sb = new StringBuilder();
            int hr;

            sb.AppendLine(">>>>> Pin Info");

            {
                PinInfo pi;
                hr = pin.QueryPinInfo(out pi);
                if (0 == hr)
                {
                    sb.AppendLine(string.Format("PinInfo  name: {0}  direction: {1}", pi.name, pi.dir.ToString()));
                }
                else
                {
                    sb.AppendLine("PinInfo: " + GetErrorText(hr));
                }
            }

            // Pins info
            {
                IEnumMediaTypes enumMediaTypes = null;
                AMMediaType[] mediaTypes = new AMMediaType[1] { null };

                try
                {
                    hr = pin.EnumMediaTypes(out enumMediaTypes);
                    DsError.ThrowExceptionForHR(hr);

                    while (enumMediaTypes.Next(1, mediaTypes, IntPtr.Zero) == 0)
                    {
                        sb.AppendLine(DumpAMMediaTypeInfo(mediaTypes[0]));
                        DsUtils.FreeAMMediaType(mediaTypes[0]);
                        mediaTypes[0] = null;
                    }
                }
                finally
                {
                    Marshal.ReleaseComObject(enumMediaTypes);
                }
            }

            sb.AppendLine("<<<<< Pin Info");

            return sb.ToString();
        }

        public static string DumpFilterInfo(IBaseFilter filter)
        {
            StringBuilder sb = new StringBuilder();

            int hr;

            sb.AppendLine(">>>>>>> BaseFilter info dump BEGIN");

            {
                Guid guid;
                hr = filter.GetClassID(out guid);
                if (0 == hr)
                {
                    sb.AppendLine("ClassID: " + guid.ToString());
                }
                else
                {
                    sb.AppendLine("ClassID: " + GetErrorText(hr));
                }
            }

            {
                string vendorInfo = null;
                hr = filter.QueryVendorInfo(out vendorInfo);
                if (0 == hr)
                {
                    sb.AppendLine(string.Format("VendorInfo: {0}", vendorInfo));
                }
                else
                {
                    sb.AppendLine("VendorInfo: " + GetErrorText(hr));
                }
            }

            {
                FilterInfo fi;
                hr = filter.QueryFilterInfo(out fi);
                if (0 == hr)
                {
                    sb.AppendLine(string.Format("FilterInfo achName: {0}", fi.achName));
                }
                else
                {
                    sb.AppendLine("FilterInfo achName: " + GetErrorText(hr));
                }
            }

            IFileSourceFilter fileSourceFilter = filter as IFileSourceFilter;
            if (fileSourceFilter != null)
            {
                string fileName;
                AMMediaType mt = new AMMediaType();
                fileSourceFilter.GetCurFile(out fileName, mt);
                DsUtils.FreeAMMediaType(mt);
                sb.AppendLine("IFileSourceFilter CurFile: " + fileName);
            }

            // Pins info
            {
                IEnumPins enumPins = null;
                IPin[] pins = new IPin[1] { null };

                try
                {
                    hr = filter.EnumPins(out enumPins);
                    DsError.ThrowExceptionForHR(hr);

                    while (enumPins.Next(1, pins, IntPtr.Zero) == 0)
                    {
                        sb.AppendLine(DumpPinInfo(pins[0]));
                        Util.ReleaseComObject(ref pins[0]);
                    }
                }
                finally
                {
                    Marshal.ReleaseComObject(enumPins);
                }
            }

            sb.AppendLine(">>>>>>> BaseFilter info dump END");

            return sb.ToString();
        }
    }
}
