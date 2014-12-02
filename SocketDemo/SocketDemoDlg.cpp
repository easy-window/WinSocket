// SocketDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SocketDemo.h"
#include "SocketDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSocketDemoDlg dialog




CSocketDemoDlg::CSocketDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSocketDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSocketDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSocketDemoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CSocketDemoDlg::OnBnClickedButtonSend)
	ON_BN_CLICKED(IDC_BUTTON2, &CSocketDemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON_SEND2, &CSocketDemoDlg::OnBnClickedButtonSend2)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CSocketDemoDlg message handlers

BOOL CSocketDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	HANDLE hHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RecvFileThreadProc), (LPVOID)this, 0, NULL);
	if(NULL != hHandle)
	{
		CloseHandle(hHandle);
	}

	hHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RecvMegThreadProc), (LPVOID)this, 0, NULL);
	if(NULL != hHandle)
	{
		CloseHandle(hHandle);
	}

	hHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RecvStructThreadProc), (LPVOID)this, 0, NULL);
	if(NULL != hHandle)
	{
		CloseHandle(hHandle);
	}

	CString sIP = m_SendFile.GetIP();

	((CEdit*)GetDlgItem(IDC_EDIT_PORT2))->SetWindowText(_T("7777"));
	((CEdit*)GetDlgItem(IDC_EDIT_PORT))->SetWindowText(_T("9999"));
	((CEdit*)GetDlgItem(IDC_IPADDRESS))->SetWindowText(sIP);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSocketDemoDlg::OnPaint()
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
HCURSOR CSocketDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSocketDemoDlg::OnBnClickedButtonSend()
{
	CString sPort = _T(""), sIP = _T(""), sMeg =_T("");
	((CEdit*)GetDlgItem(IDC_EDIT_PORT))->GetWindowText(sPort);
	((CEdit*)GetDlgItem(IDC_IPADDRESS))->GetWindowText(sIP);
	((CEdit*)GetDlgItem(IDC_EDIT_MEG ))->GetWindowText(sMeg);

	CSocketApp SendSocket;
	SendSocket.Init(_ttoi(sPort), sIP);
	if(SOCKET_OK != SendSocket.Send(sMeg))
	{
		SendSocket.Close();
		return;
	}
	SendSocket.Close();
}

void CSocketDemoDlg::OnBnClickedButton2()
{
	CString sPathName = _T("");
	CFileDialog dlg(TRUE, 
		NULL,
		NULL, 
		OFN_HIDEREADONLY | OFN_EXPLORER,
		_T("All Files (*.*)|*.*;||"), 
		NULL);
	if(dlg.DoModal() == IDOK)
	{
		sPathName = dlg.GetPathName();
		((CEdit*)GetDlgItem(IDC_EDIT_PATH))->SetWindowText(sPathName);
		HANDLE hHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(SendFileThreadProc), (LPVOID)this, 0, NULL);
		if(NULL != hHandle)
		{
			CloseHandle(hHandle);
		}
	}
}

DWORD CSocketDemoDlg::RecvStructThreadProc(LPVOID lpParameter)
{
	CSocketDemoDlg* pMain = (CSocketDemoDlg*)lpParameter;
	CSocketApp RecvSocket;
	RecvSocket.Init(7777);
	while(true)
	{
		MEG stMeg;
		CString sMessage = _T(""), sText = _T("");
		if(SOCKET_OK != RecvSocket.Recv(&stMeg, sizeof(MEG)))
		{
			RecvSocket.Close();
			return 1;
		}
		sMessage.Format(_T("Id:%d, Message:%s"), stMeg.nId, stMeg.szMeg);
		((CEdit*)pMain->GetDlgItem(IDC_EDIT_RECV))->GetWindowText(sText);
		((CEdit*)pMain->GetDlgItem(IDC_EDIT_RECV))->SetWindowText(sText + sMessage + _T("\r\n"));
	}
	RecvSocket.Close();
	return 0;
}

DWORD CSocketDemoDlg::RecvMegThreadProc(LPVOID lpParameter)
{
	CSocketDemoDlg* pMain = (CSocketDemoDlg*)lpParameter;
	CSocketApp RecvSocket;
	RecvSocket.Init(9999);
	while(true)
	{
		CString sMessage = _T(""), sText = _T("");
		if(SOCKET_OK != RecvSocket.Recv(sMessage))
		{
			RecvSocket.Close();
			return 1;
		}
		((CEdit*)pMain->GetDlgItem(IDC_EDIT_RECV))->GetWindowText(sText);
		((CEdit*)pMain->GetDlgItem(IDC_EDIT_RECV))->SetWindowText(sText + _T("Message:") + sMessage + _T("\r\n"));
	}
	RecvSocket.Close();
	return 0;
}

