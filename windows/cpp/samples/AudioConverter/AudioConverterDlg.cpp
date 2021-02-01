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
#include "AvbTranscoder.h"

#include <atlpath.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


CAudioConverterDlg::CAudioConverterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAudioConverterDlg::IDD, pParent)
	, m_strInputFile(_T(""))
	, m_strOutputFile(_T(""))
	, m_strStatus(_T(""))
{
	m_bWorking = false;
	m_bStop = false;

	m_transcoder.SetCallback(this);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAudioConverterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT_FILE, m_strInputFile);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_FILE, m_strOutputFile);
	DDX_Text(pDX, IDC_STATIC_STATUS, m_strStatus);
	DDX_Control(pDX, IDC_COMBO_PRESETS, m_cbPreset);
	DDX_Control(pDX, IDC_PROGRESS1, m_progressBar);
}

BEGIN_MESSAGE_MAP(CAudioConverterDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_INPUT_FILE, OnBnClickedButtonChooseInputFile)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_OUTPUT_FILE, OnBnClickedButtonChooseOutputFile)
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_MESSAGE(WM_CONVERT_THREAD_FINISHED, OnConvertThreadFinished)
	ON_MESSAGE(WM_CONVERT_PROGRESS, OnConvertProgress)
	ON_CBN_SELCHANGE(IDC_COMBO_PRESETS, &CAudioConverterDlg::OnCbnSelchangeComboPresets)
END_MESSAGE_MAP()


// CAudioConverterDlg message handlers

BOOL CAudioConverterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	for (int i=0; ;++i)
	{
		if ( !avb_presets[i].Id )
			break;

		if ( !avb_presets[i].AudioOnly )
			continue;

		CString str(avb_presets[i].Id);
		str.MakeUpper();

		m_cbPreset.InsertString(-1, str);
		m_cbPreset.SetItemData( m_cbPreset.GetCount() -1, i );
	}

	m_cbPreset.SetCurSel(0);

	UpdateControls();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAudioConverterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAudioConverterDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAudioConverterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAudioConverterDlg::OnBnClickedOk()
{
}

void CAudioConverterDlg::OnBnClickedCancel()
{
	if (m_bWorking)
		return;

	OnCancel();
}

void CAudioConverterDlg::OnBnClickedButtonChooseInputFile()
{
	UpdateData(TRUE);

	CFileDialog dlg(TRUE, NULL,
		m_strInputFile.IsEmpty() ? NULL : m_strInputFile,
		OFN_HIDEREADONLY,
		_TEXT("Audio files (*.wma,*.wav,*.aac,*.m4a,*.mp3,*.mp2,*.ogg,*.oga,*.ogm)|*.wma;*.wav;*.aac;*.m4a;*.mp3;*.mp2;*.ogg;*.oga;*.ogm|")
		_TEXT("Video files (*.mp4,*.mpg,*.mpeg,*.avi,*.wmv,*.mts,*.ts,*.m4v,*.webm,*.dat,*.mpe,*.mpeg4)|*.mp4;*.mpg;*.mpeg;*.avi;*.wmv;*.mts;*.ts;*.m4v;*.webm;*.dat;*.mpe;*.mpeg4|")
		_TEXT("All files (*.*)|*.*||"), 
		NULL);

	if(IDOK != dlg.DoModal())
		return;

	m_strInputFile = dlg.m_ofn.lpstrFile;

	UpdateData(FALSE);
}

void CAudioConverterDlg::OnBnClickedButtonChooseOutputFile()
{
	UpdateData(TRUE);

	const PresetDescriptor& preset = GetSelectedPreset();
	
	CString filter, defext;

	if (preset.FileExtension)
	{
		defext = preset.FileExtension;
		filter.Format (L"(*.%hs)|*.%hs|", preset.FileExtension, preset.FileExtension);
	}
	LPCWSTR pDefaultExtension = defext.IsEmpty() ? NULL : defext;

	filter += _TEXT("All files (*.*)|*.*||");

	CFileDialog dlg(FALSE, pDefaultExtension , 
					m_strOutputFile.IsEmpty() ? NULL : m_strOutputFile, 
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);

	if(IDOK != dlg.DoModal())
		return;

	m_strOutputFile = dlg.m_ofn.lpstrFile;

	UpdateData(FALSE);
}

void CAudioConverterDlg::OnBnClickedButtonStart()
{
	UpdateData(TRUE);

	if(m_bWorking)
		return;

	if(!ValidateInputData())
		return;

	m_OutputPreset = GetSelectedPreset().Id;
	m_hWnd = this->GetSafeHwnd();
	m_bStop = false;
	m_bWorking = true;

	UpdateControls();

	DeleteFile( m_strOutputFile );

	AfxBeginThread(ConvertThread, this);
}

