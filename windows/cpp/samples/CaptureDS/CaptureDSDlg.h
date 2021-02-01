
// VideoRecorderDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "MediaState.h"
#include "SampleCallbacks.h"
#include "AvbTranscoder.h"

typedef std::deque<std::wstring> StringList;


// CCaptureDSDlg dialog
class CCaptureDSDlg : public CDialog
{
// Construction
public:
	CCaptureDSDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CAPTURE_DS };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_listAudioDev;
	CButton m_cmdAudioDevProp;
	CComboBox m_listVideoDev;
	CButton m_cmdVideoCaptureProp;
	CButton m_cmdRecord;
	void OnOK();
	void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

public:
	void ResetStats();

protected:
	void EnumInputDev(const IID & devClass, CComboBox& list, StringList& devices);
	HRESULT InitInputDev(MediaState& ms, int videoItem, int audioItem);
	bool CheckInputDevice(IBaseFilter* pInputDevice);
	void ShowPropPages(IUnknown* pUnk);


	bool StartRecording();
	void StopRecording();
	void UpdateStats();
	void CloseApp();
	void EnableCommandUI(BOOL enable);

	StringList m_audioDevices; // device monikors
	StringList m_videoDevices; // device monikors
	bool m_bRecording;
	bool m_bCmdRecordBusy;
	MediaState m_ms;
	const UINT_PTR m_updateStatsEvent;

	SampleGrabberCB m_videoCB;
	SampleGrabberCB m_audioCB;

	DWORD recStartTime; // tick count
    DWORD fpsStartTime; // current fps
    DWORD fpsProcessed; // current fps

	bool ConfigureTranscoder();

	bool BuildGraph();
	void ClearGraph();

	const PresetDescriptor& GetSelectedPreset();

public:
	afx_msg void OnCbnSelchangeListAudioDev();
	afx_msg void OnCbnSelchangeListVideoDev();
	afx_msg void OnCmdAudioDevProp();
	afx_msg void OnCmdVideoDevProp();
	afx_msg void OnCmdVideoCaptureProp();
	afx_msg void OnCmdRecord();
	CStatic m_txtRecording;
	CStatic m_preview;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	CComboBox m_cbPreset;
	CString m_strOutputFile;
	afx_msg void OnBnClickedButtonChooseOutputFile();
	afx_msg void OnCbnSelchangeComboPresets();
};
