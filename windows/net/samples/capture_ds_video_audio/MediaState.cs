using System;
using System.Collections.Generic;
using System.Text;
using DirectShowLib;
using System.Diagnostics;

namespace CaptureDS
{
    class MediaState
    {
        public MediaState()
        {
        }

        public IMediaControl mediaControl;
        public IFilterGraph2 graph;
        public ICaptureGraphBuilder2 captureGraph;

        public IBaseFilter audioInput;
        public IBaseFilter videoInput;
        public IBaseFilter smartTee;
        public IBaseFilter previewRenderer;

        public IBaseFilter audioSampleGrabber;
        public IBaseFilter videoSampleGrabber;
        public ISampleGrabber audioGrabber;
        public ISampleGrabber videoGrabber;

        public IBaseFilter audioNullRenderer;
        public IBaseFilter videoNullRenderer;

        public DsROTEntry rot;

        public AMMediaType videoType = new AMMediaType();
        public AMMediaType audioType = new AMMediaType();

        public IAMDroppedFrames droppedFrames;
        
        //public PrimoSoftware.AVBlocks.Transcoder transcoder;

        //public CompositeTranscoder t = new CompositeTranscoder();
        public CompositeTranscoder transcoder;

        public void Reset(bool full)
        {
            /*
             * NOTE:
             * Interfaces obtained with a cast (as) should not be released with Marshal.ReleaseComObject.
             */

            if (mediaControl != null)
            {
                mediaControl.StopWhenReady();
                mediaControl = null;
            }

            if (full && rot != null)
            {
                rot.Dispose();
                rot = null;
            }

            DsUtils.FreeAMMediaType(videoType);
            DsUtils.FreeAMMediaType(audioType);

            if (transcoder != null)
            {
                transcoder.Flush();
                transcoder.Close();
                Util.DisposeObject(ref transcoder);
            }

            if (previewRenderer != null)
            {
                IVideoWindow window = graph as IVideoWindow;
                window.put_Owner(IntPtr.Zero);
                window.put_Visible(OABool.False);
            }
            
            if (videoInput != null)
            {
                Util.NukeDownstream(graph, videoInput);
            }

            if (audioInput != null)
            {
                Util.NukeDownstream(graph, audioInput);
            }
            
            audioNullRenderer = null;
            videoNullRenderer = null;
            audioGrabber = null;
            videoGrabber = null;
            audioSampleGrabber = null;
            videoSampleGrabber = null;
            droppedFrames = null;
            previewRenderer = null;
            smartTee = null;

            if (full)
            {
                Util.ReleaseComObject(ref videoInput);
                Util.ReleaseComObject(ref audioInput);
                Util.ReleaseComObject(ref graph);
                Util.ReleaseComObject(ref captureGraph);
            }
        }
    }
}
