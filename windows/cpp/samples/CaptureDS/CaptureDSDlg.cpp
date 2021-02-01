
// VideoRecorderDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CaptureDS.h"
#include "CaptureDSDlg.h"
#include "VideoCapturePropertiesDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCaptureDSDlg dialog

CCaptureDSDlg::CCaptureDSDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCaptureDSDlg::IDD, pParent),
m_updateStatsEvent(1)
, m_strOutputFile(_T(""))
, m_videoCB(L"VideoGrabberCB")
, m_audioCB(L"AudioGrabberCB")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bRecording = false;
	m_bCmdRecordBusy = false;
}

void CCaptureDSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_AUDIO_DEV, m_listAudioDev);
	DDX_Control(pDX, IDC_CMD_AUDIO_DEV_PROP, m_cmdAudioDevProp);
	DDX_Control(pDX, IDC_LIST_VIDEO_DEV, m_listVideoDev);
	DDX_Control(pDX, IDC_CMD_VIDEO_CAPTURE_PROP, m_cmdVideoCaptureProp);
	DDX_Control(pDX, IDC_CMD_RECORD, m_cmdRecord);
	DDX_Control(pDX, IDC_TXT_RECORDING, m_txtRecording);
	DDX_Control(pDX, IDC_PREVIEW, m_preview);
	DDX_Control(pDX, IDC_COMBO_PRESETS, m_cbPreset);
	DDX_Text(pDX, IDC_EDIT_OUTPUT_FILE, m_strOutputFile);
}

BEGIN_MESSAGE_MAP(CCaptureDSDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SYSCOMMAND()
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_LIST_AUDIO_DEV, &CCaptureDSDlg::OnCbnSelchangeListAudioDev)
	ON_CBN_SELCHANGE(IDC_LIST_VIDEO_DEV, &CCaptureDSDlg::OnCbnSelchangeListVideoDev)
	ON_BN_CLICKED(IDC_CMD_AUDIO_DEV_PROP, &CCaptureDSDlg::OnCmdAudioDevProp)
	ON_BN_CLICKED(IDC_CMD_VIDEO_DEV_PROP, &CCaptureDSDlg::OnCmdVideoDevProp)
	ON_BN_CLICKED(IDC_CMD_VIDEO_CAPTURE_PROP, &CCaptureDSDlg::OnCmdVideoCaptureProp)
	ON_BN_CLICKED(IDC_CMD_RECORD, &CCaptureDSDlg::OnCmdRecord)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_OUTPUT_FILE, &CCaptureDSDlg::OnBnClickedButtonChooseOutputFile)
	ON_CBN_SELCHANGE(IDC_COMBO_PRESETS, &CCaptureDSDlg::OnCbnSelchangeComboPresets)
END_MESSAGE_MAP()


// CCaptureDSDlg message handlers

BOOL CCaptureDSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

#ifdef WIN64
	CString title;
	GetWindowText(title);
	title += L" (64-bit)";
	SetWindowText(title);
#endif

	EnumInputDev(CLSID_AudioInputDeviceCategory, m_listAudioDev, m_audioDevices);
	// input device with index 0 designates disabled input
	m_listAudioDev.InsertString(0, L"[No audio input]");
	m_audioDevices.push_front(L""); 

	EnumInputDev(CLSID_VideoInputDeviceCategory, m_listVideoDev, m_videoDevices);

	m_listAudioDev.SetCurSel(m_audioDevices.size() > 1 ? 1 : 0);

	if (m_videoDevices.size() > 0)
		m_listVideoDev.SetCurSel(0);

	m_cmdRecord.SetWindowTextW(L"Start Recording");
	GetDlgItem(IDC_TXT_RECORDING)->ShowWindow(SW_HIDE);
	ResetStats();

	int audioItem = m_listAudioDev.GetCurSel();
	int videoItem = m_listVideoDev.GetCurSel();

	int hr = InitInputDev(m_ms, videoItem, audioItem);
	if (FAILED(hr))
	{
		MessageBox(_T("Cannot use the selected capture devices"));
		return TRUE;
	}

	BuildGraph();

	AddGraphToRot(m_ms.pGraph, &m_ms.dwROT);

	RECT r;
	m_preview.GetWindowRect(&r);
	LONG width = r.right - r.left + 1;
	LONG height = 3 * width / 4;
	m_preview.SetWindowPos(0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);

	for (int i = 0;; ++i)
	{
		if (avb_presets[i].Id == NULL)
			break;

		if (avb_presets[i].AudioOnly)
			continue;

		CString str(avb_presets[i].Id);
		if (avb_presets[i].FileExtension)
		{
			CStringA ext;
			ext.Format(" (.%s)", avb_presets[i].FileExtension);
			str += ext;
		}
		m_cbPreset.InsertString(-1, str);
		m_cbPreset.SetItemData(m_cbPreset.GetCount() - 1, i);
	}

	m_cbPreset.SetCurSel(0);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCaptureDSDlg::OnPaint()
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

// The system calls this function to obtain the cursor to display while the user drags the minimized window.
HCURSOR CCaptureDSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CCaptureDSDlg::OnOK()
{}

void CCaptureDSDlg::OnCancel()
{}

void CCaptureDSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch(nID) {
	  case SC_CLOSE:
		  CloseApp();
		  break;

	  default:
		  CDialog::OnSysCommand(nID, lParam);
	}
}

HBRUSH CCaptureDSDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// Change any attributes of the DC here
	if (pWnd->GetDlgCtrlID() == IDC_TXT_RECORDING)
	{
		pDC->SetTextColor(RGB(255, 0, 0));
	}

	return hbr;
}

