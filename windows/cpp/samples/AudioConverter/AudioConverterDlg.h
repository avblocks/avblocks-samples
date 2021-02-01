/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "AvbTranscoder.h"

class CAudioConverterDlg : public CDialog, public primo::avblocks::TranscoderCallback
{
// Construction
public:
	CAudioConverterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CONVERTER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	
	std::string m_OutputPreset;
	HWND m_hWnd;
	bool m_success;

	enum
	{
		WM_CONVERT_THREAD_FINISHED = WM_USER + 1,
		WM_CONVERT_PROGRESS = WM_USER + 2
	};

	bool m_bWorking;
	bool m_bStop;

	bool ValidateInputData();
	void UpdateControls();

	// TranscoderCallback
	virtual void	onProgress(double currentTime, double totalTime);
	virtual void	onStatus(primo::avblocks::TranscoderStatus::Enum status);
	virtual bool_t	onContinue(double currentTime);

	void Convert();
	static UINT ConvertThread(LPVOID pParam);

	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonChooseInputFile();
	afx_msg void OnBnClickedButtonChooseOutputFile();
	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnCbnSelchangeComboPresets();

	afx_msg LRESULT OnConvertThreadFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConvertProgress(WPARAM wParam, LPARAM lParam);

	CString m_strInputFile;
	CString m_strOutputFile;
	CString m_strStatus;
	CComboBox m_cbPreset;
	CProgressCtrl m_progressBar;

	AvbTranscoder m_transcoder;

	const PresetDescriptor& GetSelectedPreset()
	{
		int comboIndex = m_cbPreset.GetCurSel();
		int presetIndex = (int)m_cbPreset.GetItemData(comboIndex);
		return avb_presets[presetIndex];
	}
	
};
