Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Text
Imports System.Windows.Forms
Imports System.Runtime.InteropServices
Imports System.Runtime.InteropServices.ComTypes
Imports System.IO
Imports System.Diagnostics
Imports System.Threading

Imports DirectShowLib
Imports PrimoSoftware.AVBlocks

Namespace CaptureDS
	Partial Public Class CaptureDSForm
		Inherits Form

		Private ms As New MediaState()

		Private bIgnoreDeviceSelection As Boolean
		Private bRecording As Boolean
		Private bCmdRecordBusy As Boolean
		Private Const sStartRecording As String = "Start Recording"
		Private Const sStopRecording As String = "Stop Recording"

		Private statsTimer As System.Windows.Forms.Timer
		Private recStartTime As Date
		Private fpsStartTime As Date ' current fps
		Private fpsNotDropped As Integer ' current fps

		Private m_videoCB As New SampleGrabberCB("VideoGrabberCB")
		Private m_audioCB As New SampleGrabberCB("AudioGrabberCB")
		Private m_selDrives As New List(Of Char)()
		Private m_mainWindow As IntPtr = IntPtr.Zero

		Public Sub New()
			InitializeComponent()
#If WIN64 Then
			Me.Text &= " (64-bit)"
#End If

			EnumInputDev(FilterCategory.AudioInputDevice, listAudioDev)
			listAudioDev.Items.Insert(0, New DevItem() With {.FriendlyName = "[ No audio input ]"})
				'  DisplayName = null

			EnumInputDev(FilterCategory.VideoInputDevice, listVideoDev)

			statsTimer = New System.Windows.Forms.Timer()
			AddHandler statsTimer.Tick, AddressOf UpdateStats
			statsTimer.Interval = 500

			bIgnoreDeviceSelection = False
			bRecording = False
			cmdRecord.Text = sStartRecording
			txtRecording.Visible = False

			ResetStats()

			Dim h As Integer = previewBox.Size.Height
			Dim w As Integer = previewBox.Size.Width
			previewBox.Size = New Size(w, w * 3 \ 4)

			For i As Integer = 0 To AvbTranscoder.Presets.Length - 1
				Dim preset As PresetDescriptor = AvbTranscoder.Presets(i)

				If Not preset.AudioOnly Then
					comboPresets.Items.Add(preset)
				End If
			Next i

			comboPresets.SelectedIndex = 0
		End Sub

		Private Sub UpdateStats(ByVal sender As Object, ByVal e As EventArgs)
			Dim now As Date = Date.Now
			Dim rec As TimeSpan = now.Subtract(recStartTime)
			txtRecTime.Text = String.Format("{0}:{1:d2}:{2:d2}", rec.Hours, rec.Minutes, rec.Seconds)

			If ms.droppedFrames IsNot Nothing Then
				Dim hr As Integer = 0
				Dim dropped As Integer = Nothing
				hr = ms.droppedFrames.GetNumDropped(dropped)
				If 0 = hr Then
					txtNumDropped.Text = dropped.ToString()
				End If

				Dim notDropped As Integer = Nothing
				hr = ms.droppedFrames.GetNumNotDropped(notDropped)

				If 0 = hr Then
					txtNumNotDropped.Text = notDropped.ToString()
					If notDropped >= 0 Then
						Dim averageFPS As Double = CDbl(notDropped) / rec.TotalSeconds
						txtAverageFPS.Text = averageFPS.ToString("F3")

						Dim tsfps As TimeSpan = now.Subtract(fpsStartTime)
						Dim fpsElapsed As Double = tsfps.TotalSeconds
						If fpsElapsed > 5.0 Then
							Dim curFPS As Double = CDbl(notDropped - fpsNotDropped) / fpsElapsed
							txtCurrentFPS.Text = curFPS.ToString("F3")

							fpsStartTime = now
							fpsNotDropped = notDropped
						End If
					End If
				End If
			End If

			txtACallbacks.Text = m_audioCB.SampleIndex.ToString()
			txtAProcessed.Text = m_audioCB.ProcessedSamples.ToString()
			txtADropped.Text = m_audioCB.DroppedSamples.ToString()

			txtVCallbacks.Text = m_videoCB.SampleIndex.ToString()
			txtVProcessed.Text = m_videoCB.ProcessedSamples.ToString()
			txtVDropped.Text = m_videoCB.DroppedSamples.ToString()
		End Sub

		Private Sub ResetStats()
			Dim sEmpty As String = "--"

			txtRecTime.Text = "--:--:--"

			txtNumDropped.Text = sEmpty
			txtNumNotDropped.Text = sEmpty
			txtAverageFPS.Text = sEmpty
			txtCurrentFPS.Text = sEmpty

			txtACallbacks.Text = sEmpty
			txtADropped.Text = sEmpty
			txtAProcessed.Text = sEmpty

			txtVCallbacks.Text = sEmpty
			txtVDropped.Text = sEmpty
			txtVProcessed.Text = sEmpty
		End Sub

		Private Sub EnumInputDev(ByVal filterCategory As Guid, ByVal list As ComboBox)
			If list Is Nothing Then
				Return
			End If

			Dim devEnum As ICreateDevEnum = Nothing
			Dim enumCat As IEnumMoniker = Nothing
			Dim moniker() As IMoniker = { Nothing }
			Dim propBag As IPropertyBag = Nothing

			Dim hr As Integer = 0
			Try

				devEnum = TryCast(New CreateDevEnum(), ICreateDevEnum)
				If devEnum Is Nothing Then
					Throw New COMException("Cannot create CLSID_SystemDeviceEnum")
				End If

				' Obtain enumerator for the capture category.
				hr = devEnum.CreateClassEnumerator(filterCategory, enumCat, 0)
				DsError.ThrowExceptionForHR(hr)

				If enumCat Is Nothing Then
                    If filterCategory = DirectShowLib.FilterCategory.AudioInputDevice Then
                        MessageBox.Show("No audio devices found")
                    ElseIf filterCategory = DirectShowLib.FilterCategory.VideoInputDevice Then
                        MessageBox.Show("No video devices found")
                    End If

					Return
				End If

				' Enumerate the monikers.
				Dim fetchedCount As New IntPtr(0)
				Do While 0 = enumCat.Next(1, moniker, fetchedCount)

					Dim bagId As Guid = GetType(IPropertyBag).GUID
					Dim bagObj As Object = Nothing
					moniker(0).BindToStorage(Nothing, Nothing, bagId, bagObj)

					propBag = TryCast(bagObj, IPropertyBag)

					If propBag IsNot Nothing Then
						Dim val As Object = Nothing
						Dim friendlyName As String = Nothing
						Dim displayName As String = Nothing

						hr = propBag.Read("FriendlyName", val, Nothing)
						If hr = 0 Then
							friendlyName = TryCast(val, String)
						End If
						Util.ReleaseComObject(propBag)

						moniker(0).GetDisplayName(Nothing, Nothing, displayName)

						' create an instance of the filter
						Dim baseFilterId As Guid = GetType(IBaseFilter).GUID
						Dim filter As Object = Nothing
						Dim addFilter As Boolean = False
						Try
							moniker(0).BindToObject(Nothing, Nothing, baseFilterId, filter)
							If filter IsNot Nothing Then
								addFilter = True
								Util.ReleaseComObject(filter)
							End If
						Catch
							System.Diagnostics.Trace.WriteLine("Cannot use input device " & friendlyName)
						End Try

						If addFilter = True AndAlso friendlyName IsNot Nothing AndAlso displayName IsNot Nothing Then
							Dim fi As New DevItem(friendlyName, displayName)
							list.Items.Add(fi)
						End If
					End If ' if IPropertyBag

					Util.ReleaseComObject(moniker(0))
				Loop ' while enum devices
			Catch ex As COMException
				MessageBox.Show(ex.Message)
			Finally
				Util.ReleaseComObject(propBag)
				Util.ReleaseComObject(moniker(0))
				Util.ReleaseComObject(enumCat)
				Util.ReleaseComObject(devEnum)
			End Try

		End Sub ' EnumInputDev

		Private Function InitInputDev(ByVal ms As MediaState, ByVal videoItem As DevItem, ByVal audioItem As DevItem) As Integer
			Dim hr As Integer = 0
			' create Filter Graph Manager
			If ms.graph Is Nothing Then
				'ms.graph = new FilterGraph() as IGraphBuilder;
				ms.graph = TryCast(New FilterGraph(), IFilterGraph2)
				If ms.graph Is Nothing Then
					Throw New COMException("Cannot create FilterGraph")
				End If

				ms.captureGraph = TryCast(New CaptureGraphBuilder2(), ICaptureGraphBuilder2)
				If ms.captureGraph Is Nothing Then
					Throw New COMException("Cannot create CaptureGraphBuilder2")
				End If

				hr = ms.captureGraph.SetFiltergraph(ms.graph)
				DsError.ThrowExceptionForHR(hr)
			End If


			If audioItem IsNot Nothing Then
				' remove the old audio input
				If ms.audioInput IsNot Nothing Then
					hr = ms.graph.RemoveFilter(ms.audioInput)
					Util.ReleaseComObject(ms.audioInput)
					DsError.ThrowExceptionForHR(hr)
				End If

				' create audio input
				If Not audioItem.Disabled Then
					' using BindToMoniker
					ms.audioInput = TryCast(Marshal.BindToMoniker(audioItem.DisplayName), IBaseFilter)

					' add audio input to the graph
					hr = ms.graph.AddFilter(ms.audioInput, audioItem.FriendlyName)
					DsError.ThrowExceptionForHR(hr)
				End If
			End If


			If videoItem IsNot Nothing Then
				' remove the old video input
				If ms.videoInput IsNot Nothing Then
					hr = ms.graph.RemoveFilter(ms.videoInput)
					Util.ReleaseComObject(ms.videoInput)
					DsError.ThrowExceptionForHR(hr)
				End If

				' create video input

				' Using BindToMoniker
				ms.videoInput = TryCast(Marshal.BindToMoniker(videoItem.DisplayName), IBaseFilter)

				' add video input to the graph
				hr = ms.graph.AddFilter(ms.videoInput, videoItem.FriendlyName)
				DsError.ThrowExceptionForHR(hr)
			End If

			Return hr
		End Function

		Private Sub RecorderForm_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
			Try
				bIgnoreDeviceSelection = True

				listAudioDev.SelectedIndex = If(listAudioDev.Items.Count > 1, 1, 0)

				If listVideoDev.Items.Count > 0 Then
					listVideoDev.SelectedIndex = 0
				End If

				bIgnoreDeviceSelection = False

				Dim videoItem As DevItem = DirectCast(listVideoDev.SelectedItem, DevItem)
				Dim audioItem As DevItem = DirectCast(listAudioDev.SelectedItem, DevItem)

				Dim hr As Integer = InitInputDev(ms, videoItem, audioItem)
				DsError.ThrowExceptionForHR(hr)

				If hr <> 0 Then
					Trace.WriteLine("Cannot use the selected capture devices")
				End If

				BuildGraph()

				ms.rot = New DsROTEntry(ms.graph)

				m_mainWindow = Me.Handle
			Catch ex As COMException
				MessageBox.Show(ex.ToString())
			End Try
		End Sub

		Private Sub listAudioDev_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles listAudioDev.SelectedIndexChanged
			Dim item As DevItem = DirectCast(listAudioDev.SelectedItem, DevItem)
			Dim hr As Integer
			If item IsNot Nothing Then
				If Not bIgnoreDeviceSelection Then
					ClearGraph()

					Try
						hr = InitInputDev(ms, Nothing, item)
						DsError.ThrowExceptionForHR(hr)
					Catch ex As COMException
						MessageBox.Show(ex.ToString())
						Return
					End Try

					BuildGraph()
				End If
			End If
		End Sub

		Private Sub listVideoDev_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles listVideoDev.SelectedIndexChanged
			Dim item As DevItem = DirectCast(listVideoDev.SelectedItem, DevItem)
			Dim hr As Integer
			If item IsNot Nothing Then
				If Not bIgnoreDeviceSelection Then
					ClearGraph()

					Try
						hr = InitInputDev(ms, item, Nothing)
						DsError.ThrowExceptionForHR(hr)
					Catch ex As COMException
						MessageBox.Show(ex.ToString())
						Return
					End Try

					BuildGraph()
				End If
			End If
		End Sub

		Private Sub cmdAudioDevProp_Click(ByVal sender As Object, ByVal e As EventArgs) Handles cmdAudioDevProp.Click
			If Not CheckInputDevice(ms.audioInput) Then
				Return
			End If

			ClearGraph()

			ShowPropPages(ms.audioInput)

			BuildGraph()
		End Sub

		Private Function CheckInputDevice(ByVal inputDevice As IBaseFilter) As Boolean
			If inputDevice Is Nothing Then
				MessageBox.Show("No input device is selected!")
				Return False
			End If
			Return True
		End Function

		Private Sub ShowPropPages(ByVal obj As Object)
			Dim specPropPages As ISpecifyPropertyPages = Nothing

			Try
				specPropPages = TryCast(obj, ISpecifyPropertyPages)
				If Nothing Is specPropPages Then
					MessageBox.Show("Property pages not available")
					Return
				End If

				Dim cauuid As DsCAUUID = Nothing
				Dim hr As Integer = specPropPages.GetPages(cauuid)
				DsError.ThrowExceptionForHR(hr)

				If hr = 0 AndAlso cauuid.cElems > 0 Then
					' show property pages
					hr = WinAPI.OleCreatePropertyFrame(Me.Handle, 30, 30, Nothing, 1, obj, cauuid.cElems, cauuid.pElems, 0, 0, IntPtr.Zero)

					Marshal.FreeCoTaskMem(cauuid.pElems)

				End If
			Catch ex As COMException
				MessageBox.Show(ex.ToString())
			End Try

			'do not release interfaces obtained with a cast (as), the primary interface is also released
		End Sub

		Private Sub cmdVideoDevProp_Click(ByVal sender As Object, ByVal e As EventArgs) Handles cmdVideoDevProp.Click
			If Not CheckInputDevice(ms.videoInput) Then
				Return
			End If

			ClearGraph()

			ShowPropPages(ms.videoInput)

			BuildGraph()
		End Sub

		Private Sub cmdVideoCaptureProp_Click(ByVal sender As Object, ByVal e As EventArgs) Handles cmdVideoCaptureProp.Click
			If Not CheckInputDevice(ms.videoInput) Then
				Return
			End If

			Dim hr As Integer = 0
			Dim streamConfigId As Guid = GetType(IAMStreamConfig).GUID
			Dim streamConfigObj As Object = Nothing

			ClearGraph()

			Try
				hr = ms.captureGraph.FindInterface(PinCategory.Capture, DirectShowLib.MediaType.Video, ms.videoInput, streamConfigId, streamConfigObj)

				DsError.ThrowExceptionForHR(hr)

                Using form As New VideoCapturePropertiesForm()
                    form.VideoConfig = TryCast(streamConfigObj, IAMStreamConfig)
                    form.ShowDialog()
                End Using

			Catch ex As COMException
				MessageBox.Show(ex.ToString())
			Finally
				Util.ReleaseComObject(streamConfigObj)
			End Try

			BuildGraph()
		End Sub

		Private Function BuildGraph() As Boolean
			Dim hr As Integer = 0
			Dim cleanup As Boolean = True
			Dim audioConfigObj As Object = Nothing
			Dim audioConfig As IAMStreamConfig = Nothing

			cmdRecord.Enabled = False

			Try
				If ms.audioInput IsNot Nothing Then
					' Create the Audio Sample Grabber.
					ms.audioSampleGrabber = TryCast(New SampleGrabber(), IBaseFilter)
					If ms.audioSampleGrabber Is Nothing Then
						Throw New COMException("Cannot create SampleGrabber")
					End If

					hr = ms.graph.AddFilter(ms.audioSampleGrabber, "Audio SampleGrabber")
					DsError.ThrowExceptionForHR(hr)

					ms.audioGrabber = TryCast(ms.audioSampleGrabber, ISampleGrabber)
					If ms.audioGrabber Is Nothing Then
						Throw New COMException("Cannot obtain ISampleGrabber")
					End If

					' Create and add the audio null renderer in the graph
					ms.audioNullRenderer = TryCast(New NullRenderer(), IBaseFilter)
					If ms.audioNullRenderer Is Nothing Then
						Throw New COMException("Cannot create NullRenderer")
					End If

					hr = ms.graph.AddFilter(ms.audioNullRenderer, "Audio NullRenderer")
					DsError.ThrowExceptionForHR(hr)

					' manually connect the filters
					hr = Util.ConnectFilters(ms.graph, ms.audioInput, ms.audioSampleGrabber)
					DsError.ThrowExceptionForHR(hr)

					hr = Util.ConnectFilters(ms.graph, ms.audioSampleGrabber, ms.audioNullRenderer)
					DsError.ThrowExceptionForHR(hr)
				End If

				ms.videoSampleGrabber = TryCast(New SampleGrabber(), IBaseFilter)
				If ms.videoSampleGrabber Is Nothing Then
					Throw New COMException("Cannot create SampleGrabber")
				End If

				hr = ms.graph.AddFilter(ms.videoSampleGrabber, "Video SampleGrabber")
				DsError.ThrowExceptionForHR(hr)

				ms.videoGrabber = TryCast(ms.videoSampleGrabber, ISampleGrabber)
				If ms.videoGrabber Is Nothing Then
					Throw New COMException("Cannot obtain ISampleGrabber")
				End If

				' Create and add the video null renderer in the graph
				ms.videoNullRenderer = TryCast(New NullRenderer(), IBaseFilter)
				If ms.videoNullRenderer Is Nothing Then
					Throw New COMException("Cannot create NullRenderer")
				End If

				hr = ms.graph.AddFilter(ms.videoNullRenderer, "Video NullRenderer")
				DsError.ThrowExceptionForHR(hr)

				' add the smart tee if preview is required
				If ms.smartTee Is Nothing Then
					ms.smartTee = TryCast(New SmartTee(), IBaseFilter)
					hr = ms.graph.AddFilter(ms.smartTee, "Smart Tee")
					DsError.ThrowExceptionForHR(hr)
				End If

				If ms.smartTee IsNot Nothing Then
					' connect the video input to the smart tee
					Util.ConnectFilters(ms.graph, ms.videoInput, ms.smartTee)

					' connect smart tee capture to video grabber
					Dim capturePin As IPin = Nothing
					hr = Util.GetPin(ms.smartTee, PinDirection.Output, "Capture", capturePin)
					DsError.ThrowExceptionForHR(hr)

					Dim videoGrabberPin As IPin = Nothing
					hr = Util.GetUnconnectedPin(ms.videoSampleGrabber, PinDirection.Input, videoGrabberPin)
					DsError.ThrowExceptionForHR(hr)

					hr = ms.graph.ConnectDirect(capturePin, videoGrabberPin, Nothing)
					DsError.ThrowExceptionForHR(hr)

					' connect smart tee preview to video renderer
					ms.previewRenderer = TryCast(New VideoRendererDefault(), IBaseFilter)
					hr = ms.graph.AddFilter(ms.previewRenderer, "Preview Renderer")
					DsError.ThrowExceptionForHR(hr)

					Dim previewPin As IPin = Nothing
					hr = Util.GetPin(ms.smartTee, PinDirection.Output, "Preview", previewPin)
					DsError.ThrowExceptionForHR(hr)

					Dim videoRendererPin As IPin = Nothing
					hr = Util.GetUnconnectedPin(ms.previewRenderer, PinDirection.Input, videoRendererPin)
					DsError.ThrowExceptionForHR(hr)

					hr = ms.graph.Connect(previewPin, videoRendererPin)
					DsError.ThrowExceptionForHR(hr)
				Else
					hr = Util.ConnectFilters(ms.graph, ms.videoInput, ms.videoSampleGrabber)
					DsError.ThrowExceptionForHR(hr)
				End If

				hr = Util.ConnectFilters(ms.graph, ms.videoSampleGrabber, ms.videoNullRenderer)
				DsError.ThrowExceptionForHR(hr)

				ms.mediaControl = TryCast(ms.graph, IMediaControl)
				If Nothing Is ms.mediaControl Then
					Throw New COMException("Cannot obtain IMediaControl")
				End If

				If ms.audioInput IsNot Nothing Then
					' configure input audio
					Dim streamConfigId As Guid = GetType(IAMStreamConfig).GUID
					hr = ms.captureGraph.FindInterface(PinCategory.Capture, Nothing, ms.audioInput, streamConfigId, audioConfigObj)
					DsError.ThrowExceptionForHR(hr)

					audioConfig = TryCast(audioConfigObj, IAMStreamConfig)
					If Nothing Is audioConfig Then
						Throw New COMException("Cannot obtain IAMStreamConfig")
					End If

					Dim audioType As AMMediaType = Nothing
					hr = audioConfig.GetFormat(audioType)
					DsError.ThrowExceptionForHR(hr)

					Dim wfx As New WaveFormatEx()
					Marshal.PtrToStructure(audioType.formatPtr, wfx)

					' set audio capture parameters
					wfx.nSamplesPerSec = 48000
					wfx.nChannels = 2
					wfx.wBitsPerSample = 16
                    wfx.nBlockAlign = CShort(Math.Truncate(wfx.nChannels * wfx.wBitsPerSample / 8))
					wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign
					'wfx.wFormatTag = 1; // PCM
					Marshal.StructureToPtr(wfx, audioType.formatPtr, False)
					hr = audioConfig.SetFormat(audioType)
					DsUtils.FreeAMMediaType(audioType)
					DsError.ThrowExceptionForHR(hr)

					' Store the audio media type for later use.
					hr = ms.audioGrabber.GetConnectedMediaType(ms.audioType)
					DsError.ThrowExceptionForHR(hr)
				Else
					' There's no audio media type because the audio input is disabled
					ms.audioType.majorType = DirectShowLib.MediaType.Null
				End If

				Try
					ms.droppedFrames = TryCast(ms.videoInput, IAMDroppedFrames)
					'the video capture device may not support IAMDroppedFrames
				Catch
				End Try


				' Store the video media type for later use.
				hr = ms.videoGrabber.GetConnectedMediaType(ms.videoType)
				DsError.ThrowExceptionForHR(hr)

				Dim ivw As IVideoWindow = TryCast(ms.graph, IVideoWindow)
				Try
					hr = ivw.put_Owner(previewBox.Handle)
					DsError.ThrowExceptionForHR(hr)

					hr = ivw.put_WindowStyle(WindowStyle.Child Or WindowStyle.ClipChildren Or WindowStyle.ClipSiblings)
					DsError.ThrowExceptionForHR(hr)

					hr = ivw.put_Visible(OABool.True)
					DsError.ThrowExceptionForHR(hr)

					Dim rc As Rectangle = previewBox.ClientRectangle
					hr = ivw.SetWindowPosition(0, 0, rc.Right, rc.Bottom)
					DsError.ThrowExceptionForHR(hr)

				Catch
				End Try

				hr = ms.mediaControl.Run()
				DsError.ThrowExceptionForHR(hr)

				cleanup = False
			Catch ex As COMException
				MessageBox.Show(ex.ToString())
				Return False
			Finally
				Util.ReleaseComObject(audioConfigObj)
				If cleanup Then
					ms.Reset(False)
					m_audioCB.Reset()
					m_videoCB.Reset()
				End If
			End Try

			cmdRecord.Enabled = True

			Return True
		End Function

		Private Sub ClearGraph()
			ms.Reset(False) ' leave the input devices in the graph
		End Sub

		Private Function StartRecording() As Boolean
			If Nothing Is ms.videoInput Then
				MessageBox.Show("No video input!")
				Return False
			End If

			If txtOutput.Text.Length = 0 Then
				MessageBox.Show("Please choose output file.")
				Return False
			End If

			If System.IO.File.Exists(txtOutput.Text) Then
				Dim prompt As String = String.Format("{0} already exists. Do you want to replace the file?", txtOutput.Text)
				If System.Windows.Forms.DialogResult.Yes <> MessageBox.Show(prompt, "Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) Then
					Return False
				End If

				Try
					File.Delete(txtOutput.Text)
				Catch
				End Try
			End If


			Dim hr As Integer = 0
			Try

				hr = ms.mediaControl.Stop()
				DsError.ThrowExceptionForHR(hr)

				If ms.audioInput IsNot Nothing Then
					hr = ms.audioGrabber.SetCallback(m_audioCB, CInt(CBMethod.Sample))
					DsError.ThrowExceptionForHR(hr)
				End If

				hr = ms.videoGrabber.SetCallback(m_videoCB, CInt(CBMethod.Sample))
				DsError.ThrowExceptionForHR(hr)

				m_audioCB.Reset()
				m_videoCB.Reset()

				' pass the media state to the callbacks
				m_audioCB.MediaState = ms
				m_videoCB.MediaState = ms
				m_audioCB.MainWindow = m_mainWindow
				m_videoCB.MainWindow = m_mainWindow

				If Not ConfigureTranscoder() Then
					Return False
				End If

				hr = ms.mediaControl.Pause()
				DsError.ThrowExceptionForHR(hr)

				System.Threading.Thread.Sleep(300)

				hr = ms.mediaControl.Run()
				DsError.ThrowExceptionForHR(hr)

				ResetStats()

				recStartTime = Date.Now
				fpsStartTime = recStartTime
				fpsNotDropped = 0
				statsTimer.Start()
			Catch ex As COMException
				MessageBox.Show(ex.ToString())
				Return False
			End Try

			Return True
		End Function

		Private Shared Sub SetH264FastEncoding(ByVal plist As IDictionary(Of String, Object))
			plist.Add(Param.Encoder.Video.H264.Profile, 66) ' Baseline
			plist.Add(Param.Encoder.Video.H264.EntropyCodingMode, 0) ' CAVLC
			plist.Add(Param.Encoder.Video.H264.NumBFrames, 0)
			plist.Add(Param.Encoder.Video.H264.NumRefFrames, 1)
			plist.Add(Param.Encoder.Video.H264.Transform8x8, False)
			plist.Add(Param.Encoder.Video.H264.KeyFrameInterval, 15)
			plist.Add(Param.Encoder.Video.H264.KeyFrameIDRInterval, 1)
			plist.Add(Param.Encoder.Video.H264.QualitySpeed, 0) ' max speed
			plist.Add(Param.Encoder.Video.H264.RateControlMethod, 3) ' constant quantizers
			plist.Add(Param.Encoder.Video.H264.RateControlQuantI, 26) ' 0-51, default 20
			plist.Add(Param.Encoder.Video.H264.RateControlQuantP, 26) ' 0-51, default 20
			plist.Add(Param.Encoder.Video.H264.DeblockingFilter, 2) ' on, within slice boundaries
			plist.Add(Param.Encoder.Video.H264.MESplitMode, 0) ' only 16x16 blocks
			plist.Add(Param.Encoder.Video.H264.MEMethod, 8) ' UHM
		End Sub

		Private Function CustomOutputSocket(ByVal frameWidth As Integer, ByVal frameHeight As Integer, ByVal framerate As Double) As MediaSocket
			Dim socket = New MediaSocket()

			' video pin
            socket.Pins.Add(New MediaPin() With {.StreamInfo = New VideoStreamInfo() With {.StreamType = StreamType.H264, .FrameWidth = frameWidth, .FrameHeight = frameHeight, .FrameRate = framerate, .DisplayRatioWidth = frameWidth, .DisplayRatioHeight = frameHeight}, .Params = New ParameterList()})

'            
'             * Set H264 encoder params on the output pin to get the best encoding speed
'              
			SetH264FastEncoding(socket.Pins(0).Params)
			socket.Pins(0).Params.Add(Param.Encoder.Video.H264.FixedFramerate, False)

			' audio pin
			socket.Pins.Add(New MediaPin() With {
				.StreamInfo = New AudioStreamInfo() With {.StreamType = StreamType.Aac, .SampleRate = 44100, .Channels = 2}
			})

			Return socket
		End Function

		Private Function ConfigureTranscoder() As Boolean
			ms.transcoder = New PrimoSoftware.AVBlocks.Transcoder()
			ms.transcoder.AllowDemoMode = True

			' set audio input pin if audio is not disabled
			If ms.audioInput IsNot Nothing Then
				If (ms.audioType.majorType <> DirectShowLib.MediaType.Audio) OrElse (ms.audioType.formatType <> DirectShowLib.FormatType.WaveEx) Then
					Return False
				End If

				Dim wfx As New WaveFormatEx()
				Marshal.PtrToStructure(ms.audioType.formatPtr, wfx)

				If wfx.wFormatTag <> 1 Then ' WAVE_FORMAT_PCM
					Return False
				End If

				Dim audioInfo As New AudioStreamInfo()
				audioInfo.BitsPerSample = wfx.wBitsPerSample
				audioInfo.Channels = wfx.nChannels
				audioInfo.SampleRate = wfx.nSamplesPerSec
				audioInfo.StreamType = StreamType.LPCM

                Dim inputAudioPin As New MediaPin()
                inputAudioPin.StreamInfo = audioInfo

                Dim inputAudioSocket As New MediaSocket()
                inputAudioSocket.Pins.Add(inputAudioPin)
                inputAudioSocket.StreamType = StreamType.LPCM

				m_audioCB.StreamNumber = ms.transcoder.Inputs.Count
                ms.transcoder.Inputs.Add(inputAudioSocket)
			End If

			' set video input pin
            If (ms.videoType.majorType <> DirectShowLib.MediaType.Video) OrElse (ms.videoType.formatType <> DirectShowLib.FormatType.VideoInfo) Then
                Return False
            End If

            Dim videoInfo As New VideoStreamInfo()
            Dim vih As VideoInfoHeader = DirectCast(Marshal.PtrToStructure(ms.videoType.formatPtr, GetType(VideoInfoHeader)), VideoInfoHeader)

            If vih.AvgTimePerFrame > 0 Then
                videoInfo.FrameRate = CDbl(10000000) / vih.AvgTimePerFrame
            End If

            videoInfo.Bitrate = 0 'vih.BitRate;
            videoInfo.FrameHeight = Math.Abs(vih.BmiHeader.Height)
            videoInfo.FrameWidth = vih.BmiHeader.Width
            videoInfo.DisplayRatioWidth = videoInfo.FrameWidth
            videoInfo.DisplayRatioHeight = videoInfo.FrameHeight
            videoInfo.ScanType = ScanType.Progressive
            videoInfo.Duration = 0

            If ms.videoType.subType = MediaSubType.MJPG Then
                videoInfo.StreamType = StreamType.Mjpeg
                videoInfo.ColorFormat = ColorFormat.YUV422
            Else
                videoInfo.StreamType = StreamType.UncompressedVideo
                videoInfo.ColorFormat = Util.GetColorFormat(ms.videoType.subType)
            End If

            ' unsupported capture format
            If videoInfo.ColorFormat = ColorFormat.Unknown Then
                Return False
            End If

            Select Case videoInfo.ColorFormat
                Case ColorFormat.BGR32, ColorFormat.BGRA32, ColorFormat.BGR24, ColorFormat.BGR444, ColorFormat.BGR555, ColorFormat.BGR565
                    videoInfo.FrameBottomUp = (vih.BmiHeader.Height > 0)
            End Select

            Dim inputVideoPin As New MediaPin()
            inputVideoPin.StreamInfo = videoInfo

            Dim inputVideoSocket As New MediaSocket()
            inputVideoSocket.Pins.Add(inputVideoPin)
            inputVideoSocket.StreamType = StreamType.UncompressedVideo

            m_videoCB.StreamNumber = ms.transcoder.Inputs.Count
            ms.transcoder.Inputs.Add(inputVideoSocket)

			Dim preset As PresetDescriptor = TryCast(comboPresets.SelectedItem, PresetDescriptor)
			Dim outputSocket As MediaSocket

			' custom output sockets
			If preset.Name = "custom-mp4-h264-704x576-25fps-aac" Then
				outputSocket = CustomOutputSocket(704, 576, 25)
			ElseIf preset.Name = "custom-mp4-h264-704x576-12fps-aac" Then
				outputSocket = CustomOutputSocket(704, 576, 12)
			ElseIf preset.Name = "custom-mp4-h264-352x288-25fps-aac" Then
				outputSocket = CustomOutputSocket(352, 288, 25)
			ElseIf preset.Name = "custom-mp4-h264-352x288-12fps-aac" Then
				outputSocket = CustomOutputSocket(352, 288, 12)
			Else
				outputSocket = MediaSocket.FromPreset(preset.Name)
			End If

			outputSocket.File = txtOutput.Text

			ms.transcoder.Outputs.Add(outputSocket)

			If Not ms.transcoder.Open() Then
				Return False
			End If

			Return True
		End Function

		Private Sub StopRecording()
			statsTimer.Stop()

			ClearGraph()

			m_audioCB.Reset()
			m_videoCB.Reset()

			BuildGraph()
		End Sub

		Private Sub cmdRecord_Click(ByVal sender As Object, ByVal e As EventArgs) Handles cmdRecord.Click
			If bCmdRecordBusy Then
				Return
			End If

			bCmdRecordBusy = True
			cmdRecord.Enabled = False

			If bRecording Then
				' stop recording
				StopRecording()

				txtRecording.Visible = False
				cmdRecord.Text = sStartRecording
				EnableCommandUI(True)
				bRecording = False
			Else
				' start recording
				Try
					EnableCommandUI(False)
					If StartRecording() Then
						txtRecording.Visible = True
						cmdRecord.Text = sStopRecording

						bRecording = True
					Else
						EnableCommandUI(True)
					End If
				Catch ex As COMException
					MessageBox.Show(ex.ToString())
				End Try
			End If

			cmdRecord.Enabled = True
			bCmdRecordBusy = False
		End Sub

		Protected Overrides Sub WndProc(ByRef m As Message)
			If m.Msg = Util.WM_STOP_CAPTURE Then
				System.Diagnostics.Trace.WriteLine("WM_STOP_CAPTURE WParam:" & m.WParam.ToInt32().ToString())

				cmdRecord_Click(Nothing, Nothing)

				Dim stopReason As Integer = m.WParam.ToInt32()

				If stopReason >= 0 Then
					MessageBox.Show(Me, "An error occurred encoding captured data. The recording has been stopped.", "AVBlocks", MessageBoxButtons.OK, MessageBoxIcon.Error)
				Else
					MessageBox.Show(Me, "An error occurred while recording. The recording has been stopped.", "Unexpected Error", MessageBoxButtons.OK, MessageBoxIcon.Error)
				End If
			Else
				MyBase.WndProc(m)
			End If
		End Sub

		Private Sub RecorderForm_FormClosing(ByVal sender As Object, ByVal e As FormClosingEventArgs) Handles MyBase.FormClosing
			If bRecording Then
				statsTimer.Stop()

				ms.Reset(True)

				m_audioCB.Reset()
				m_videoCB.Reset()

				Thread.Sleep(300)
			Else
				ms.Reset(True)
			End If
		End Sub

		Private Class DevItem
			Public Sub New()
			End Sub
			Public Sub New(ByVal friendlyName As String, ByVal displayName As String)
				Me.FriendlyName = friendlyName
				Me.DisplayName = displayName
			End Sub

			Public FriendlyName As String
			Public DisplayName As String ' null designates disabled input

			Public Overrides Function ToString() As String
				Return FriendlyName
			End Function

			Public ReadOnly Property Disabled() As Boolean
				Get
					Return DisplayName Is Nothing
				End Get
			End Property
		End Class

		Private Sub EnableCommandUI(ByVal enable As Boolean)
			listAudioDev.Enabled = enable
			cmdAudioDevProp.Enabled = enable
			listVideoDev.Enabled = enable
			cmdVideoDevProp.Enabled = enable
			cmdVideoCaptureProp.Enabled = enable
			comboPresets.Enabled = enable
			btnChooseOutput.Enabled = enable
		End Sub

		Private Sub btnChooseOutput_Click(ByVal sender As Object, ByVal e As EventArgs) Handles btnChooseOutput.Click
			Dim preset As PresetDescriptor = TryCast(comboPresets.SelectedItem, PresetDescriptor)

			Dim dlg As New SaveFileDialog()

			Dim filter As String = String.Empty

			If Not String.IsNullOrEmpty(preset.FileExtension) Then
				dlg.DefaultExt = preset.FileExtension
				filter = String.Format("(*.{0})|*.{0}|", preset.FileExtension)
			End If

			filter &= "All files (*.*)|*.*"
			dlg.Filter = filter
			dlg.OverwritePrompt = True

			If txtOutput.Text.Length > 0 Then
				dlg.FileName = System.IO.Path.GetFileName(txtOutput.Text)
				dlg.InitialDirectory = System.IO.Path.GetDirectoryName(txtOutput.Text)
			End If

			If System.Windows.Forms.DialogResult.OK <> dlg.ShowDialog() Then
				Return
			End If

			txtOutput.Text = dlg.FileName
		End Sub

		Private Sub comboPresets_SelectedIndexChanged(ByVal sender As Object, ByVal e As EventArgs) Handles comboPresets.SelectedIndexChanged
			Dim preset As PresetDescriptor = TryCast(comboPresets.SelectedItem, PresetDescriptor)

			If String.IsNullOrEmpty(preset.FileExtension) OrElse txtOutput.Text.Length = 0 Then
				Return
			End If

			Dim newExt As String = "." & preset.FileExtension
			Dim oldExt As String = System.IO.Path.GetExtension(txtOutput.Text)

			If oldExt = newExt Then
				Return
			End If

			Dim newFile As String = System.IO.Path.ChangeExtension(txtOutput.Text, newExt)

			If System.IO.File.Exists(newFile) Then
				Dim prompt As String = String.Format("{0} already exists. Do you want to replace the file?", newFile)
				If System.Windows.Forms.DialogResult.Yes <> MessageBox.Show(prompt, "Warning", MessageBoxButtons.YesNo, MessageBoxIcon.Warning) Then
					Return
				End If
			End If

			txtOutput.Text = newFile
		End Sub
	End Class
End Namespace