void CCaptureDSDlg::EnumInputDev(const IID & devClass, CComboBox& list, StringList& devices)
{
	if (devClass != CLSID_AudioInputDeviceCategory &&
		devClass != CLSID_VideoInputDeviceCategory)
	{
		return;
	}

	struct Res
	{
		ICreateDevEnum *pSysDevEnum;
		IEnumMoniker *pEnumCat;
		IMoniker *pMoniker;
		IPropertyBag *pPropBag;

		Res() : pSysDevEnum(0), pEnumCat(0), pMoniker(0), pPropBag(0)
		{}

		~Res()
		{
			if (pPropBag) pPropBag->Release();
			if (pMoniker) pMoniker->Release();
			if (pEnumCat) pEnumCat->Release();
			if (pSysDevEnum) pSysDevEnum->Release();
		}
	} res;



	HRESULT hr = S_OK;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
		IID_ICreateDevEnum, (void **)&res.pSysDevEnum);
	if (FAILED(hr))
	{
		TRACE(_T("CoCreateInstance CLSID_SystemDeviceEnum failed"));
		return;
	}

	// Obtain a class enumerator for the capture category.
	hr = res.pSysDevEnum->CreateClassEnumerator(devClass, &res.pEnumCat, 0);

	if (FAILED(hr))
	{
		TRACE(_T("CreateClassEnumerator failed"));
		return;
	}

	if (!res.pEnumCat)
	{
		if (CLSID_AudioInputDeviceCategory == devClass)
			TRACE(_T("No audio input devices found"));
		else if (CLSID_VideoInputDeviceCategory == devClass)
			TRACE(_T("No video input devices found"));

		return;
	}

	// Enumerate the monikers.
	ULONG cFetched;
	while(res.pEnumCat->Next(1, &res.pMoniker, &cFetched) == S_OK)
	{
		hr = res.pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
			(void **)&res.pPropBag);
		if (SUCCEEDED(hr))
		{
			std::wstring friendlyName;
			std::wstring devName;

			// retrieve the filter's friendly name
			VARIANT varName;
			VariantInit(&varName);
			hr = res.pPropBag->Read(L"FriendlyName", &varName, 0);
			if (SUCCEEDED(hr))
			{
				friendlyName = (wchar_t*)varName.bstrVal;
				ATLTRACE(_T("friendly name: %s\n"),varName.bstrVal);
			}
			VariantClear(&varName);

			LPOLESTR pDisplayName;
			hr = res.pMoniker->GetDisplayName(NULL,NULL,(LPOLESTR*)&pDisplayName);
			devName = pDisplayName;
			ATLTRACE(_T("display name: %s\n"),pDisplayName);
			CoTaskMemFree(pDisplayName);

			// try to create an instance of the filter
			IBaseFilter *pFilter;
			hr = res.pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**)&pFilter);
			CLSID filterCLSID;

			if (SUCCEEDED(hr))
			{
				hr = pFilter->GetClassID(&filterCLSID);
				if (SUCCEEDED(hr))
				{
					list.AddString(friendlyName.c_str());
					devices.push_back(devName);
				}
				pFilter->Release();
			}

			res.pPropBag->Release();
			res.pPropBag = 0;
		}

		res.pMoniker->Release();
		res.pMoniker = 0;

	} // while
} // EnumInputDev

void CCaptureDSDlg::ResetStats()
{
	const wchar_t* pEmpty = L"--";
	SetDlgItemText(IDC_TXT_REC_TIME, L"0:00:00");

	SetDlgItemText(IDC_TXT_NUM_DROPPED, pEmpty);
	SetDlgItemText(IDC_TXT_NUM_PROCESSED, pEmpty);
	SetDlgItemText(IDC_TXT_AVERAGE_FPS, pEmpty);
	SetDlgItemText(IDC_TXT_CURRENT_FPS, pEmpty);

	SetDlgItemText(IDC_TXT_ACB, pEmpty);
	SetDlgItemText(IDC_TXT_ADROP, pEmpty);
	SetDlgItemText(IDC_TXT_APROC, pEmpty);

	SetDlgItemText(IDC_TXT_VCB, pEmpty);
	SetDlgItemText(IDC_TXT_VDROP, pEmpty);
	SetDlgItemText(IDC_TXT_VPROC, pEmpty);
}


