#include "StdAfx.h"
#include "SocketApp.h"
#include <iphlpapi.h>
#include <conio.h>
#pragma comment(lib, "IPHLPAPI.lib")

ADAPTER_INFO::_ADAPTER_INFO()
{
	sMac  = _T("");
	sIPs.clear();
	uType = 0;
	sName = _T("");
	sDesc = _T("");

}

_ADAPTER_INFO& ADAPTER_INFO::operator = (const ADAPTER_INFO info)
{
	if(this != &info)
	{
		sMac  = info.sMac;
		sIPs  = info.sIPs;
		uType = info.uType;
		sName = info.sName;
		sDesc = info.sDesc;
	}

	return *this;

}

//////////////////////////////////////////////////////////////////////////

CSocketApp::CSocketApp(void)
{
	m_socket = INVALID_SOCKET;
	m_sockFile = INVALID_SOCKET;
	Init();
}

CSocketApp::~CSocketApp(void)
{
	Close();
	WSACleanup();
}

CSocketApp::CSocketApp(int nPort, int nType)
{
	m_socket = INVALID_SOCKET;
	Init();
	Set(nPort, SOCKET_ANY_IP, nType);
}

CSocketApp::CSocketApp(int nPort, CString sIP, int nType)
{
	m_socket = INVALID_SOCKET;
	Init();
	Set(nPort, sIP, nType);
}

DWORD CSocketApp::Init(void)
{
	m_nLastError = 0;
	m_fSendPerson = 0;
	m_fRecvPerson = 0;

	m_bSendFile = false;
	m_bRecvFile = false;

	if (LOBYTE(m_wsaData.wVersion) != 2 ||
        HIBYTE(m_wsaData.wVersion) != 2 )
	{
		if(0 != WSAStartup(MAKEWORD(2, 2), &m_wsaData))
		{
			m_nLastError = WSAGetLastError();
			return SOCKET_ERR_INIT;
		}
	}
	return SOCKET_OK;
}

DWORD CSocketApp::Init(int nPort, CString sIP, int nType)
{
	if(Init())
		return SOCKET_ERR_INIT;
	return Set(nPort, sIP, nType);
}

DWORD CSocketApp::Set(int nPort, CString sIP, int nType)
{
	m_nLastError = 0;
	USES_CONVERSION;
	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_port = htons(nPort);
	m_sockaddr.sin_addr.s_addr = inet_addr(W2A(sIP));
	if(SOCKET_TYPE_MEG == nType)
	{
		m_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if(SOCKET_ANY_IP == sIP)
		{
			if(SOCKET_ERROR == bind(m_socket,(LPSOCKADDR)&m_sockaddr,sizeof(m_sockaddr)))
			{
				m_nLastError = WSAGetLastError();
				return SOCKET_ERR_BIND;
			}

		}
	}
	else if(SOCKET_TYPE_FILE == nType)
	{
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(SOCKET_ANY_IP != sIP)
		{
			if(SOCKET_ERROR == connect(m_socket, (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr)))
			{
				m_nLastError = WSAGetLastError();
				return SOCKET_ERR_CONNECT;
			}
		}
		else
		{
			if(SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&m_sockaddr, sizeof(m_sockaddr)))
			{
				m_nLastError = WSAGetLastError();
				return SOCKET_ERR_CONNECT;
			}
			if(SOCKET_ERROR == listen(m_socket, 5))
			{
				m_nLastError = WSAGetLastError();
				return SOCKET_ERR_LISTEN;
			}
		}
	}
	else
	{
		return SOCKET_ERR_UNKNOWNTYPE;
	}

	if(m_socket == INVALID_SOCKET)
		return SOCKET_ERR_SET;
	return SOCKET_OK;
}

DWORD CSocketApp::Send(CString sMeg)
{
	m_nLastError = 0;
	USES_CONVERSION;
	TCHAR szBuff[SOCKET_MEG_SIZE];

	_tcscpy(szBuff, sMeg.GetBuffer());
	sMeg.ReleaseBuffer();
	bool op = true;
	if(SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char FAR *)&op, sizeof(op)))
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_INITSEND;
	}

	if(SOCKET_ERROR == sendto(m_socket, (char*)szBuff, sizeof(szBuff), 0, (LPSOCKADDR)&m_sockaddr, sizeof(m_sockaddr)) == SOCKET_ERROR)
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_SEND;
	}

	return SOCKET_OK;
}

DWORD CSocketApp::Recv(CString& sMeg)
{
	m_nLastError = 0;
	USES_CONVERSION;
	TCHAR szBuff[SOCKET_MEG_SIZE];
	int socklen = sizeof(m_sockaddr);
	if(SOCKET_ERROR == recvfrom(m_socket, (char*)szBuff, sizeof(szBuff), 0, (LPSOCKADDR)&m_sockaddr, (int*)&socklen) == SOCKET_ERROR)
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_RECV;
	}
	sMeg.Format(_T("%s"), szBuff);
	return SOCKET_OK;
}


