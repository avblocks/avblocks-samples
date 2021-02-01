Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Runtime.InteropServices
Imports System.Runtime.InteropServices.ComTypes
Imports DirectShowLib
Imports System.Diagnostics
Imports PrimoSoftware.AVBlocks

Namespace CaptureDS
	Friend NotInheritable Class Util

		Private Sub New()
		End Sub
		Public Const WM_STOP_CAPTURE As Integer = WinAPI.WM_APP + 1

		Private Structure ColorSpaceEntry
			Public videoSubType As Guid
			Public colorFormat As ColorFormat

			Public Sub New(ByVal videoSubType As Guid, ByVal colorFormat As ColorFormat)
				Me.videoSubType = videoSubType
				Me.colorFormat = colorFormat
			End Sub
		End Structure

        Private Shared ColorSpaceTab() As ColorSpaceEntry = {
         New ColorSpaceEntry(MediaSubType.RGB24, ColorFormat.BGR24),
         New ColorSpaceEntry(MediaSubType.ARGB32, ColorFormat.BGRA32),
         New ColorSpaceEntry(MediaSubType.RGB32, ColorFormat.BGR32),
         New ColorSpaceEntry(MediaSubType.RGB565, ColorFormat.BGR565),
         New ColorSpaceEntry(MediaSubType.ARGB1555, ColorFormat.BGR555),
         New ColorSpaceEntry(MediaSubType.RGB555, ColorFormat.BGR555),
         New ColorSpaceEntry(MediaSubType.ARGB4444, ColorFormat.BGR444),
         New ColorSpaceEntry(MediaSubType.YV12, ColorFormat.YV12),
         New ColorSpaceEntry(MediaSubType.I420, ColorFormat.YUV420),
         New ColorSpaceEntry(MediaSubType.IYUV, ColorFormat.YUV420),
         New ColorSpaceEntry(MediaSubType.YUY2, ColorFormat.YUY2),
         New ColorSpaceEntry(MediaSubType.NV12, ColorFormat.NV12),
         New ColorSpaceEntry(MediaSubType.UYVY, ColorFormat.UYVY),
         New ColorSpaceEntry(MediaSubType.Y411, ColorFormat.Y411),
         New ColorSpaceEntry(MediaSubType.Y41P, ColorFormat.Y41P),
         New ColorSpaceEntry(MediaSubType.YVU9, ColorFormat.YVU9)
        }

		Public Shared Sub ReleaseComObject(Of T)(ByRef comObject As T)
			If comObject IsNot Nothing Then
				Marshal.ReleaseComObject(comObject)

				comObject = Nothing
			End If
		End Sub

		Public Shared Sub DisposeObject(Of T As IDisposable)(ByRef obj As T)
			If obj IsNot Nothing Then
				obj.Dispose()
				obj = Nothing
			End If
		End Sub

		Public Shared Function GetPin(ByVal filter As IBaseFilter, ByVal pinDir As PinDirection, ByVal name As String, <System.Runtime.InteropServices.Out()> ByRef ppPin As IPin) As Integer
			ppPin = Nothing

			Dim enumPins As IEnumPins = Nothing
			Dim pins() As IPin = { Nothing }

			Try
				Dim hr As Integer = filter.EnumPins(enumPins)
				DsError.ThrowExceptionForHR(hr)

				Do While enumPins.Next(1, pins, IntPtr.Zero) = 0
					Dim pi As PinInfo = Nothing
					hr = pins(0).QueryPinInfo(pi)

					Dim found As Boolean = False
                    If hr = 0 AndAlso pi.dir = pinDir AndAlso pi.name = name Then
                        found = True

                        ppPin = pins(0)

                        DsUtils.FreePinInfo(pi)
                    End If

					If found Then
						Return 0
					End If

					Util.ReleaseComObject(pins(0))
				Loop

				' Did not find a matching pin.
			Catch e1 As COMException
			Finally
				Marshal.ReleaseComObject(enumPins)
			End Try

			Return WinAPI.E_FAIL
		End Function

		Public Shared Function GetUnconnectedPin(ByVal filter As IBaseFilter, ByVal pinDir As PinDirection, <System.Runtime.InteropServices.Out()> ByRef ppPin As IPin) As Integer
			ppPin = Nothing

			Dim enumPins As IEnumPins = Nothing
			Dim pins() As IPin = { Nothing }
			Dim tmpPin As IPin = Nothing

			Try
				Dim hr As Integer = filter.EnumPins(enumPins)
				DsError.ThrowExceptionForHR(hr)

				Do While enumPins.Next(1, pins, IntPtr.Zero) = 0
					Dim thisPinDir As PinDirection = Nothing
					hr = pins(0).QueryDirection(thisPinDir)

                    If hr = 0 AndAlso thisPinDir = pinDir Then
                        hr = pins(0).ConnectedTo(tmpPin)
                        If tmpPin IsNot Nothing Then ' Already connected, not the pin we want.
                            Util.ReleaseComObject(tmpPin)
                        Else ' Unconnected, this is the pin we want.
                            ppPin = pins(0)
                            Return 0
                        End If
                    End If

					Util.ReleaseComObject(pins(0))
				Loop

				' Did not find a matching pin.
			Catch e1 As COMException
			Finally
				Marshal.ReleaseComObject(enumPins)
			End Try

			Return WinAPI.E_FAIL
		End Function


		Public Shared Function ConnectFilters(ByVal graph As IGraphBuilder, ByVal src As IBaseFilter, ByVal dest As IBaseFilter) As Integer
			If (graph Is Nothing) OrElse (src Is Nothing) OrElse (dest Is Nothing) Then
				Return WinAPI.E_FAIL
			End If

			' Find an output pin on the upstream filter.
			Dim pinOut As IPin = Nothing
			Dim pinIn As IPin = Nothing

			Try
				Dim hr As Integer = GetUnconnectedPin(src, PinDirection.Output, pinOut)
				DsError.ThrowExceptionForHR(hr)

				' Find an input pin on the downstream filter.

				hr = GetUnconnectedPin(dest, PinDirection.Input, pinIn)
				DsError.ThrowExceptionForHR(hr)

				' Try to connect them.
				hr = graph.ConnectDirect(pinOut, pinIn, Nothing)
				DsError.ThrowExceptionForHR(hr)

				Return 0
			Catch e1 As COMException
			Finally
				Util.ReleaseComObject(pinIn)
				Util.ReleaseComObject(pinOut)
			End Try

			Return WinAPI.E_FAIL
		End Function

		Public Shared Function GetColorFormat(ByRef videoSubtype As Guid) As ColorFormat
			For i As Integer = 0 To ColorSpaceTab.Length - 1
				If ColorSpaceTab(i).videoSubType = videoSubtype Then
					Return ColorSpaceTab(i).colorFormat
				End If
			Next i

			Return ColorFormat.Unknown
		End Function

		' Tear down everything downstream of a given filter
		Public Shared Function NukeDownstream(ByVal graph As IFilterGraph, ByVal filter As IBaseFilter) As Integer
			If filter Is Nothing Then
				Return WinAPI.E_FAIL
			End If

			Dim enumPins As IEnumPins = Nothing
			Dim pins() As IPin = { Nothing }

			Try
				Dim hr As Integer = filter.EnumPins(enumPins)
				DsError.ThrowExceptionForHR(hr)
				enumPins.Reset() ' start at the first pin

				Do While enumPins.Next(1, pins, IntPtr.Zero) = 0
					If pins(0) IsNot Nothing Then
						Dim pindir As PinDirection = Nothing
						pins(0).QueryDirection(pindir)
                        If pindir = PinDirection.Output Then
                            Dim pTo As IPin = Nothing
                            pins(0).ConnectedTo(pTo)
                            If pTo IsNot Nothing Then
                                Dim pi As PinInfo = Nothing
                                hr = pTo.QueryPinInfo(pi)

                                If hr = 0 Then
                                    NukeDownstream(graph, pi.filter)

                                    graph.Disconnect(pTo)
                                    graph.Disconnect(pins(0))
                                    graph.RemoveFilter(pi.filter)

                                    Util.ReleaseComObject(pi.filter)
                                    DsUtils.FreePinInfo(pi)
                                End If
                                Marshal.ReleaseComObject(pTo)
                            End If
                        End If
						Marshal.ReleaseComObject(pins(0))
					End If
				Loop

				Return 0
			Catch e1 As COMException
			Finally
				Marshal.ReleaseComObject(enumPins)
			End Try

			Return WinAPI.E_FAIL

		End Function
	End Class
End Namespace