void CCaptureDSDlg::UpdateStats()
{
	const int NUMSIZE = 32;
	WCHAR number[NUMSIZE];
	DWORD now = GetTickCount();
	CTimeSpan rec((now - recStartTime)/1000);
	WCHAR recTime[10];
	swprintf_s(recTime,10,L"%.2d:%.2d:%.2d",rec.GetHours(),rec.GetMinutes(),rec.GetSeconds());
	SetDlgItemText(IDC_TXT_REC_TIME, recTime);

	if (m_ms.pDroppedFrames)
	{
		HRESULT hr = S_OK;
		long dropped;
		hr = m_ms.pDroppedFrames->GetNumDropped(&dropped);
		if (S_OK == hr)
		{
			swprintf_s(number,NUMSIZE,L"%d",dropped);
			SetDlgItemText(IDC_TXT_NUM_DROPPED, number);
		}

		long notDropped;
		hr = m_ms.pDroppedFrames->GetNumNotDropped(&notDropped);
		if (S_OK== hr)
		{
			swprintf_s(number,NUMSIZE,L"%d",notDropped);
			SetDlgItemText(IDC_TXT_NUM_PROCESSED, number);
			if (notDropped >= 0)
			{
				double averageFPS = (double)notDropped / rec.GetTotalSeconds();
				swprintf_s(number,NUMSIZE,L"%.3f",averageFPS);
				SetDlgItemText(IDC_TXT_AVERAGE_FPS, number);

				double fpsElapsed = ((double)(now - fpsStartTime)) / 1000;
				if (fpsElapsed > 5.0)
				{
					double curFPS = ((double)(notDropped-fpsProcessed))/ fpsElapsed;
					swprintf_s(number,NUMSIZE,L"%.3f",curFPS);
					SetDlgItemText(IDC_TXT_CURRENT_FPS, number);

					fpsStartTime = now;
					fpsProcessed = notDropped;
				}
			}
		}
	}

	swprintf_s(number,NUMSIZE,L"%d", (int)m_audioCB.SampleIndex);
	SetDlgItemText(IDC_TXT_ACB, number);

	swprintf_s(number,NUMSIZE,L"%d", (int)m_audioCB.SampleProcessed);
	SetDlgItemText(IDC_TXT_APROC, number);

	swprintf_s(number,NUMSIZE,L"%d", (int)m_audioCB.SampleDropped);
	SetDlgItemText(IDC_TXT_ADROP, number);

	swprintf_s(number,NUMSIZE,L"%d", (int)m_videoCB.SampleIndex);
	SetDlgItemText(IDC_TXT_VCB, number);

	swprintf_s(number,NUMSIZE,L"%d", (int)m_videoCB.SampleProcessed);
	SetDlgItemText(IDC_TXT_VPROC, number);

	swprintf_s(number,NUMSIZE,L"%d", (int)m_videoCB.SampleDropped);
	SetDlgItemText(IDC_TXT_VDROP, number);
}

HRESULT CCaptureDSDlg::InitInputDev(MediaState& ms, int videoItem, int audioItem)
{
	HRESULT hr = S_OK;
	// Create Filter Graph Manager

	ClearGraph();

	if (!ms.pGraph)
	{
		hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IFilterGraph2,
			(void **)&ms.pGraph);

		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot create FilterGraph"));
			return hr;
		}

		// Create the Capture Graph Builder.
		hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, 
			(void **)&ms.pCaptureGraph);

		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot create CaptureGraphBuilder2"));
			return hr;
		}

		hr = ms.pCaptureGraph->SetFiltergraph(ms.pGraph);
		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot set Filtergraph"));
			return hr;
		}
	}

	if  (audioItem >= 0)
	{
		// remove the old audio input
		if (ms.pAudioInput)
		{
			hr = ms.pGraph->RemoveFilter(ms.pAudioInput);
			SafeRelease(ms.pAudioInput);
			if (FAILED(hr))
			{
				ATLTRACE(_T("Cannot remove audio input filter"));
				return hr;
			}
		}

		// the first audio input device is [No audio input]
		if (audioItem >= 1)
		{
			// create audio input
			hr = CoGetObject(m_audioDevices[audioItem].c_str(), NULL, IID_IBaseFilter, (void**)&ms.pAudioInput);
			if (FAILED(hr))
			{
				ATLTRACE(_T("CoGetObject AudioInput failed"));
				return hr;
			}

			// add audio input to the graph
			TCHAR friendlyName[256];
			m_listAudioDev.GetLBText(audioItem, friendlyName);
			hr = ms.pGraph->AddFilter(ms.pAudioInput, friendlyName);
			if (FAILED(hr))
			{
				ATLTRACE(_T("Cannot add audio input filter"));
				return hr;
			}
		}
	}

	if (videoItem >= 0)
	{
		// remove the old video input
		if (ms.pVideoInput)
		{
			hr = ms.pGraph->RemoveFilter(ms.pVideoInput);
			SafeRelease(ms.pVideoInput);
			if (FAILED(hr))
			{
				ATLTRACE(_T("Cannot remove video input filter"));
				return hr;
			}
		}

		// create video input
		hr = CoGetObject(m_videoDevices[videoItem].c_str(),NULL, IID_IBaseFilter, (void**)&ms.pVideoInput);
		if (FAILED(hr))
		{
			ATLTRACE(_T("CoGetObject AudioInput failed"));
			return hr;
		}

		// add video input to the graph
		TCHAR friendlyName[256];
		m_listVideoDev.GetLBText(videoItem, friendlyName);
		hr = ms.pGraph->AddFilter(ms.pVideoInput, friendlyName);
		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot add video input filter"));
			return hr;
		}
	}

	return hr;
}

void CCaptureDSDlg::OnCbnSelchangeListAudioDev()
{
	int sel = m_listAudioDev.GetCurSel();
	if (sel < 0)
		return;

	ClearGraph();

	HRESULT hr = InitInputDev(m_ms, -1, sel);
	if (FAILED(hr))
	{
		MessageBox(_T("Cannot use the selected input devices"));
		return;
	}

	BuildGraph();
}

void CCaptureDSDlg::OnCbnSelchangeListVideoDev()
{
	int sel = m_listVideoDev.GetCurSel();
	if (sel < 0)
		return;

	ClearGraph();

	HRESULT hr = InitInputDev(m_ms, sel, -1);
	if (FAILED(hr))
	{
		MessageBox(_T("Cannot use the selected input devices"));
		return;
	}

	BuildGraph();
}

void CCaptureDSDlg::OnCmdAudioDevProp()
{
	if (!CheckInputDevice(m_ms.pAudioInput))
		return;

	ClearGraph();

	ShowPropPages(m_ms.pAudioInput);

	BuildGraph();
}

void CCaptureDSDlg::OnCmdVideoDevProp()
{
	if (!CheckInputDevice(m_ms.pVideoInput))
		return;

	ClearGraph();

	ShowPropPages(m_ms.pVideoInput);

	BuildGraph();
}

