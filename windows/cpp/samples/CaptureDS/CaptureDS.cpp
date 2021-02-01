
// VideoRecorder.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "CaptureDS.h"
#include "CaptureDSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CVideoRecorderApp

BEGIN_MESSAGE_MAP(CVideoRecorderApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CVideoRecorderApp construction

CVideoRecorderApp::CVideoRecorderApp()
{
}


// The one and only CVideoRecorderApp object

CVideoRecorderApp theApp;


// CVideoRecorderApp initialization

BOOL CVideoRecorderApp::InitInstance()
{
	using namespace primo::avblocks;

	CoInitialize(NULL);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	Library::initialize();

	// Replace the values with your license for AVBlocks.
	Library::setLicense("YOUR AVBLOCKS LICENSE");

	// allow AMD MFT
	Library::config()->hardware()->setAmdMft(TRUE);

	CCaptureDSDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	
	Library::shutdown();

	CoUninitialize();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