DWORD CSocketDemoDlg::SendLoadThreadProc(LPVOID lpParameter)
{
	CSocketDemoDlg* pMain = (CSocketDemoDlg*)lpParameter;
	Sleep(1000);
	while(!pMain->m_SendFile.isSendDone())
	{
		CString sValue = _T("");
		sValue.Format(_T(" %0.4f%%"), pMain->m_SendFile.GetSendLoading());
		((CEdit*)pMain->GetDlgItem(IDC_STATIC_LOAD))->SetWindowText(sValue);
	}
	return 0;
}

DWORD CSocketDemoDlg::RecvLoadThreadProc(LPVOID lpParameter)
{
	CSocketDemoDlg* pMain = (CSocketDemoDlg*)lpParameter;
	Sleep(1000);
	while(!pMain->m_RecvFile.isRecvDone())
	{
		CString sValue = _T("");
		sValue.Format(_T(" %0.4f%%"), pMain->m_RecvFile.GetRecvLoading());
		((CEdit*)pMain->GetDlgItem(IDC_STATIC_LOAD2))->SetWindowText(sValue);
	}
	return 0;
}

DWORD CSocketDemoDlg::SendFileThreadProc(LPVOID lpParameter)
{
	CSocketDemoDlg* pMain = (CSocketDemoDlg*)lpParameter;
	CString sPort = _T(""), sIP = _T(""), sPath = _T("");
	((CEdit*)pMain->GetDlgItem(IDC_IPADDRESS))->GetWindowText(sIP);
	((CEdit*)pMain->GetDlgItem(IDC_EDIT_PATH))->GetWindowText(sPath);

	pMain->m_SendFile.Init(8888, sIP, SOCKET_TYPE_FILE);

	HANDLE hHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(SendLoadThreadProc), (LPVOID)lpParameter, 0, NULL);
	if(NULL != hHandle)
	{
		CloseHandle(hHandle);
	}

	if(SOCKET_OK != pMain->m_SendFile.SendFile(sPath))
	{
		pMain->m_SendFile.Close();
		AfxMessageBox(_T("Send Faild..."));
		return 1;
	}
	pMain->m_SendFile.Close();
	AfxMessageBox(_T("Send Done..."));

	return 0;
}

DWORD CSocketDemoDlg::RecvFileThreadProc(LPVOID lpParameter)
{
	CSocketDemoDlg* pMain = (CSocketDemoDlg*)lpParameter;
	CString sFilename = _T("");
	pMain->m_RecvFile.Init(8888, SOCKET_ANY_IP, SOCKET_TYPE_FILE);
	while(true)
	{
		if(SOCKET_OK != pMain->m_RecvFile.RecvFileName(sFilename))
		{
			pMain->m_RecvFile.Close();
			return 1;
		}

		CString sPathName = _T("");
		CFileDialog dlg(FALSE, 
			NULL,
			sFilename, 
			OFN_HIDEREADONLY | OFN_EXPLORER,
			_T("All Files (*.*)|*.*;||"), 
			NULL);
		if(dlg.DoModal() == IDOK)
		{
			HANDLE hHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(RecvLoadThreadProc), (LPVOID)lpParameter, 0, NULL);
			if(NULL != hHandle)
			{
				CloseHandle(hHandle);
			}

			if(SOCKET_OK != pMain->m_RecvFile.RecvFile(dlg.GetPathName()))
			{
				pMain->m_RecvFile.Close();
				return 1;
			}
		}
		else
		{
			if(SOCKET_OK != pMain->m_RecvFile.RecvFile(_T(""), false))
			{
				pMain->m_RecvFile.Close();
				return 1;
			}

		}
	}
	pMain->m_RecvFile.Close();

	return 0;
}

void CSocketDemoDlg::OnBnClickedButtonSend2()
{
	MEG stMeg;
	CString sId = _T(""), sMeg = _T(""), sPort = _T(""), sIP = _T("");
	((CEdit*)GetDlgItem(IDC_EDIT_PORT2))->GetWindowText(sPort);
	((CEdit*)GetDlgItem(IDC_IPADDRESS))->GetWindowText(sIP);
	((CEdit*)GetDlgItem(IDC_EDIT_ID))->GetWindowText(sId);
	((CEdit*)GetDlgItem(IDC_EDIT_MEG2))->GetWindowText(sMeg);

	stMeg.nId = _ttoi(sId);
	_tcscpy(stMeg.szMeg, sMeg.GetBuffer());
	sMeg.ReleaseBuffer();

	CSocketApp SendSocket;
	SendSocket.Init(_ttoi(sPort), sIP);
	if(SOCKET_OK != SendSocket.Send(&stMeg, sizeof(MEG)))
	{
		SendSocket.Close();
	}
	SendSocket.Close();
}

void CSocketDemoDlg::OnClose()
{
	m_RecvFile.Close();
	m_SendFile.Close();
	CDialog::OnClose();
}