void CCaptureDSDlg::OnCmdVideoCaptureProp()
{
	if (!CheckInputDevice(m_ms.pVideoInput))
		return;

	ClearGraph();

	IAMStreamConfig *pStreamConfig;

	HRESULT hr = m_ms.pCaptureGraph->FindInterface(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video,
		m_ms.pVideoInput, IID_IAMStreamConfig, (void **)&pStreamConfig);

	if (FAILED(hr))
	{
		TRACE(_T("FindInterface IID_IAMStreamConfig failed"));
		return;
	}

	VideoCapturePropertiesDialog dlg;

	dlg.set_StreamConfig(pStreamConfig);

	dlg.DoModal();

	pStreamConfig->Release();

	BuildGraph();
}


bool CCaptureDSDlg::CheckInputDevice(IBaseFilter* pInputDevice)
{
	if (!pInputDevice)
	{
		MessageBox(_T("No input device is selected!"));
		return false;
	}

	return true;
}

void CCaptureDSDlg::ShowPropPages(IUnknown* pUnk)
{
	ISpecifyPropertyPages* pSpecPropPages = NULL;

	HRESULT hr = pUnk->QueryInterface<ISpecifyPropertyPages>(&pSpecPropPages);
	if (FAILED(hr))
	{
		MessageBox(_T("Property pages not available"));
		return;
	}

	CAUUID pages;
	hr = pSpecPropPages->GetPages(&pages);

	if (S_OK == hr && pages.cElems > 0)
	{
		// show property pages
		hr = OleCreatePropertyFrame(m_hWnd,
			30, 30, NULL, 1,
			&pUnk, pages.cElems,
			pages.pElems, 0, 0, NULL);

		CoTaskMemFree(pages.pElems);

	}

	pSpecPropPages->Release();

}

void CCaptureDSDlg::OnCmdRecord()
{
	if (m_bCmdRecordBusy)
		return;

	m_bCmdRecordBusy = true;
	m_cmdRecord.EnableWindow(0);

	if (m_bRecording)
	{
		// Stop Recording
		StopRecording();

		m_txtRecording.ShowWindow(SW_HIDE);
		m_cmdRecord.SetWindowTextW(L"Start Recording");

		EnableCommandUI(1);

		m_bRecording = false;
	}
	else
	{
		// Start Recording
		EnableCommandUI(0);
		if (StartRecording())
		{
			m_txtRecording.ShowWindow(SW_SHOW);
			m_cmdRecord.SetWindowTextW(L"Stop Recording");
			m_bRecording = true;
		}
		else
		{
			EnableCommandUI(1);
		}
	}

	m_cmdRecord.EnableWindow(1);
	m_bCmdRecordBusy = false;
}

void CCaptureDSDlg::ClearGraph()
{
	// leave the input devices in the graph
	m_ms.Reset(false);
}

