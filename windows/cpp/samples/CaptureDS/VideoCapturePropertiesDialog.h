#pragma once
#include "afxwin.h"


// VideoCapturePropertiesDialog dialog

class VideoCapturePropertiesDialog : public CDialog
{
	DECLARE_DYNAMIC(VideoCapturePropertiesDialog)

public:
	VideoCapturePropertiesDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~VideoCapturePropertiesDialog();

// Dialog Data
	enum { IDD = IDD_VIDEO_PROPS };

	IAMStreamConfig *m_pStreamConfig;

	bool InitFormatsList();
	bool SetFormat(int formatIndex, int frameRate);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CComboBox m_comboVideoFormat;
	CComboBox m_comboFrameRate;

	void set_StreamConfig(IAMStreamConfig *pStreamConfig)
	{
		m_pStreamConfig = pStreamConfig;
	}

	IAMStreamConfig* get_StreamConfig()
	{
		return m_pStreamConfig;
	}
};
