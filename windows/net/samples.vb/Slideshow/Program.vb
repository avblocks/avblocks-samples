'
' *  Copyright (c) 2013 Primo Software. All Rights Reserved.
' *
' *  Use of this source code is governed by a BSD-style license
' *  that can be found in the LICENSE file in the root of the source
' *  tree.  
'
Imports System
Imports System.Reflection
Imports System.IO
Imports PrimoSoftware.AVBlocks

Namespace SlideshowSample
	Friend Class Program
		Shared Function Main(ByVal args() As String) As Integer
			Dim opt = New Options()

			If Not opt.Prepare(args) Then
				Return If(opt.Error, CInt(ExitCodes.OptionsError), CInt(ExitCodes.Success))
			End If

			Library.Initialize()

			' Set license information. To run AVBlocks in demo mode, comment the next line out
			' Library.SetLicense("<license-string>");

			Dim encodeResult As Boolean = Encode(opt)

			Library.Shutdown()

			Return If(encodeResult, CInt(ExitCodes.Success), CInt(ExitCodes.EncodeError))
		End Function

		<Flags>
		Private Enum ExitCodes As Integer
			Success = 0
			OptionsError = 1
			EncodeError = 2
		End Enum

		Private Shared Function Encode(ByVal opt As Options) As Boolean
			Dim outFilename As String = "cube." & opt.FileExtension
			Const imageCount As Integer = 250
			Const inputFrameRate As Double = 25.0

			Using transcoder = New Transcoder()
				' In order to use the OEM release for testing (without a valid license),
				' the transcoder demo mode must be enabled.
				transcoder.AllowDemoMode = True

				Try
					Dim result As Boolean

					Try
						File.Delete(outFilename)
					Catch
					End Try

					' Configure Input
					If True Then
						Using medInfo As New MediaInfo()
							medInfo.InputFile = GetImagePath(0)

							result = medInfo.Load()
							PrintError("Load MediaInfo", medInfo.Error)
							If Not result Then
								Return False
							End If

							Dim vidInfo As VideoStreamInfo = CType(medInfo.Streams(0), VideoStreamInfo)
							vidInfo.FrameRate = inputFrameRate

							Dim pin As New MediaPin()
							pin.StreamInfo = vidInfo

							Dim socket As New MediaSocket()
							socket.Pins.Add(pin)

							transcoder.Inputs.Add(socket)
						End Using
					End If

					' Configure Output
					If True Then
						Dim socket As MediaSocket = MediaSocket.FromPreset(opt.PresetID)
						socket.File = outFilename

						transcoder.Outputs.Add(socket)
					End If

					' Encode Images
					result = transcoder.Open()
					PrintError("Open Transcoder", transcoder.Error)
					If Not result Then
						Return False
					End If

					For i As Integer = 0 To imageCount - 1
						Dim imagePath As String = GetImagePath(i)

						Dim mediaBuffer As New MediaBuffer(File.ReadAllBytes(imagePath))

						Dim mediaSample As New MediaSample()
						mediaSample.StartTime = i / inputFrameRate
						mediaSample.Buffer = mediaBuffer

						If Not transcoder.Push(0, mediaSample) Then
							PrintError("Push Transcoder", transcoder.Error)
							Return False
						End If
					Next i

					result = transcoder.Flush()
					PrintError("Flush Transcoder", transcoder.Error)
					If Not result Then
						Return False
					End If

					transcoder.Close()
					Console.WriteLine("Output video: ""{0}""", outFilename)
				Catch ex As Exception
					Console.WriteLine(ex.ToString())
					Return False
				End Try
			End Using

			Return True
		End Function

		Private Shared Function GetImagePath(ByVal imageNumber As Integer) As String
			Dim exeDir As String = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location)
			Return Path.Combine(exeDir, String.Format("..\assets\img\cube{0:0000}.jpeg", imageNumber))
		End Function

		Private Shared Sub PrintError(ByVal action As String, ByVal e As ErrorInfo)
			If action IsNot Nothing Then
				Console.Write("{0}: ", action)
			End If

			If ErrorFacility.Success = e.Facility Then
				Console.WriteLine("Success")
				Return
			Else
				Console.WriteLine("{0}, facility:{1} code:{2}", If(e.Message, ""), e.Facility, e.Code)
			End If
		End Sub
	End Class
End Namespace