bool CCaptureDSDlg::BuildGraph()
{
	m_cmdRecord.EnableWindow(0);

	struct CLEANUP
	{
		bool invoke;
		CLEANUP(bool b) :
			ms(0), invoke(b), audioConfig(0), pMediaType(0), videoWindow(0)
		{}

		MediaState* ms;
		IAMStreamConfig* audioConfig;
		AM_MEDIA_TYPE* pMediaType;
		IVideoWindow* videoWindow;

		~CLEANUP()
		{
			if (invoke)
			{
				if (ms) ms->Reset(false);
			}

			if (audioConfig) audioConfig->Release();
			if (videoWindow) videoWindow->Release();
			if (pMediaType) DeleteMediaType(pMediaType);
		}

	} cleanup(true);


	cleanup.ms = &m_ms;

	HRESULT hr = S_OK;

	if (m_ms.pAudioInput)
	{
		// Create the Audio Sample Grabber
		hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (void**)&m_ms.pAudioGrabberFilter);

		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot create SampleGrabber"));
			return false;
		}

		hr = m_ms.pGraph->AddFilter(m_ms.pAudioGrabberFilter, L"Audio SampleGrabber");
		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot Add Filter"));
			return false;
		}

		// get the ISampleGrabber interface.
		hr = m_ms.pAudioGrabberFilter->QueryInterface<ISampleGrabber>(&m_ms.pAudioGrabber);
		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot obtain ISampleGrabber"));
			return false;
		}

		// Create and add the audio null renderer in the graph
		hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (void**)&m_ms.pAudioNullRenderer);
		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot create NullRenderer"));
			return false;
		}

		hr = m_ms.pGraph->AddFilter(m_ms.pAudioNullRenderer, L"Audio Null Renderer");
		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot add NullRenderer"));
			return false;
		}

		// manually connect audio the filters
		hr = ConnectFilters(m_ms.pGraph, m_ms.pAudioInput, m_ms.pAudioGrabberFilter);
		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot connect filters"));
			return false;
		}

		hr = ConnectFilters(m_ms.pGraph, m_ms.pAudioGrabberFilter, m_ms.pAudioNullRenderer);
		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot connect filters"));
			return false;
		}
	}

	// Create the Video Sample Grabber
	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void**)&m_ms.pVideoGrabberFilter);

	if (FAILED(hr))
	{
		ATLTRACE(_T("Cannot create SampleGrabber"));
		return false;
	}

	hr = m_ms.pGraph->AddFilter(m_ms.pVideoGrabberFilter, L"Video SampleGrabber");
	if (FAILED(hr))
	{
		ATLTRACE(_T("Cannot Add Filter"));
		return false;
	}

	// get the ISampleGrabber interface.
	hr = m_ms.pVideoGrabberFilter->QueryInterface<ISampleGrabber>(&m_ms.pVideoGrabber);
	if (FAILED(hr))
	{
		ATLTRACE(_T("Cannot obtain ISampleGrabber"));
		return false;
	}

	// Create and add the video null renderer in the graph
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void**)&m_ms.pVideoNullRenderer);
	if (FAILED(hr))
	{
		ATLTRACE(_T("Cannot create NullRenderer"));
		return false;
	}

	hr = m_ms.pGraph->AddFilter(m_ms.pVideoNullRenderer, L"Video Null Renderer");
	if (FAILED(hr))
	{
		ATLTRACE(_T("Cannot add NullRenderer"));
		return false;
	}

	// add the smart tee if preview is required
	if (!m_ms.pSmartTee)
	{
		hr = CoCreateInstance(CLSID_SmartTee, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (void**)&m_ms.pSmartTee);

		hr = m_ms.pGraph->AddFilter(m_ms.pSmartTee, L"Smart Tee");

		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot add Smart Tee"));
			return false;
		}
	}

	if (m_ms.pSmartTee)
	{
		// connect the video input to the smart tee
		ConnectFilters(m_ms.pGraph, m_ms.pVideoInput, m_ms.pSmartTee);

		// connect smart tee capture to video grabber
		IPin* pCapturePin = NULL;
		hr = GetPin(m_ms.pSmartTee, PINDIR_OUTPUT, L"Capture", &pCapturePin);
		if (FAILED(hr))
		{
			return false;
		}

		IPin* pVideoGrabberPin = NULL;
		hr = GetUnconnectedPin(m_ms.pVideoGrabberFilter, PINDIR_INPUT, &pVideoGrabberPin);
		if (FAILED(hr))
		{
			return false;
		}

		hr = m_ms.pGraph->ConnectDirect(pCapturePin, pVideoGrabberPin, NULL);
		if (FAILED(hr))
		{
			return false;
		}

		// connect smart tee preview to video renderer
		hr = CoCreateInstance(CLSID_VideoRendererDefault, NULL, CLSCTX_INPROC_SERVER,
			IID_IBaseFilter, (void**)&m_ms.pPreviewRenderer);
		if (FAILED(hr))
		{
			return false;
		}

		hr = m_ms.pGraph->AddFilter(m_ms.pPreviewRenderer, L"Preview Renderer");
		if (FAILED(hr))
		{
			return false;
		}

		IPin* pPreviewPin;
		hr = GetPin(m_ms.pSmartTee, PINDIR_OUTPUT, L"Preview", &pPreviewPin);
		if (FAILED(hr))
		{
			return false;
		}

		IPin* pVideoRendererPin;
		hr = GetUnconnectedPin(m_ms.pPreviewRenderer, PINDIR_INPUT, &pVideoRendererPin);
		if (FAILED(hr))
		{
			return false;
		}

		hr = m_ms.pGraph->Connect(pPreviewPin, pVideoRendererPin);
		if (FAILED(hr))
		{
			return false;
		}
	}
	else
	{
		hr = ConnectFilters(m_ms.pGraph, m_ms.pVideoInput, m_ms.pVideoGrabberFilter);
		if (FAILED(hr))
		{
			return false;
		}
	}

	hr = ConnectFilters(m_ms.pGraph, m_ms.pVideoGrabberFilter, m_ms.pVideoNullRenderer);
	if (FAILED(hr))
	{
		return false;
	}

	hr = m_ms.pGraph->QueryInterface<IMediaControl>(&m_ms.pMediaControl);
	if (FAILED(hr))
	{
		ATLTRACE(_T("Cannot obtain IMediaControl"));
		return false;
	}

	if (m_ms.pAudioInput)
	{
		IAMStreamConfig* pAudioConfig;
		hr = m_ms.pCaptureGraph->FindInterface(&PIN_CATEGORY_CAPTURE, NULL, m_ms.pAudioInput, IID_IAMStreamConfig, (void**)&pAudioConfig);
		if (FAILED(hr))
		{
			ATLTRACE(_T("Cannot obtain IAMStreamConfig"));
			return false;
		}
		cleanup.audioConfig = pAudioConfig;

		AM_MEDIA_TYPE* pAudioType = NULL;
		pAudioConfig->GetFormat(&pAudioType);
		if (FAILED(hr))
		{
			return false;
		}
		cleanup.pMediaType = pAudioType;

		// set audio capture parameters
		WAVEFORMATEX *pWFX = (WAVEFORMATEX*)pAudioType->pbFormat;
		pWFX->nSamplesPerSec = 48000;
		pWFX->nChannels = 2;
		pWFX->wBitsPerSample = 16;
		pWFX->nBlockAlign = pWFX->nChannels * pWFX->wBitsPerSample / 8;
		pWFX->nAvgBytesPerSec = pWFX->nSamplesPerSec * pWFX->nBlockAlign;
		pWFX->wFormatTag = 1; // PCM
		hr = pAudioConfig->SetFormat(pAudioType);
		if (FAILED(hr))
			return false;

		// Store the audio media type for later use.
		hr = m_ms.pAudioGrabber->GetConnectedMediaType(&m_ms.AudioType);
		if (FAILED(hr))
			return false;
	}
	else
	{
		// There's no audio input type because the audio input is disabled
		m_ms.AudioType.majortype = MEDIATYPE_NULL;
	}

	// Store the video media type for later use.
	hr = m_ms.pVideoGrabber->GetConnectedMediaType(&m_ms.VideoType);
	if (FAILED(hr))
		return false;

	hr = m_ms.pVideoInput->QueryInterface<IAMDroppedFrames>(&m_ms.pDroppedFrames);
	//the video capture device may not support IAMDroppedFrames

	IVideoWindow* pVideoWindow = NULL;
	m_ms.pGraph->QueryInterface<IVideoWindow>(&pVideoWindow);
	if (pVideoWindow)
	{
		cleanup.videoWindow = pVideoWindow;

		pVideoWindow->put_Owner((OAHWND)m_preview.m_hWnd);
		pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		pVideoWindow->put_Visible(OATRUE);

		RECT r;
		m_preview.GetClientRect(&r);
		pVideoWindow->SetWindowPosition(0, 0, r.right, r.bottom);
	}

	hr = m_ms.pMediaControl->Run();
	if (FAILED(hr))
		return false;

	cleanup.invoke = false;

	m_cmdRecord.EnableWindow(1);

	return true;
}