DWORD CSocketApp::Send(void* sMeg, int nLen)
{
	m_nLastError = 0;
	bool op = true;
	if(SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_BROADCAST, (char FAR *)&op, sizeof(op)))
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_INITSEND;
	}
	if(SOCKET_ERROR == sendto(m_socket, (char*)sMeg, nLen, 0, (LPSOCKADDR)&m_sockaddr, sizeof(m_sockaddr)) == SOCKET_ERROR)
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_SEND;
	}
	return SOCKET_OK;
}

DWORD CSocketApp::Recv(void* sMeg, int nLen)
{
	m_nLastError = 0;
	USES_CONVERSION;
	int socklen = sizeof(m_sockaddr);
	if(SOCKET_ERROR == recvfrom(m_socket, (char*)sMeg, nLen, 0, (LPSOCKADDR)&m_sockaddr, (int*)&socklen) == SOCKET_ERROR)
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_RECV;
	}
	return SOCKET_OK;
}

DWORD CSocketApp::SendFile(CString sPath)
{
	m_nLastError = 0;
	char caBuffer[SOCKET_BUFFERSIZE]; 
	
	if(m_bSendFile)
		return SOCKET_ERR_SENDING;

	m_bSendFile = true;
	HANDLE hFile = CreateFile(sPath, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);
	if(INVALID_HANDLE_VALUE == hFile)
	{
		m_bSendFile = false;
		CloseHandle(hFile);
		m_nLastError = GetLastError();
		return SOCKET_ERR_CREATEFILE;
	}

	LARGE_INTEGER i64FileSizeSend, i64FileSizeTotal;
	if(FALSE == GetFileSizeEx(hFile, &i64FileSizeSend))
	{
		m_bSendFile = false;
		CloseHandle(hFile);
		m_nLastError = GetLastError();
		return SOCKET_ERR_READFILE;
	}
	i64FileSizeTotal = i64FileSizeSend;
	
	USES_CONVERSION;
	char szPath[SOCKET_MEG_SIZE] = "";
	strcpy(szPath, W2A(sPath.Mid(sPath.ReverseFind('\\') + 1, sPath.GetLength() - sPath.ReverseFind('\\') - 1)));
	if(SOCKET_ERROR == send(m_socket, szPath, SOCKET_MEG_SIZE, 0))
	{
		m_bSendFile = false;
		CloseHandle(hFile);
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_SENDNAME;
	}
	Sleep(100);
	if(SOCKET_ERROR == send(m_socket, (char*)&i64FileSizeSend, sizeof(LARGE_INTEGER),0))
	{
		m_bSendFile = false;
		CloseHandle(hFile);
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_SENDLEN;
	}

	while( i64FileSizeSend.QuadPart > 0 )
	{
		DWORD dwNumBytes, dwNumberToRead = SOCKET_BUFFERSIZE;
		while(true)
		{
			if( i64FileSizeSend.QuadPart <= dwNumberToRead )
			{
				dwNumberToRead = i64FileSizeSend.QuadPart;
			}
			if(FALSE == ReadFile(hFile, caBuffer, dwNumberToRead, &dwNumBytes, 0))
			{
				m_nLastError = GetLastError();
				return SOCKET_ERR_READFILE;
			}
			i64FileSizeSend.QuadPart -= dwNumBytes;
			if( dwNumBytes < dwNumberToRead )
			{
				dwNumberToRead -= dwNumBytes;
			}
			else
			{
				break;
			}
		}

		if(SOCKET_ERROR == send(m_socket, caBuffer, SOCKET_BUFFERSIZE, 0))
		{
			m_bSendFile = false;
			CloseHandle(hFile);
			m_nLastError = WSAGetLastError();
			return SOCKET_ERR_SENDFILE;
		}

		m_fSendPerson = 100.0 - (i64FileSizeSend.QuadPart * 100.0) / i64FileSizeTotal.QuadPart;
	}
	CloseHandle(hFile);

	m_bSendFile = false;
	return SOCKET_OK;
}

DWORD CSocketApp::RecvFileName(CString& sFilename)
{
	int nLen = sizeof(m_sockaddr);
	m_sockFile = accept(m_socket, (SOCKADDR*)&m_sockaddr, &nLen);
	if(m_sockFile == INVALID_SOCKET)
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_ACCEPT;
	}

	USES_CONVERSION;
	char szPath[SOCKET_MEG_SIZE] = "";
	if(SOCKET_ERROR == recv(m_sockFile, szPath, SOCKET_MEG_SIZE, 0))
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_RECVNAME;
	}

	m_bRecvFile = true;
	sFilename = A2W(szPath);

	return SOCKET_OK;
}

