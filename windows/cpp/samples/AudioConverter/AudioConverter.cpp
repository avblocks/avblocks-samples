/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#include "stdafx.h"
#include "AudioConverter.h"
#include "AudioConverterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CAudioConverterApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


CAudioConverterApp::CAudioConverterApp()
{
	// add construction code here,
	// Place all significant initialization in InitInstance
}

CAudioConverterApp theApp;

BOOL CAudioConverterApp::InitInstance()
{

	primo::avblocks::Library::initialize();
	// primo::avblocks::Library::setLicense("<license-string>");

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CAudioConverterDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();

	primo::avblocks::Library::shutdown();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