void CAudioConverterDlg::OnBnClickedButtonStop()
{
	m_bStop = true;
	UpdateControls();
}

bool CAudioConverterDlg::ValidateInputData()
{
	if(m_strInputFile.GetLength() == 0)
	{
		AfxMessageBox(_T("Please enter input file."));
		return false;
	}

	if (m_strOutputFile.GetLength() == 0)
	{
		AfxMessageBox(_T("Please enter output file."));
		return false;
	}

	return true;
}

void CAudioConverterDlg::UpdateControls()
{
	UpdateData(TRUE);

	GetDlgItem(IDC_BUTTON_CHOOSE_INPUT_FILE)->EnableWindow(!m_bWorking);
	GetDlgItem(IDC_BUTTON_CHOOSE_OUTPUT_FILE)->EnableWindow(!m_bWorking);
	GetDlgItem(IDC_COMBO_PRESETS)->EnableWindow(!m_bWorking);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(!m_bWorking);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_bWorking && !m_bStop);

	if (m_bStop)
	{
		m_strStatus = _T("Stopping ...");
	}
	else
	{
		if (m_bWorking)
		{
			m_strStatus = _T("Working ...");
		}
		else
		{
			m_strStatus = _T("Ready");
		}
	}

	UpdateData(FALSE);
}

void CAudioConverterDlg::onProgress(double currentTime, double totalTime)
{
	if(totalTime > 0)
	{
		int progress = static_cast<int>(100 * currentTime / totalTime);
		if(progress < 0) 
			progress = 0;

		if(progress > 100) 
			progress = 100;

		::PostMessage(m_hWnd, WM_CONVERT_PROGRESS, progress, 0);
	}
}

void CAudioConverterDlg::onStatus(primo::avblocks::TranscoderStatus::Enum status)
{
	if (status == primo::avblocks::TranscoderStatus::Completed && !m_bStop)
	{
		::PostMessage(m_hWnd, WM_CONVERT_PROGRESS, 100, 0);
	}
}

bool_t	CAudioConverterDlg::onContinue(double currentTime)
{
	return !m_bStop;
}

LRESULT CAudioConverterDlg::OnConvertThreadFinished(WPARAM wParam, LPARAM lParam)
{
	m_strStatus = L"Finished";
	UpdateData(FALSE);

	if (m_success)
	{
		AfxMessageBox(_T("Conversion successful."), MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		AfxMessageBox(m_transcoder.GetErrorMessage(), MB_OK | MB_ICONHAND);
	}

	UpdateData(TRUE);
	m_progressBar.SetPos(0);

	m_bStop = false;
	m_bWorking = false;

	UpdateControls();
	UpdateData(FALSE);
	return 0;
}

LRESULT CAudioConverterDlg::OnConvertProgress(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	m_progressBar.SetPos((int)wParam); // wParam contains the progress expressed in percents.
	UpdateData(FALSE);
	return 0;
}

UINT CAudioConverterDlg::ConvertThread(LPVOID param)
{
	HRESULT hr = CoInitialize(NULL);

	CAudioConverterDlg *dlg = reinterpret_cast<CAudioConverterDlg*>(param);
	dlg->Convert();
	
	
	CoUninitialize();

	::PostMessage(dlg->m_hWnd, WM_CONVERT_THREAD_FINISHED, 0, 0);

	return 0;
}

void CAudioConverterDlg::Convert()
{
	CStringA utf8;
	
	m_transcoder.SetInputFile(m_strInputFile);

	m_transcoder.SetOutputFile(m_strOutputFile);

	m_transcoder.SetOutputPreset(m_OutputPreset.c_str());

	m_success = m_transcoder.Convert();

}

void CAudioConverterDlg::OnCbnSelchangeComboPresets()
{
	const PresetDescriptor& preset = GetSelectedPreset();

	if (!preset.FileExtension || m_strOutputFile.IsEmpty())
		return;

	CString newext ( preset.FileExtension );
	newext.Insert(0,L'.');

	CPath newfile (m_strOutputFile);
	CString oldext = newfile.GetExtension();

	if (oldext == newext)
		return;

	newfile.RenameExtension(newext);
	if (newfile.FileExists())
	{
		CString prompt;
		prompt.Format(L"%s already exists. Do you want to replace the file?", newfile.m_strPath);
		if (IDYES != AfxMessageBox(prompt, MB_YESNO | MB_ICONEXCLAMATION))
			return;
	}

	m_strOutputFile = newfile.m_strPath;
	UpdateData(FALSE);
}