bool CCaptureDSDlg::StartRecording()
{
	HRESULT hr = S_OK;

	if (!m_ms.pVideoInput)
	{
		MessageBox(_T("No video input!"));
		return false;
	}

	if (m_strOutputFile.GetLength() == 0)
	{
		AfxMessageBox(_T("Please choose output file."));
		return false;
	}

	{
		CPath newfile(m_strOutputFile);
		if (newfile.FileExists())
		{
			CString prompt;
			prompt.Format(L"%s already exists. Do you want to replace the file?", newfile.m_strPath);
			if (IDYES != AfxMessageBox(prompt, MB_YESNO | MB_ICONEXCLAMATION))
				return false;

			DeleteFile(m_strOutputFile);
		}
	}

	hr = m_ms.pMediaControl->Stop();
	if (FAILED(hr))
	{
		return false;
	}
	
	if (m_ms.pAudioInput)
	{
		hr = m_ms.pAudioGrabber->SetCallback(&m_audioCB, CBMethodSample);
		if (FAILED(hr))
		{
			return false;
		}
	}

	hr = m_ms.pVideoGrabber->SetCallback(&m_videoCB, CBMethodSample);
	if (FAILED(hr))
	{
		return false;
	}

	m_audioCB.Reset();
	m_videoCB.Reset();

	// pass the media state to the callbacks
	m_audioCB.pMediaState = &m_ms;
	m_videoCB.pMediaState = &m_ms;
	m_audioCB.MainWindow = m_hWnd;
	m_videoCB.MainWindow = m_hWnd;

	if (!ConfigureTranscoder())
		return false;

	hr = m_ms.pMediaControl->Pause();
	if (FAILED(hr))
	{
		return false;
	}

	Sleep(300);
	hr = m_ms.pMediaControl->Run();
	if (FAILED(hr))
	{
		return false;
	}

	ResetStats();

	recStartTime = GetTickCount();
	fpsStartTime = recStartTime;
	fpsProcessed = 0;
	SetTimer(m_updateStatsEvent, 500, NULL);


	return true;
}


void SetH264FastEncoding(primo::avblocks::ParameterList* params)
{
	using namespace primo::codecs;
	using namespace primo::avblocks::Param::Encoder::Video;
	
	params->addInt(H264::Profile, H264Profile::Baseline);
	params->addInt(H264::EntropyCodingMode, H264EntropyCodingMode::CAVLC);
	params->addInt(H264::NumBFrames, 0);
	params->addInt(H264::NumRefFrames, 1);
	params->addInt(H264::Transform8x8, false);
	params->addInt(H264::KeyFrameInterval, 15);
	params->addInt(H264::KeyFrameIDRInterval, 1);
	params->addInt(H264::QualitySpeed, 0); // max speed
	params->addInt(H264::RateControlMethod, H264RateControlMethod::ConstantQuant);
	params->addInt(H264::RateControlQuantI, 26); // 0-51, default 20
	params->addInt(H264::RateControlQuantP, 26); // 0-51, default 20
	params->addInt(H264::DeblockingFilter, H264DeblockingFilter::InSlice);
	params->addInt(H264::MESplitMode, H264MeSplitMode::Only16x16);
	params->addInt(H264::MEMethod, H264MeMethod::UMH);
}

primo::avblocks::MediaSocket* CustomOutputSocket(int frameWidth, int frameHeight, double framerate)
{
	using namespace primo::avblocks;
	using namespace primo::codecs;

	MediaSocket* socket(Library::createMediaSocket());

	// video pin
	{
		primo::ref<MediaPin> pin(Library::createMediaPin());
		socket->pins()->add(pin.get());

		primo::ref<VideoStreamInfo> info(Library::createVideoStreamInfo());
		info->setStreamType(StreamType::H264);
		info->setFrameWidth(frameWidth);
		info->setFrameHeight(frameHeight);
		info->setFrameRate(framerate);
		info->setDisplayRatioWidth(frameWidth);
		info->setDisplayRatioHeight(frameHeight);
		pin->setStreamInfo(info.get());

		// set H264 encoder params on the output pin to get the best encoding speed
		SetH264FastEncoding(pin->params());
		// the video capture cannot guarantee a fixed input frame rate; better disable it in the encoded stream
		pin->params()->addInt(Param::Encoder::Video::H264::FixedFramerate, 0);
	}

	// audio pin
	{
		primo::ref<MediaPin> pin(Library::createMediaPin());
		socket->pins()->add(pin.get());

		primo::ref<AudioStreamInfo> info(Library::createAudioStreamInfo());
		info->setStreamType(StreamType::AAC);
		info->setSampleRate(44100);
		info->setChannels(2);
		pin->setStreamInfo(info.get());
	}

	return socket;
}


static void AddOrReplaceIntParam(primo::avblocks::ParameterList* params, const char* name, int64_t value)
{
	primo::avblocks::Parameter * param = params->itemByName(name);
	if ((NULL != param) && (param->type() == primo::avblocks::ParamType::Int))
	{
		primo::avblocks::IntParameter *intparam = dynamic_cast<primo::avblocks::IntParameter*>(param);
		intparam->setValue(value);
	}
	else
	{
		params->addInt(name, value);
	}
}