DWORD CSocketApp::RecvFile(CString sPath, bool bRecv)
{
	if(!bRecv)
	{
		goto RECVDONE;
	}

	char caBuffer[SOCKET_BUFFERSIZE]; 

	LARGE_INTEGER i64FileSizeSend, i64FileSizeTotal;
	if(SOCKET_ERROR == recv(m_sockFile, (char*)&i64FileSizeSend.QuadPart, sizeof(i64FileSizeSend.QuadPart), 0))
	{
		m_nLastError = WSAGetLastError();
		return SOCKET_ERR_RECVNAME;
	}
	i64FileSizeTotal = i64FileSizeSend;

	HANDLE hFile = CreateFile(sPath, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_ALWAYS, 0, 0);

	while(i64FileSizeSend.QuadPart > 0)
	{
		ZeroMemory(caBuffer, SOCKET_BUFFERSIZE);
		int nSize = 0;
		nSize = recv(m_sockFile, caBuffer, SOCKET_BUFFERSIZE, 0);

		DWORD dwNumBytes = 0;
		while(true)
		{
			if(i64FileSizeSend.QuadPart < nSize)
			{
				nSize = i64FileSizeSend.QuadPart;
			}

			if(FALSE == WriteFile(hFile, caBuffer, nSize, &dwNumBytes, 0))
			{
				m_nLastError = GetLastError();
				return SOCKET_ERR_READFILE;
			}

			i64FileSizeSend.QuadPart -= dwNumBytes;

			if(dwNumBytes < nSize)
			{
				nSize -= dwNumBytes;
			}
			else
			{
				break;
			}
		}

		m_fRecvPerson = 100.0 - (i64FileSizeSend.QuadPart * 100.0) / i64FileSizeTotal.QuadPart;
	}

	CloseHandle(hFile);
	RECVDONE: closesocket(m_sockFile);
	m_sockFile = INVALID_SOCKET;

	m_bRecvFile = false;
	return SOCKET_OK;
}


void CSocketApp::Close(void)
{
	if(m_socket != INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

CString CSocketApp::GetIP()
{
	USES_CONVERSION;
	CString sHostIP;
	char HostName[100]; 
	gethostname(HostName, 100);
	hostent* hn; 
	hn = gethostbyname(HostName);
	sHostIP = A2W(inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]));
	return sHostIP;
}

CString CSocketApp::GetMac(void)
{
	ADAPTER_LIST AdapterList;
	AdapterList = GetMacInfo();
	CString sMacAddr = _T("");

	ADAPTER_LIST::iterator AdapterItem;
	for (AdapterItem = AdapterList.begin(); AdapterItem != AdapterList.end(); AdapterItem++)
	{
		CString sIP = AdapterItem->sIPs[0];
		if (_T("0.0.0.0") != sIP && _T("127.0.0.1") != sIP)
		{
			sMacAddr = AdapterItem->sMac;
			break;
		}
	}

	return sMacAddr;
}

CString CSocketApp::GetHostname(void)
{
	char name[128];
	CString sHost;
	gethostname(name, sizeof(name));
	USES_CONVERSION;
	sHost = A2W(name);
	return sHost;
}

ADAPTER_LIST CSocketApp::GetMacInfo(void)
{
	USES_CONVERSION;
	ADAPTER_LIST AdapterList;
	
	PIP_ADAPTER_INFO pIpAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pIpAdapterInfo = (IP_ADAPTER_INFO *) malloc(sizeof (IP_ADAPTER_INFO));
    if (pIpAdapterInfo == NULL) {
		AllocConsole();
        _cprintf("Error allocating memory needed to call GetAdaptersinfo\n");
        FreeConsole();
		return AdapterList;
    }
    if (GetAdaptersInfo(pIpAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(pIpAdapterInfo);
        pIpAdapterInfo = (IP_ADAPTER_INFO *) malloc(ulOutBufLen);
        if (pIpAdapterInfo == NULL) {
			AllocConsole();
            _cprintf("Error allocating memory needed to call GetAdaptersinfo\n");
			FreeConsole();
            return AdapterList;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pIpAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pIpAdapterInfo;
        while (pAdapter) {
			ADAPTER_INFO AdapterInfo;

			AdapterInfo.sName = A2W(pAdapter->AdapterName);
			AdapterInfo.sDesc = A2W(pAdapter->Description);
			AdapterInfo.uType = pAdapter->Type;

			CString sMacAddr;
			CString sTemp;
			for (DWORD i = 0; i < pAdapter->AddressLength; i++)
			{
				sTemp.Format(_T("%02x"), pAdapter->Address[i]);
				sMacAddr += sTemp;
			}
			AdapterInfo.sMac = sMacAddr;

			IP_ADDR_STRING *pIpAddrString =&(pAdapter->IpAddressList);
			do 
			{
				AdapterInfo.sIPs.push_back(A2W(pIpAddrString->IpAddress.String));
				pIpAddrString = pIpAddrString->Next;
			} while (pIpAddrString);
			pAdapter = pAdapter->Next;

			AdapterList.push_back(AdapterInfo);
        }
    } else {
		AllocConsole();
        _cprintf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
		FreeConsole();
    }
    if (pIpAdapterInfo)
        free(pIpAdapterInfo);
	return AdapterList;
}