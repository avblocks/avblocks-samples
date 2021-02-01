// VideoCapturePropertiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CaptureDS.h"
#include "VideoCapturePropertiesDialog.h"
#include "afxdialogex.h"


// VideoCapturePropertiesDialog dialog
IMPLEMENT_DYNAMIC(VideoCapturePropertiesDialog, CDialog)

VideoCapturePropertiesDialog::VideoCapturePropertiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(VideoCapturePropertiesDialog::IDD, pParent)
{
	m_pStreamConfig = NULL;
}

VideoCapturePropertiesDialog::~VideoCapturePropertiesDialog()
{
}

void VideoCapturePropertiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_VIDEO_FORMAT, m_comboVideoFormat);
	DDX_Control(pDX, IDC_COMBO_FPS, m_comboFrameRate);
}


BEGIN_MESSAGE_MAP(VideoCapturePropertiesDialog, CDialog)
	ON_BN_CLICKED(IDOK, &VideoCapturePropertiesDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &VideoCapturePropertiesDialog::OnBnClickedCancel)
END_MESSAGE_MAP()

bool VideoCapturePropertiesDialog::SetFormat(int formatIndex, int frameRate)
{
	int capsCount, capSize;
	BYTE *pSCC = NULL;

	HRESULT hr = m_pStreamConfig->GetNumberOfCapabilities(&capsCount, &capSize);
	if (FAILED(hr))
		return false;

	pSCC = new BYTE[capSize];

	AM_MEDIA_TYPE *pmt = NULL;
	hr = m_pStreamConfig->GetStreamCaps(formatIndex, &pmt, pSCC);

	if (hr == S_OK)
	{
		VIDEOINFOHEADER *pvh = (VIDEOINFOHEADER*)pmt->pbFormat;

		if(frameRate > 0)
		{
			pvh->AvgTimePerFrame = (long)(10000000.0 / frameRate);
		}

		hr = m_pStreamConfig->SetFormat(pmt);
		
		DeleteMediaType(pmt);
	}

	delete [] pSCC;

	return SUCCEEDED(hr);
}

void VideoCapturePropertiesDialog::OnBnClickedOk()
{
	int index = m_comboVideoFormat.GetCurSel();
	if(index >= 0)
	{
		int formatIndex = m_comboVideoFormat.GetItemData(index);

		int frameRate = -1;
		index = m_comboFrameRate.GetCurSel();
		if(index >= 0)
		{
			frameRate = m_comboFrameRate.GetItemData(index);
		}

		if(!SetFormat(formatIndex, frameRate))
		{
			MessageBox(_T("Failed to set the selected video format."));
		}
	}

	CDialog::OnOK();
}

void VideoCapturePropertiesDialog::OnBnClickedCancel()
{
	CDialog::OnCancel();
}


struct FormatEntry
{
	GUID videoSubType;
	const char* name;
};

static FormatEntry FormatsTab[] = {
		
	{ MEDIASUBTYPE_MJPG,	"MJPG"},

	{ MEDIASUBTYPE_RGB24,	"RGB24"},
	{ MEDIASUBTYPE_ARGB32,	"ARGB32"},
	{ MEDIASUBTYPE_RGB32,	"RGB32"},
	{ MEDIASUBTYPE_RGB565,	"RGB565"},
	{ MEDIASUBTYPE_ARGB1555, "ARGB1555"},
	{ MEDIASUBTYPE_RGB555,	"RGB555"},
	{ MEDIASUBTYPE_ARGB4444,  "ARGB4444"},
		
	{ MEDIASUBTYPE_YV12, "YV12"},
	{ MEDIASUBTYPE_I420, "I420"},
	{ MEDIASUBTYPE_IYUV, "IYUV"},
	{ MEDIASUBTYPE_YUY2, "YUY2"},

	{ MEDIASUBTYPE_NV12, "NV12"},
	{ MEDIASUBTYPE_UYVY, "UYVY"},
	{ MEDIASUBTYPE_Y411, "Y411"},
	{ MEDIASUBTYPE_Y41P, "Y41P"},
	{ MEDIASUBTYPE_YVU9, "YVU9"}
};

