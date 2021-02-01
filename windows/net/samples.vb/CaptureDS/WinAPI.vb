Imports System
Imports System.Collections.Generic
Imports System.Text
Imports System.Runtime.InteropServices

Namespace CaptureDS
	<Flags>
	Friend Enum ROTFlags
		RegistrationKeepsAlive = &H1
		AllowAnyClient = &H2
	End Enum

	Friend NotInheritable Class WinAPI

		Private Sub New()
		End Sub

		Friend Const WM_APP As Integer = &H8000
        Friend Const E_FAIL As Integer = CInt(&H80004005) ' -2147467259 == 0x80004005L
		Friend Const S_OK As Integer = 0

		 <DllImport("oleaut32.dll", CharSet:=CharSet.Unicode, ExactSpelling:=True, PreserveSig := True)>
		 Friend Shared Function OleCreatePropertyFrame(ByVal hwndOwner As IntPtr, ByVal x As Integer, ByVal y As Integer, ByVal lpszCaption As String, ByVal cObjects As Integer, <[In], MarshalAs(UnmanagedType.Interface)> ByRef ppUnk As Object, ByVal cPages As Integer, ByVal pPageClsID As IntPtr, ByVal lcid As Integer, ByVal dwReserved As Integer, ByVal pvReserved As IntPtr) As Integer
		 End Function

        <DllImport("user32.dll", SetLastError:=True)>
        Friend Shared Function PostMessage(ByVal hWnd As IntPtr, ByVal Msg As Integer, ByVal wParam As IntPtr, ByVal lParam As IntPtr) As <MarshalAs(UnmanagedType.Bool)> Boolean
        End Function
	End Class
End Namespace