static void SetRealTimeVideoMode(primo::avblocks::MediaSocket* socket)
{
	using namespace primo::avblocks;
	using namespace primo::codecs;

	for (int i = 0; i < socket->pins()->count(); i++)
	{
		MediaPin *pin = socket->pins()->at(i);

		if (pin->streamInfo()->mediaType() == MediaType::Video)
		{
			AddOrReplaceIntParam(pin->params(), primo::avblocks::Param::Video::FrameRateConverter::Use, Use::On);
			AddOrReplaceIntParam(pin->params(), primo::avblocks::Param::Video::FrameRateConverter::RealTime, TRUE);

			if (pin->streamInfo()->streamType() == StreamType::H264)
			{
				AddOrReplaceIntParam(pin->params(), Param::Encoder::Video::H264::FixedFramerate, FALSE);
			}

			AddOrReplaceIntParam(pin->params(), Param::HardwareEncoder, HardwareEncoder::Auto);
		}
	}
}


bool CCaptureDSDlg::ConfigureTranscoder()
{
	if (m_ms.pTranscoder)
	{
		m_ms.pTranscoder->release();
		m_ms.pTranscoder = NULL;
	}

	m_ms.pTranscoder = primo::avblocks::Library::createTranscoder();
	m_ms.pTranscoder->setAllowDemoMode(TRUE);

	// audio input 
	if (m_ms.pAudioInput)
	{
		// Examine the format block.
		if ((m_ms.AudioType.majortype != MEDIATYPE_Audio) || (m_ms.AudioType.formattype != FORMAT_WaveFormatEx) || (m_ms.AudioType.pbFormat == NULL))
		{
			return false;
		}

		WAVEFORMATEX *pWFX = (WAVEFORMATEX*)m_ms.AudioType.pbFormat;

		if (pWFX->wFormatTag != WAVE_FORMAT_PCM)
			return false;

		primo::avblocks::MediaSocket * inSocket = primo::avblocks::Library::createMediaSocket();
		ASSERT(inSocket);
		if (!inSocket)
			return false;

		primo::avblocks::MediaPin *pin = primo::avblocks::Library::createMediaPin();
		pin->setConnection(primo::avblocks::PinConnection::Auto);

		primo::codecs::AudioStreamInfo *streamInfo = primo::avblocks::Library::createAudioStreamInfo();

		streamInfo->setStreamType( primo::codecs::StreamType::LPCM );
		streamInfo->setSampleRate( pWFX->nSamplesPerSec );
		streamInfo->setChannels( pWFX->nChannels );
		streamInfo->setBitsPerSample( pWFX->wBitsPerSample );
		streamInfo->setBytesPerFrame( (pWFX->wBitsPerSample / 8) * pWFX->nChannels );
		
		if(streamInfo->bitsPerSample() <= 8)
		{
			streamInfo->setPcmFlags(primo::codecs::PcmFlags::Unsigned);
		}
		
		pin->setStreamInfo(streamInfo);
		streamInfo->release();

		inSocket->pins()->add(pin);
		inSocket->setStreamType(pin->streamInfo()->streamType());
		pin->release();

		m_audioCB.StreamNumber = m_ms.pTranscoder->inputs()->count();

		m_ms.pTranscoder->inputs()->add(inSocket);
		inSocket->release();
	}

	// video input
	{
		// Examine the format block.
		if ((m_ms.VideoType.majortype != MEDIATYPE_Video) || (m_ms.VideoType.formattype != FORMAT_VideoInfo) ||
			(m_ms.VideoType.cbFormat < sizeof(VIDEOINFOHEADER)) || (m_ms.VideoType.pbFormat == NULL))
		{
			return false;
		}

		VIDEOINFOHEADER* pvh = (VIDEOINFOHEADER*)m_ms.VideoType.pbFormat;

		primo::avblocks::MediaSocket * inSocket = primo::avblocks::Library::createMediaSocket();
		ASSERT(inSocket);
		if (!inSocket)
			return false;

		primo::avblocks::MediaPin *pin = primo::avblocks::Library::createMediaPin();
		pin->setConnection(primo::avblocks::PinConnection::Auto);

		primo::codecs::VideoStreamInfo *streamInfo = primo::avblocks::Library::createVideoStreamInfo();

		if (pvh->AvgTimePerFrame > 0)
			streamInfo->setFrameRate((double)10000000 / pvh->AvgTimePerFrame);
		streamInfo->setBitrate(0);

		streamInfo->setFrameWidth(pvh->bmiHeader.biWidth);
		streamInfo->setFrameHeight(abs(pvh->bmiHeader.biHeight));

		streamInfo->setDisplayRatioWidth(pvh->bmiHeader.biWidth);
		streamInfo->setDisplayRatioHeight(pvh->bmiHeader.biHeight);
		
		streamInfo->setDuration(0.0);
		streamInfo->setScanType(primo::codecs::ScanType::Progressive);

		if (m_ms.VideoType.subtype == MEDIASUBTYPE_MJPG)
		{
			streamInfo->setStreamType(primo::codecs::StreamType::MJPEG);
			streamInfo->setColorFormat(primo::codecs::ColorFormat::YUV422);
		}
		else
		{
			streamInfo->setStreamType(primo::codecs::StreamType::UncompressedVideo);
			streamInfo->setColorFormat(GetColorFormat(m_ms.VideoType.subtype));
		}

		// unsupported capture format
		if (streamInfo->colorFormat() == primo::codecs::ColorFormat::Unknown)
			return false;

		switch (streamInfo->colorFormat())
		{
		case primo::codecs::ColorFormat::BGR32:
		case primo::codecs::ColorFormat::BGRA32:
		case primo::codecs::ColorFormat::BGR24:
		case primo::codecs::ColorFormat::BGR444:
		case primo::codecs::ColorFormat::BGR555:
		case primo::codecs::ColorFormat::BGR565:
			streamInfo->setFrameBottomUp(pvh->bmiHeader.biHeight > 0);
			break;
		}

		pin->setStreamInfo(streamInfo);
		streamInfo->release();

		inSocket->pins()->add(pin);
		inSocket->setStreamType(pin->streamInfo()->streamType());
		pin->release();

		m_videoCB.StreamNumber = m_ms.pTranscoder->inputs()->count();

		m_ms.pTranscoder->inputs()->add(inSocket);
		inSocket->release();
	}

	// configure output
	{
		primo::avblocks::MediaSocket * outSocket = NULL;
		const char* const encoderPreset = GetSelectedPreset().Id;

		// custom output sockets
		if (0 == strcmp(encoderPreset, "custom-mp4-h264-704x576-25fps-aac")) {
			outSocket = CustomOutputSocket(704, 576, 25);
		}
		else if (0 == strcmp(encoderPreset, "custom-mp4-h264-704x576-12fps-aac")) {
			outSocket = CustomOutputSocket(704, 576, 12);
		}
		else if (0 == strcmp(encoderPreset, "custom-mp4-h264-352x288-25fps-aac")) {
			outSocket = CustomOutputSocket(352, 288, 25);
		}
		else if (0 == strcmp(encoderPreset, "custom-mp4-h264-352x288-12fps-aac")) {
			outSocket = CustomOutputSocket(352, 288, 12);
		}
		else {	// output socket from a preset
			outSocket = primo::avblocks::Library::createMediaSocket(encoderPreset);
		}

		ASSERT(outSocket);
		if (!outSocket)
			return false;

		SetRealTimeVideoMode(outSocket);

		outSocket->setFile(m_strOutputFile);

		m_ms.pTranscoder->outputs()->add(outSocket);
		outSocket->release();
	}

	if (!m_ms.pTranscoder->open())
		return false;

	return true;
}