const char* GetSubtypeString(GUID& videoSubtype)
{
	for (int i = 0; i < sizeof(FormatsTab) / sizeof(FormatEntry); i++)
	{
		if (FormatsTab[i].videoSubType == videoSubtype)
			return FormatsTab[i].name;
	}

	return NULL;
}

bool VideoCapturePropertiesDialog::InitFormatsList()
{
	if(NULL == m_pStreamConfig)
		return false;

	int capsCount, capSize;
	BYTE *pSCC = NULL;

	HRESULT hr = m_pStreamConfig->GetNumberOfCapabilities(&capsCount, &capSize);
	if (FAILED(hr))
		return false;

	pSCC = new BYTE[capSize];

	int videoFormatIndex = -1;
    int minFps = -1;
    int maxFps = -1;

    int currentWidth = 0;
    int currentHeight = 0;
    GUID currentSubType;
    int currentFps = 0;

    {
        AM_MEDIA_TYPE *pmt = NULL;
        hr = m_pStreamConfig->GetFormat(&pmt);
		if (SUCCEEDED(hr))
		{
			VIDEOINFOHEADER *pvh = (VIDEOINFOHEADER*)pmt->pbFormat;
			currentFps = (int)(10000000.0 / pvh->AvgTimePerFrame);
			currentWidth = pvh->bmiHeader.biWidth;
			currentHeight = pvh->bmiHeader.biHeight;
			currentSubType = pmt->subtype;

			DeleteMediaType(pmt);
		}
    }

    for (int i = 0; i < capsCount; ++i)
    {
        AM_MEDIA_TYPE *pmt = NULL;
		hr = m_pStreamConfig->GetStreamCaps(i, &pmt, pSCC);

		if (hr == S_OK)
		{
			if(pmt->formattype == FORMAT_VideoInfo)
			{
				const char *pFormatName = GetSubtypeString(pmt->subtype);
				if(pFormatName)
				{
					VIDEOINFOHEADER *pvh = (VIDEOINFOHEADER*)pmt->pbFormat;
					VIDEO_STREAM_CONFIG_CAPS *pCaps = (VIDEO_STREAM_CONFIG_CAPS*)pSCC;

                    int fps = (int)(10000000.0 / pCaps->MaxFrameInterval);
                    if ((minFps < 0) || (minFps > fps))
                        minFps = fps;

                    fps = (int)(10000000.0 / pCaps->MinFrameInterval);
                    if ((maxFps < 0) || (maxFps < fps))
                        maxFps = fps;

					CString formatName(pFormatName);
					CString capline;
					capline.Format(_T("%d x %d, min fps %.2f, max fps %.2f, %s"), pvh->bmiHeader.biWidth, pvh->bmiHeader.biHeight, 
						10000000.0 / pCaps->MaxFrameInterval, 10000000.0 / pCaps->MinFrameInterval, formatName);

                    if ((pvh->bmiHeader.biWidth == currentWidth) &&
                        (pvh->bmiHeader.biHeight == currentHeight) &&
                        (pmt->subtype == currentSubType))
                    {
                        videoFormatIndex = m_comboVideoFormat.GetCount();
                    }

					m_comboVideoFormat.AddString(capline);
					m_comboVideoFormat.SetItemData(m_comboVideoFormat.GetCount() - 1, i);
				}
			}
			
			DeleteMediaType(pmt);
		}
    }

	delete [] pSCC;
	pSCC = NULL;

    if (videoFormatIndex >= 0)
		m_comboVideoFormat.SetCurSel(videoFormatIndex);

    if ((minFps >= 0) && (maxFps >= 0))
    {
        for (int i = minFps; i <= maxFps; i++)
        {
			CString str;
			str.Format(_T("%d"), i);

			m_comboFrameRate.AddString(str);
			m_comboFrameRate.SetItemData(m_comboFrameRate.GetCount() - 1, i);

            if (currentFps == i)
                m_comboFrameRate.SetCurSel(m_comboFrameRate.GetCount() - 1);
        }
    }

	return true;
}

BOOL VideoCapturePropertiesDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitFormatsList();

	return TRUE; 
}
