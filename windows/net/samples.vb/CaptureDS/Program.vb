Imports System
Imports System.Collections.Generic
Imports System.Windows.Forms

Namespace CaptureDS
	Friend NotInheritable Class Program

		Private Sub New()
		End Sub

		''' <summary>
		''' The main entry point for the application.
		''' </summary>
		<STAThread>
		Shared Sub Main()
			' Call EnableVisualStyles before initializing PrimoSoftware libraries
			Application.EnableVisualStyles()
			Application.SetCompatibleTextRenderingDefault(False)

			PrimoSoftware.AVBlocks.Library.Initialize()

			' Replace the values with your name, company and license key for AVBlocks.NET
			PrimoSoftware.AVBlocks.Library.SetLicense("PRIMOSOFTWARE-LICENSE")

			Application.Run(New CaptureDSForm())

			PrimoSoftware.AVBlocks.Library.Shutdown()
		End Sub
	End Class
End Namespace
