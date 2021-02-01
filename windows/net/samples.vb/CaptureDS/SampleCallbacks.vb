Imports System
Imports System.IO
Imports System.Runtime.InteropServices
Imports System.Windows.Forms
Imports System.Diagnostics
Imports DirectShowLib
Imports PrimoSoftware.AVBlocks

Namespace CaptureDS
	Friend Class SampleGrabberCB
		Implements ISampleGrabberCB

		Protected lastMediaTime As Long = -1

		Protected name As String
        Protected _mediaState As MediaState
        Protected bProcess As Boolean = True

        ' how many times the callback has been called
        Protected _sampleIndex As Long

        ' how many samples are processed (based on the media time)
        Protected sampleProcessed As Long

        ' how many samples are dropped (based on the media time)
        Protected sampleDropped As Long

        Protected _streamNumber As Integer

        Protected _mainWindow As IntPtr

        Private sampleBuffer() As Byte

        Private Function ProcessSample(ByVal pBuffer As IntPtr, ByVal dataLen As Integer, ByVal sampleTime As Double) As Boolean
            Dim pushResult As Boolean = True

            If (_mediaState IsNot Nothing) AndAlso ((_mediaState.transcoder IsNot Nothing)) Then
                If sampleTime < 0 Then
                    sampleTime = 0
                End If

                If (Nothing Is sampleBuffer) OrElse (sampleBuffer.Length <> dataLen) Then
                    sampleBuffer = New Byte(dataLen - 1) {}
                End If

                Marshal.Copy(pBuffer, sampleBuffer, 0, dataLen)

                Dim inputSample As New MediaSample()
                inputSample.Buffer = New MediaBuffer(sampleBuffer)
                inputSample.StartTime = sampleTime

                ' System.Diagnostics.Debug.WriteLine(string.Format("transcoder.Push(stream: {0}, sampleTime: {1}, sampleData: {2})", StreamNumber, inputSample.StartTime, inputSample.Buffer.DataSize));

                ' transcoder.Push() is not threads safe.
                ' lock (transcoder){ } ensure that only one thread is calling transcoder.Push()
                SyncLock _mediaState.transcoder
                    pushResult = _mediaState.transcoder.Push(StreamNumber, inputSample)
                End SyncLock
            End If

            If pushResult Then
                Return True
            End If

            WinAPI.PostMessage(MainWindow, Util.WM_STOP_CAPTURE, New IntPtr(_streamNumber), IntPtr.Zero)
            bProcess = False
            Return False
        End Function


        Public Function BufferCB(ByVal sampleTime As Double, ByVal pBuffer As IntPtr, ByVal bufferLen As Integer) As Integer Implements ISampleGrabberCB.BufferCB
            If Not bProcess Then
                Return WinAPI.E_FAIL
            End If

            _sampleIndex += 1

            Dim processed As Boolean = ProcessSample(pBuffer, bufferLen, sampleTime)

            Return If(processed, WinAPI.S_OK, WinAPI.E_FAIL)

        End Function

        Public Function SampleCB(ByVal sampleTime As Double, ByVal pSample As IMediaSample) As Integer Implements ISampleGrabberCB.SampleCB
            Try
                If Not bProcess Then
                    Return WinAPI.E_FAIL
                End If

                ' internal stats
                _sampleIndex += 1
                Dim tStart As Long = Nothing, tEnd As Long = Nothing

                pSample.GetMediaTime(tStart, tEnd)
                Debug.Assert(tStart < tEnd)
                Debug.Assert(tStart > lastMediaTime)
                sampleProcessed += tEnd - tStart
                sampleDropped += tStart - lastMediaTime - 1
                lastMediaTime = tEnd - 1

                Dim dataLen As Integer = pSample.GetActualDataLength()
                Dim bufPtr As IntPtr = Nothing
                Dim hr As Integer = pSample.GetPointer(bufPtr)
                Debug.Assert(0 = hr)

                Dim processed As Boolean = ProcessSample(bufPtr, dataLen, sampleTime)

                Return If(processed, WinAPI.S_OK, WinAPI.E_FAIL)
            Catch ex As Exception
                System.Diagnostics.Debug.WriteLine(ex.ToString())
            Finally
                Marshal.ReleaseComObject(pSample)
            End Try

            Return WinAPI.E_FAIL
        End Function ' end of SampleCB

        Public Property MainWindow() As IntPtr
            Get
                Return _mainWindow
            End Get
            Set(ByVal value As IntPtr)
                _mainWindow = value
            End Set
        End Property

        Public ReadOnly Property SampleIndex() As Long
            Get
                Return _sampleIndex
            End Get
        End Property

        Public Property StreamNumber() As Integer
            Get
                Return _streamNumber
            End Get
            Set(ByVal value As Integer)
                _streamNumber = value
            End Set
        End Property

        Public ReadOnly Property ProcessedSamples() As Long
            Get
                Return sampleProcessed
            End Get
        End Property

        Public ReadOnly Property DroppedSamples() As Long
            Get
                Return sampleDropped
            End Get
        End Property

        Public Sub New(ByVal name As String)
            If Not String.IsNullOrEmpty(name) Then
                Me.name = name
            Else
                Me.name = "SampleGrabberCB"
            End If
        End Sub

        Protected Overrides Sub Finalize()
            Reset()
        End Sub

        Public Property MediaState() As MediaState
            Get
                Return _mediaState
            End Get
            Set(ByVal value As MediaState)
                _mediaState = value
            End Set
        End Property

        Public Sub Reset()
            _sampleIndex = 0
            sampleProcessed = 0
            sampleDropped = 0

            _mediaState = Nothing
            bProcess = True

            lastMediaTime = -1

            sampleBuffer = Nothing
        End Sub

	End Class ' end of SampleGrabberCB

	' Sample grabber callback method
	Friend Enum CBMethod
		Sample = 0 ' the original sample from the upstream filter
		Buffer = 1 ' a copy of the sample of the upstream filter
	End Enum
End Namespace
