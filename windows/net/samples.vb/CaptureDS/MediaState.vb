Imports System
Imports System.Collections.Generic
Imports System.Text
Imports DirectShowLib
Imports System.Diagnostics

Namespace CaptureDS
	Friend Class MediaState
		Public Sub New()
		End Sub

		Public mediaControl As IMediaControl
		Public graph As IFilterGraph2
		Public captureGraph As ICaptureGraphBuilder2

		Public audioInput As IBaseFilter
		Public videoInput As IBaseFilter
		Public smartTee As IBaseFilter
		Public previewRenderer As IBaseFilter

		Public audioSampleGrabber As IBaseFilter
		Public videoSampleGrabber As IBaseFilter
		Public audioGrabber As ISampleGrabber
		Public videoGrabber As ISampleGrabber

		Public audioNullRenderer As IBaseFilter
		Public videoNullRenderer As IBaseFilter

		Public rot As DsROTEntry

		Public videoType As New AMMediaType()
		Public audioType As New AMMediaType()

		Public droppedFrames As IAMDroppedFrames
		Public transcoder As PrimoSoftware.AVBlocks.Transcoder

		Public Sub Reset(ByVal full As Boolean)
'            
'             * NOTE:
'             * Interfaces obtained with a cast (as) should not be released with Marshal.ReleaseComObject.
'             

			If mediaControl IsNot Nothing Then
				mediaControl.StopWhenReady()
				mediaControl = Nothing
			End If

			If full AndAlso rot IsNot Nothing Then
				rot.Dispose()
				rot = Nothing
			End If

			DsUtils.FreeAMMediaType(videoType)
			DsUtils.FreeAMMediaType(audioType)

			Util.DisposeObject(transcoder)

			If previewRenderer IsNot Nothing Then
				Dim window As IVideoWindow = TryCast(graph, IVideoWindow)
				window.put_Owner(IntPtr.Zero)
				window.put_Visible(OABool.False)
			End If

			If videoInput IsNot Nothing Then
				Util.NukeDownstream(graph, videoInput)
			End If

			If audioInput IsNot Nothing Then
				Util.NukeDownstream(graph, audioInput)
			End If

			audioNullRenderer = Nothing
			videoNullRenderer = Nothing
			audioGrabber = Nothing
			videoGrabber = Nothing
			audioSampleGrabber = Nothing
			videoSampleGrabber = Nothing
			droppedFrames = Nothing
			previewRenderer = Nothing
			smartTee = Nothing

			If full Then
				Util.ReleaseComObject(videoInput)
				Util.ReleaseComObject(audioInput)
				Util.ReleaseComObject(graph)
				Util.ReleaseComObject(captureGraph)
			End If
		End Sub
	End Class
End Namespace
