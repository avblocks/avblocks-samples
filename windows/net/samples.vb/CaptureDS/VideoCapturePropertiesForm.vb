Imports Microsoft.VisualBasic
Imports System
Imports System.Collections.Generic
Imports System.ComponentModel
Imports System.Data
Imports System.Drawing
Imports System.Text
Imports System.Windows.Forms
Imports DirectShowLib
Imports System.Runtime.InteropServices
Imports System.Runtime.InteropServices.ComTypes

Namespace CaptureDS
	Partial Public Class VideoCapturePropertiesForm
		Inherits Form
		Public Sub New()
			InitializeComponent()
		End Sub

		Private Sub SetFormat(ByVal formatIndex As Integer, ByVal frameRate As Integer)
			Dim capsCount, capSize As Integer
			Dim hr As Integer = VideoConfig.GetNumberOfCapabilities(capsCount, capSize)
			DsError.ThrowExceptionForHR(hr)

			Dim pSC As IntPtr = Marshal.AllocHGlobal(capSize)
			Dim mt As AMMediaType = Nothing

			Try
				Dim vih As New VideoInfoHeader()

				hr = VideoConfig.GetStreamCaps(formatIndex, mt, pSC)
				DsError.ThrowExceptionForHR(hr)

				If frameRate > 0 Then
					Marshal.PtrToStructure(mt.formatPtr, vih)
					vih.AvgTimePerFrame = CLng(Fix(10000000.0 / frameRate))
					Marshal.StructureToPtr(vih, mt.formatPtr, False)
				End If

				hr = VideoConfig.SetFormat(mt)
				DsError.ThrowExceptionForHR(hr)
			Finally
				DsUtils.FreeAMMediaType(mt)
				Marshal.FreeHGlobal(pSC)
			End Try
		End Sub

		Private Sub buttonOK_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonOK.Click
			Dim itemFormat As ComboboxItem = TryCast(comboBoxFormats.SelectedItem, ComboboxItem)
			If itemFormat IsNot Nothing Then
				Dim frameRate As Integer = -1
				Dim itemFPS As ComboboxItem = TryCast(comboBoxFrameRate.SelectedItem, ComboboxItem)
				If Nothing IsNot itemFPS Then
					frameRate = itemFPS.Value
				End If

				Try
					SetFormat(itemFormat.Value, frameRate)
				Catch e1 As Exception
					MessageBox.Show("Failed to set the selected video format.")
				End Try
			End If

			DialogResult = System.Windows.Forms.DialogResult.OK
		End Sub

		Private Sub buttonCancel_Click(ByVal sender As Object, ByVal e As EventArgs) Handles buttonCancel.Click
			DialogResult = System.Windows.Forms.DialogResult.Cancel
		End Sub

		Private privateVideoConfig As IAMStreamConfig
		Public Property VideoConfig() As IAMStreamConfig
			Get
				Return privateVideoConfig
			End Get
			Set(ByVal value As IAMStreamConfig)
				privateVideoConfig = value
			End Set
		End Property

		Private Structure FormatEntry
			Public videoSubType As Guid
			Public Name As String

			Public Sub New(ByVal videoSubType As Guid, ByVal name As String)
				Me.videoSubType = videoSubType
				Me.Name = name
			End Sub
		End Structure

		Private Shared FormatsTab() As FormatEntry = { New FormatEntry(MediaSubType.MJPG, "MJPG"), New FormatEntry(MediaSubType.RGB24, "RGB24"), New FormatEntry(MediaSubType.ARGB32, "ARGB32"), New FormatEntry(MediaSubType.RGB32, "RGB32"), New FormatEntry(MediaSubType.RGB565, "RGB565"), New FormatEntry(MediaSubType.ARGB1555, "ARGB1555"), New FormatEntry(MediaSubType.RGB555, "RGB555"), New FormatEntry(MediaSubType.ARGB4444, "ARGB4444"), New FormatEntry(MediaSubType.YV12, "YV12"), New FormatEntry(MediaSubType.I420, "I420"), New FormatEntry(MediaSubType.IYUV, "IYUV"), New FormatEntry(MediaSubType.YUY2, "YUY2"), New FormatEntry(MediaSubType.NV12, "NV12"), New FormatEntry(MediaSubType.UYVY, "UYVY"), New FormatEntry(MediaSubType.Y411, "Y411"), New FormatEntry(MediaSubType.Y41P, "Y41P"), New FormatEntry(MediaSubType.YVU9, "YVU9") }

		Private Function GetSubtypeString(ByVal videoSubtype As Guid) As String
			For i As Integer = 0 To FormatsTab.Length - 1
				If FormatsTab(i).videoSubType = videoSubtype Then
					Return FormatsTab(i).Name
				End If
			Next i

			Return Nothing
		End Function

		Public Class ComboboxItem
			Private privateText As String
			Public Property Text() As String
				Get
					Return privateText
				End Get
				Set(ByVal value As String)
					privateText = value
				End Set
			End Property
			Private privateValue As Integer
			Public Property Value() As Integer
				Get
					Return privateValue
				End Get
				Set(ByVal value As Integer)
					privateValue = value
				End Set
			End Property

			Public Overrides Function ToString() As String
				Return Text
			End Function
		End Class

		Private Sub VideoCapturePropertiesForm_Load(ByVal sender As Object, ByVal e As EventArgs) Handles MyBase.Load
			Try
				InitFormatsList()
			Catch e1 As Exception
				MessageBox.Show("Failed to set init the formats list.")
			End Try
		End Sub

		Private Sub InitFormatsList()
			Dim hr As Integer = 0

			If Nothing Is VideoConfig Then
				Throw New Exception("VideoConfig not set")
			End If

			' enumerate the capabilities of the video capture device
			Dim capsCount, capSize As Integer
			hr = VideoConfig.GetNumberOfCapabilities(capsCount, capSize)
			DsError.ThrowExceptionForHR(hr)

			Dim vih As New VideoInfoHeader()
			Dim vsc As New VideoStreamConfigCaps()
			Dim pSC As IntPtr = Marshal.AllocHGlobal(capSize)

			Try
				Dim videoFormatIndex As Integer = -1
				Dim minFps As Integer = -1
				Dim maxFps As Integer = -1

				Dim currentWidth As Integer = 0
				Dim currentHeight As Integer = 0
				Dim currentSubType As Guid
				Dim currentFps As Integer = 0

                Dim mt1 As AMMediaType = Nothing
                hr = VideoConfig.GetFormat(mt1)
                Marshal.PtrToStructure(mt1.formatPtr, vih)
                DsError.ThrowExceptionForHR(hr)

                currentFps = CInt(Fix(10000000.0 / vih.AvgTimePerFrame))
                currentWidth = vih.BmiHeader.Width
                currentHeight = vih.BmiHeader.Height
                currentSubType = mt1.subType

                DsUtils.FreeAMMediaType(mt1)

				For i As Integer = 0 To capsCount - 1
					Dim mt As AMMediaType = Nothing

					' the video format is described in AMMediaType and VideoStreamConfigCaps
					hr = VideoConfig.GetStreamCaps(i, mt, pSC)
					DsError.ThrowExceptionForHR(hr)

					If mt.formatType = DirectShowLib.FormatType.VideoInfo Then
						Dim formatName As String = GetSubtypeString(mt.subType)

						If (Not String.IsNullOrEmpty(formatName)) Then
							' copy the unmanaged structures to managed in order to check the format
							Marshal.PtrToStructure(mt.formatPtr, vih)
							Marshal.PtrToStructure(pSC, vsc)

							Dim fps As Integer = CInt(Fix(10000000.0 / vsc.MaxFrameInterval))
							If (minFps < 0) OrElse (minFps > fps) Then
								minFps = fps
							End If

							fps = CInt(Fix(10000000.0 / vsc.MinFrameInterval))
							If (maxFps < 0) OrElse (maxFps < fps) Then
								maxFps = fps
							End If

							Dim capline As String = String.Format("{0} x {1}, min fps {2:0.}, max fps {3:0.}, {4}", vih.BmiHeader.Width, vih.BmiHeader.Height, 10000000.0 / vsc.MaxFrameInterval, 10000000.0 / vsc.MinFrameInterval, formatName)

							If (vih.BmiHeader.Width = currentWidth) AndAlso (vih.BmiHeader.Height = currentHeight) AndAlso (mt.subType = currentSubType) Then
								videoFormatIndex = comboBoxFormats.Items.Count
							End If

							Dim item As New ComboboxItem()
							item.Text = capline
							item.Value = i

							comboBoxFormats.Items.Add(item)
						End If
					End If

					DsUtils.FreeAMMediaType(mt)
				Next i

				If videoFormatIndex >= 0 Then
					comboBoxFormats.SelectedIndex = videoFormatIndex
				End If

				If (minFps >= 0) AndAlso (maxFps >= 0) Then
					For i As Integer = minFps To maxFps
						Dim item As New ComboboxItem()
						item.Text = i.ToString()
						item.Value = i

						comboBoxFrameRate.Items.Add(item)

						If currentFps = i Then
							comboBoxFrameRate.SelectedIndex = comboBoxFrameRate.Items.Count - 1
						End If
					Next i
				End If
			Finally
				Marshal.FreeHGlobal(pSC)
			End Try
		End Sub
	End Class
End Namespace
