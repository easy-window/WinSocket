#pragma once

#include "winsock2.h"
#include "vector"
using namespace std;

#define SOCKET_BUFFERSIZE			102400

#define SOCKET_MEG_SIZE				1024

#define SOCKET_ANY_IP				_T("0.0.0.0")

#define SOCKET_TYPE_MEG				0
#define SOCKET_TYPE_FILE			1

#define SOCKET_OK					0x000000		
#define SOCKET_ERR_INIT				0x000001		// Init Socket Failed
#define SOCKET_ERR_SET				0x000002		// Init Socker Setting Failed
#define SOCKET_ERR_SEND				0x000003		// Send Message Failed
#define SOCKET_ERR_INITSEND			0x000004		// Init to Send Failed
#define SOCKET_ERR_RECV				0x000005		// Recv Message Failed
#define SOCKET_ERR_BIND				0x000006		// Bind Socket Failed

#define SOCKET_ERR_UNKNOWNTYPE		0x000007		// Unknown Socket Type
#define SOCKET_ERR_CONNECT			0x000008		// Connect TCP Socket Failed
#define SOCKET_ERR_LISTEN			0x000009		// Create Listen Socket Failed

#define SOCKET_ERR_SENDNAME			0x000010		// Send File Name Failed
#define SOCKET_ERR_SENDLEN			0x000011		// Send File Len Failed
#define SOCKET_ERR_SENDING			0x000012		// File is Sending
#define SOCKET_ERR_SENDFILE			0x000013		// Send File Failed
#define SOCKET_ERR_READFILE			0x000014		// Read File Failed
	
#define SOCKET_ERR_ACCEPT			0x000015		// Create a Accept Socket Failed
#define SOCKET_ERR_RECVNAME			0x000016		// Recv File Name Failed

#define SOCKET_ERR_CREATEFILE		0x000020		// Create File Failed

typedef struct _ADAPTER_INFO
{
	CString			sMac;
	vector<CString> sIPs;
	UINT			uType;
	CString			sName;
	CString			sDesc;

	_ADAPTER_INFO();
	_ADAPTER_INFO& operator = (const _ADAPTER_INFO info);

}ADAPTER_INFO, *PADAPTER_INFO;

typedef vector<ADAPTER_INFO> ADAPTER_LIST;

class CSocketApp
{
public:
	CSocketApp(void);
	CSocketApp(int nPort, int nType = SOCKET_TYPE_MEG);
	CSocketApp(int nPort, CString sIP, int nType = SOCKET_TYPE_MEG);
	~CSocketApp(void);


	DWORD	Init(void);
	DWORD	Init(int nPort, CString sIP = SOCKET_ANY_IP, int nType = SOCKET_TYPE_MEG); // Default Receive

	DWORD	Send(CString sMeg);
	DWORD	Send(void* sMeg, int nLen);
	DWORD	Recv(CString& sMeg);
	DWORD	Recv(void* sMeg, int nLen);

	DWORD	SendFile(CString sPath);
	DWORD	RecvFileName(CString& sFilename);
	DWORD	RecvFile(CString sPath, bool bRecv = true);

	void	Close(void);

	int		GetLastError(){ return m_nLastError; };

	bool	isSendDone(){ return !m_bSendFile; };
	bool	isRecvDone(){ return !m_bRecvFile; };

	double	GetSendLoading(){ return m_fSendPerson; }
	double	GetRecvLoading(){ return m_fRecvPerson; }
	
	CString GetIP(void);
	CString GetHostname(void);

	CString GetMac(void); // Get the first valid Mac address
	ADAPTER_LIST GetMacInfo(void);


private:
	DWORD Set(int nPort, CString sIP, int nType = SOCKET_TYPE_MEG);

	SOCKET	m_socket;
	SOCKET	m_sockFile;
	struct  sockaddr_in m_sockaddr;
	WSADATA	m_wsaData;

	int		m_nLastError;
	double	m_fSendPerson;
	double	m_fRecvPerson;
	
	bool	m_bSendFile;
	bool	m_bRecvFile;

};