void CCaptureDSDlg::StopRecording()
{
	KillTimer(m_updateStatsEvent);

	ClearGraph();
	
	m_audioCB.Reset();
	m_videoCB.Reset();

	BuildGraph();
}

void CCaptureDSDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_updateStatsEvent)
	{
		UpdateStats();
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CCaptureDSDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (message == WM_STOP_CAPTURE)
	{
		OnCmdRecord();

		int stopReason = (int)wParam;

		if (stopReason >= 0)
		{
			MessageBox(L"An error occurred encoding captured data. The recording has been stopped.",L"AVBlocks",MB_OK | MB_ICONERROR);
		}
		else
		{
			MessageBox(L"An error occurred while recording. The recording has been stopped.", L"Unexpected Error", MB_OK | MB_ICONERROR);
		}

		*pResult = 0;
		return TRUE;
	}

	return CDialog::OnWndMsg(message, wParam, lParam, pResult);
}

void CCaptureDSDlg::CloseApp()
{
	if (m_bRecording)
	{
		OnCmdRecord();
		Sleep(300);
	}

	EndDialog(IDCANCEL);
}

void CCaptureDSDlg::EnableCommandUI(BOOL enable)
{
	GetDlgItem(IDC_BUTTON_CHOOSE_OUTPUT_FILE)->EnableWindow(enable);
	GetDlgItem(IDC_COMBO_PRESETS)->EnableWindow(enable);
	GetDlgItem(IDC_LIST_AUDIO_DEV)->EnableWindow(enable);
	GetDlgItem(IDC_CMD_AUDIO_DEV_PROP)->EnableWindow(enable);
	GetDlgItem(IDC_LIST_VIDEO_DEV)->EnableWindow(enable);
	GetDlgItem(IDC_CMD_VIDEO_DEV_PROP)->EnableWindow(enable);
	GetDlgItem(IDC_CMD_VIDEO_CAPTURE_PROP)->EnableWindow(enable);
}

const PresetDescriptor& CCaptureDSDlg::GetSelectedPreset()
{
	int comboIndex = m_cbPreset.GetCurSel();
	int presetIndex = (int)m_cbPreset.GetItemData(comboIndex);
	return avb_presets[presetIndex];
}

void CCaptureDSDlg::OnBnClickedButtonChooseOutputFile()
{
	UpdateData(TRUE);

	const PresetDescriptor& preset = GetSelectedPreset();

	CString filter, defext;

	if (preset.FileExtension)
	{
		defext = preset.FileExtension;
		filter.Format(L"(*.%hs)|*.%hs|", preset.FileExtension, preset.FileExtension);
	}
	LPCWSTR pDefaultExtension = defext.IsEmpty() ? NULL : defext;

	filter += _TEXT("All files (*.*)|*.*||");

	CFileDialog dlg(FALSE, pDefaultExtension,
		m_strOutputFile.IsEmpty() ? NULL : m_strOutputFile,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, filter, NULL);
	if (IDOK != dlg.DoModal())
		return;

	m_strOutputFile = dlg.m_ofn.lpstrFile;

	UpdateData(FALSE);
}


void CCaptureDSDlg::OnCbnSelchangeComboPresets()
{
	const PresetDescriptor& preset = GetSelectedPreset();

	if (!preset.FileExtension || m_strOutputFile.IsEmpty())
		return;

	CString newext(preset.FileExtension);
	newext.Insert(0, L'.');

	CPath newfile(m_strOutputFile);
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
