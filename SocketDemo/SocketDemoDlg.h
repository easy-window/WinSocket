// SocketDemoDlg.h : header file
//

#pragma once
#include "SocketApp.h"

typedef struct _MEG
{
	int		nId;
	TCHAR	szMeg[1024];

	_MEG()
	{
		nId = 0;
		_tcscpy(szMeg, _T(""));
	}

	_MEG& operator = (const _MEG stMeg)
	{
		if(this != &stMeg)
		{
			nId = stMeg.nId;
			_tcscpy(szMeg, stMeg.szMeg);
		}
		return *this;
	}

}MEG, *PMEG;

// CSocketDemoDlg dialog
class CSocketDemoDlg : public CDialog
{
// Construction
public:
	CSocketDemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SOCKETDEMO_DIALOG };

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
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnBnClickedButton2();

	static DWORD RecvMegThreadProc(LPVOID lpParameter);
	static DWORD RecvStructThreadProc(LPVOID lpParameter);
	
	static DWORD SendFileThreadProc(LPVOID lpParameter);
	static DWORD RecvFileThreadProc(LPVOID lpParameter);
	static DWORD SendLoadThreadProc(LPVOID lpParameter);
	static DWORD RecvLoadThreadProc(LPVOID lpParameter);

	CSocketApp m_SendFile;
	CSocketApp m_RecvFile;

public:
	afx_msg void OnBnClickedButtonSend2();
public:
	afx_msg void OnClose();
};
