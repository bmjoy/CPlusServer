#include "stdafx.h"

// CIocpWorker
CIocpWorker::CIocpWorker()
{

}

CIocpWorker::~CIocpWorker()
{

}

BOOL CIocpWorker::BeginWorkerPool(int nThreads, int nConcurrency)
{
	for (int i = 0; i < nThreads; i++)
	{
		UINT uiThreadID = 0;
		HANDLE hHandle = (HANDLE)::_beginthreadex(nullptr, 0, WorkerThreadFunc, (void*)this, 0, &uiThreadID);
		if (INVALID_HANDLE_VALUE != hHandle)
		{
			CAutoLock lock(&m_csWorkers);
			m_mapWorkers.insert(std::make_pair(uiThreadID, hHandle));
		}
	}

	return TRUE;
}

BOOL CIocpWorker::EndWorkerPool()
{
	ClearWorkerOvLists();
	CloseWorkerHandles();
	return TRUE;
}

BOOL CIocpWorker::DoWorkLoop()
{
	while (TRUE)
	{
		WORKER_OV* pWorkerOv = nullptr;
		WORKER_OV* pFreeWorkerOv = MoveWorkerOvToFree(pWorkerOv);
		if (pWorkerOv)
		{
			OnRequest(pWorkerOv->m_p1, pWorkerOv->m_p2);
		}
		if (pFreeWorkerOv)
		{
			SAFE_DELETE(pFreeWorkerOv->m_p1);
			SAFE_DELETE(pFreeWorkerOv->m_p2);
			SAFE_DELETE(pFreeWorkerOv);
		}
	}
	return FALSE;
}

BOOL CIocpWorker::PutRequestToQueue(DWORD dwSize, DWORD dwKey, void* pParam1, void* pParam2)
{
	WORKER_OV* pWorkerOv = AllocateWorkerOv(pParam1, pParam2);
	return PutDataToQueue(dwSize, dwKey, pWorkerOv);
}

BOOL CIocpWorker::PutDataToQueue(DWORD dwSize, DWORD dwKey, WORKER_OV* pWorkerOv)
{
	CAutoLock lock(&m_csWorkerOvList);
	m_listWorkerOv.push_back(pWorkerOv);
	return TRUE;
}

BOOL CIocpWorker::OnRequest(void* pParam1, void* pParam2)
{
	LPCONTEXT_HEAD lpContext = (LPCONTEXT_HEAD)pParam1;
	LPREQUEST lpRequest = (LPREQUEST)pParam2;

	switch (lpRequest->m_stHead.nRequest)
	{
	case 0:
		break;
	default:
		break;
	}

	SAFE_DELETE(lpRequest->m_pDataPtr);
	return TRUE;
}

WORKER_OV* CIocpWorker::AllocateWorkerOv(void* pParam1, void* pParam2)
{
	WORKER_OV* pWorkerOv = new WORKER_OV(pParam1, pParam2);
	return pWorkerOv;
}

WORKER_OV* CIocpWorker::MoveWorkerOvToFree(WORKER_OV* pWorkerOv)
{
	CAutoLock lock(&m_csWorkerOvList);
	if (m_listWorkerOv.size() > 0)
	{
		pWorkerOv = (WORKER_OV*)m_listWorkerOv.front();
		m_listWorkerOv.pop_front();
		m_nWorkerOvCount--;
	}
	if (nullptr != pWorkerOv)
	{
		m_listFreeWorkerOv.push_back(pWorkerOv);
	}
	WORKER_OV* pFreeWorkOv = nullptr;
	if (m_listFreeWorkerOv.size() > 0)
	{
		pFreeWorkOv = (WORKER_OV*)m_listFreeWorkerOv.front();
		m_listFreeWorkerOv.pop_front();
	}
	else
	{
		m_nFreeWorkerOvCount++;
	}
	return pFreeWorkOv;
}

UINT CIocpWorker::GetWorkerCount()
{
	CAutoLock lock(&m_csWorkers);
	return m_mapWorkers.size();
}

void CIocpWorker::ClearWorkerOvLists()
{
	CAutoLock lock(&m_csWorkerOvList);
	while (m_listWorkerOv.size() > 0)
	{
		WORKER_OV* pWorkerOv = (WORKER_OV*)m_listWorkerOv.front();
		SAFE_DELETE(pWorkerOv);
		m_listWorkerOv.pop_front();
	}
	while (m_listFreeWorkerOv.size() > 0)
	{
		WORKER_OV* pWorkerOv = (WORKER_OV*)m_listFreeWorkerOv.front();
		SAFE_DELETE(pWorkerOv);
		m_listFreeWorkerOv.pop_front();
	}
}

void CIocpWorker::CloseWorkerHandles()
{
	CAutoLock lock(&m_csWorkers);
	std::map<UINT, LPVOID>::iterator iter = m_mapWorkers.begin();
	for (; iter != m_mapWorkers.end(); iter++)
	{
		if (INVALID_HANDLE_VALUE != iter->second)
		{
			::CloseHandle(iter->second);
		}
	}
}

unsigned __stdcall CIocpWorker::WorkerThreadFunc(LPVOID lpParam)
{
	CIocpWorker* pWorker = (CIocpWorker*)lpParam;
	if (pWorker)
	{
		pWorker->DoWorkLoop();
	}
	myLogConsoleI("%s 线程%d退出...", __FUNCTION__, GetCurrentThreadId());
	return THREAD_EXIT;
}

// CIocpServer
CIocpServer::CIocpServer()
{

}

CIocpServer::~CIocpServer()
{

}

BOOL CIocpServer::InitializeMembers()
{
	m_bShutdown = FALSE;

	m_pSocketBufferList = NULL;
	m_pSocketContextList = NULL;
	m_pConnectionContext = NULL;

	m_pListenContext = new CSocketListenContext;
	m_pListenContext->m_hAcceptHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_pListenContext->m_hRepostHandle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	m_nSocketBufferListCount = 0;
	m_nSocketContextListCount = 0;
	m_nConnectionContextCount = 0;

	return TRUE;
}

BOOL CIocpServer::Shutdown()
{
	m_bShutdown = TRUE;

	ClearConnectionContext();

	for (int i = 0; i < m_nWorkerThreads; i++)
	{
		::PostQueuedCompletionStatus(m_hCompletionPort, 0, 0, NULL);
	}
	::WaitForMultipleObjects(m_nWorkerThreads, m_hWorkerHandle, TRUE, 1000 * m_nWorkerThreads);
	for (int i = 0; i < m_nWorkerThreads; i++)
	{
		SAFE_RELEASE_HANDLE(m_hWorkerHandle[i]);
	}

	FreeSocketBuffer();
	FreeSocketContext();

	if (m_pListenContext)
	{
		SAFE_RELEASE_HANDLE(m_pListenContext->m_hAcceptHandle);
		SAFE_RELEASE_HANDLE(m_pListenContext->m_hRepostHandle);
		SAFE_RELEASE_SOCKET(m_pListenContext->m_hSocket);
		SAFE_DELETE(m_pListenContext);
	}

	SAFE_RELEASE_HANDLE(m_hCompletionPort);

	myLogConsoleI("%s 服务关闭", __FUNCTION__);
	//EndWorkerPool();
	return TRUE;
}

BOOL CIocpServer::OnRequest(void* lpParam1, void* lpParam2)
{

	return TRUE;
}

BOOL CIocpServer::Initialize(LPCTSTR lpSzIp, UINT nPort, UINT nInitAccepts, UINT nMaxAccpets, UINT nMaxSocketBufferListCount, UINT nMaxSocketContextListCount, UINT nMaxSendCount, UINT nThreads, UINT nConcurrency, UINT nMaxConnections)
{
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);

	InitializeMembers();
	m_nWorkerThreads = (nThreads <= MAX_WORKER_THREAD_NUMBER) ? nThreads : MAX_WORKER_THREAD_NUMBER;
	m_nConcurrency = nConcurrency;
	//BeginWorkerPool(m_nWorkerThreads, m_nConcurrency);
	m_hCompletionPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, m_nConcurrency);

	m_pListenContext->m_nInitAccpets = nInitAccepts;
	m_nMaxAccepts = nMaxAccpets;
	m_nMaxSocketBufferListCount = nMaxSocketBufferListCount;
	m_nMaxSocketContextListCount = nMaxSocketContextListCount;
	m_nMaxSendCount = nMaxSendCount;
	m_nMaxConnections = nMaxConnections;
	m_nPort = nPort;
	lstrcpy(m_szIp, lpSzIp);

	m_pListenContext->m_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN sock_in;
	memset(&sock_in, 0, sizeof(SOCKADDR_IN));
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = ::ntohs(m_nPort);
	sock_in.sin_addr.S_un.S_addr = INADDR_ANY;
	::bind(m_pListenContext->m_hSocket, (SOCKADDR*)&sock_in, sizeof(sock_in));
	::listen(m_pListenContext->m_hSocket, SOMAXCONN);

	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	DWORD dwBytes;
	::WSAIoctl(m_pListenContext->m_hSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx,
		sizeof(GuidAcceptEx),
		&(m_pListenContext->m_lpfnAcceptEx),
		sizeof(m_pListenContext->m_lpfnAcceptEx),
		&dwBytes,
		NULL,
		NULL);
	GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
	::WSAIoctl(m_pListenContext->m_hSocket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAcceptExSockaddrs,
		sizeof(GuidGetAcceptExSockaddrs),
		&(m_pListenContext->m_lpfnGetAcceptExSockaddrs),
		sizeof(m_pListenContext->m_lpfnGetAcceptExSockaddrs),
		&dwBytes,
		NULL,
		NULL);
	::CreateIoCompletionPort((HANDLE)m_pListenContext->m_hSocket, m_hCompletionPort, (DWORD)0, 0);
	::WSAEventSelect(m_pListenContext->m_hSocket, m_pListenContext->m_hAcceptHandle, FD_ACCEPT);
	m_hAcceptThread = (HANDLE)::_beginthreadex(NULL, 0, AcceptThreadFunc, (void*)this, 0, &m_uiAcceptThread);

	return TRUE;
}

BOOL CIocpServer::PostAccept(CSocketContext* pContext, CSocketBuffer* pBuffer, DWORD& dwWSAError)
{
	pBuffer->m_ioType = enIoAccept;
	DWORD dwBytes = 0;
	pBuffer->m_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	BOOL bRet = m_pListenContext->m_lpfnAcceptEx(
		m_pListenContext->m_hSocket,
		pBuffer->m_hSocket,
		pBuffer->m_pBuffer,
		/*不在Accept的时候接收来自新连接的第一份数据*/
		/*pBuffer->m_nBufferLen - ((sizeof(SOCKADDR_IN) + ACCEPTEX_BYTES_OFFSET_SIZE) * 2)*/0,
		sizeof(SOCKADDR_IN) + ACCEPTEX_BYTES_OFFSET_SIZE,
		sizeof(SOCKADDR_IN) + ACCEPTEX_BYTES_OFFSET_SIZE,
		&dwBytes,
		&(pBuffer->m_ol)
	);
	dwWSAError = ::WSAGetLastError();
	if (!bRet && WSA_IO_PENDING != dwWSAError)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CIocpServer::PostRecv(CSocketContext* pContext, CSocketBuffer* pBuffer, DWORD& dwWSAError)
{
	DWORD dwBytes = 0;
	DWORD dwFlags = 0;
	pBuffer->m_ioType = enIoRead;

	::EnterCriticalSection(&(pContext->m_csLock));
	pBuffer->m_hSocket = pContext->m_hSocket;
	pBuffer->m_nSerialNo = pContext->m_nNextReadSerialNo;
	WSABUF wsaBuf = { 0 };
	wsaBuf.buf = (CHAR*)pBuffer->m_pBuffer;
	wsaBuf.len = pBuffer->m_nBufferLen;
	DWORD dwRet = ::WSARecv(pContext->m_hSocket, &wsaBuf, 1, &dwBytes, &dwFlags, &pBuffer->m_ol, NULL);
	dwWSAError = ::WSAGetLastError();
	if (NO_ERROR != dwRet && WSA_IO_PENDING != dwWSAError)
	{
		::LeaveCriticalSection(&(pContext->m_csLock));
		return FALSE;
	}
	pContext->m_nPostedRecv++;
	pContext->m_nNextReadSerialNo++;
	::LeaveCriticalSection(&(pContext->m_csLock));

	return TRUE;
}

BOOL CIocpServer::PostSend(CSocketContext* pContext, CSocketBuffer* pBuffer, DWORD& dwWSAError)
{
	::EnterCriticalSection(&(pContext->m_csLock));
	if (pContext->m_nPostedSend > m_nMaxSendCount)
	{
		::LeaveCriticalSection(&(pContext->m_csLock));
		return FALSE;
	}

	DWORD dwBytes = 0;
	DWORD dwFlags = 0;
	pBuffer->m_ioType = enIoWrite;
	pBuffer->m_hSocket = pContext->m_hSocket;

	WSABUF wsaBuf = { 0 };
	wsaBuf.buf = (CHAR*)pBuffer->m_pBuffer;
	wsaBuf.len = pBuffer->m_nBufferLen;

	DWORD dwRet = ::WSASend(pContext->m_hSocket, &wsaBuf, 1, &dwBytes, dwFlags, &pBuffer->m_ol, NULL);
	dwWSAError = ::WSAGetLastError();
	if (NO_ERROR != dwRet && WSA_IO_PENDING != dwWSAError)
	{
		::LeaveCriticalSection(&(pContext->m_csLock));
		return FALSE;
	}
	pContext->m_nPostedSend++;
	::LeaveCriticalSection(&(pContext->m_csLock));

	return TRUE;
}

BOOL CIocpServer::OnReceiveData(CSocketContext* pContext, CSocketBuffer* pBuffer, CSessionData& rSessionData)
{
	/*收到数据包，将数据放入缓存区*/
	UINT uiPacketHeadLen = sizeof(PACKET_HEAD);
	::EnterCriticalSection(&(pContext->m_csLock));
	UINT uiDataBufLen = pContext->m_objDataBuf.GetBufferLen();
	if (pBuffer->m_nBufferLen > 0)
	{
		pContext->m_objDataBuf.Write(pBuffer->m_pBuffer, pBuffer->m_nBufferLen);
		uiDataBufLen += pBuffer->m_nBufferLen;
		rSessionData.m_stuHead.uiPacketNo = pContext->m_nPacketNo;
		pContext->m_nPacketNo++;
	}
	while (uiDataBufLen > uiPacketHeadLen)
	{
		/*长度大于包头长度，取得包头数据*/
		LPPACKET_HEAD lpPacketHead = (LPPACKET_HEAD)(PBYTE(pContext->m_objDataBuf.GetBuffer()));
		if (lpPacketHead && lpPacketHead->uiPacketLen <= (uiDataBufLen - uiPacketHeadLen))
		{
			/*数据包进行校验，校验失败，服务端主动关闭套接字*/
			BOOL bVerify = OnVerifyData(pContext, pBuffer, rSessionData);
			if(!bVerify)
			{
				myLogConsoleW("%s 接收数据包校验失败，套接字断开", __FUNCTION__);
				CloseConnectionContext(pContext);
				if (0 == pContext->m_nPostedRecv && 0 == pContext->m_nPostedSend)
				{
					ReleaseSocketContext(pContext);
				}
				ReleaseSocketBuffer(pBuffer);
				break;
			}

			/*数据包处理*/
			OnHandleData(pContext, pBuffer, rSessionData);

			/*缓存区剩下的数据长度大于或等于包头所指的包体长度，取出完整数据包*/
			pContext->m_nSessionID++;
		}
		else
		{
			/*缓存区剩下数据长度小于包头所指的包体长度，无法取出完整数据包，退出*/
			break;
		}
		/*获取当前套接字下缓冲区数据大小*/
		uiDataBufLen = pContext->m_objDataBuf.GetBufferLen();
		if (uiDataBufLen > 0)
		{
			myLogConsoleI("%s 当前套接字下缓冲区数据大小剩余：%d（字节）", __FUNCTION__, uiDataBufLen);
		}
	}
	::LeaveCriticalSection(&(pContext->m_csLock));
	return TRUE;
}

BOOL CIocpServer::OnVerifyData(CSocketContext* pContext, CSocketBuffer* pBuffer, CSessionData& rSessionData)
{
	/*数据包校验（CRC32），校验失败，服务端主动关闭套接字sockte连接*/
	LPPACKET_HEAD lpHead = (LPPACKET_HEAD)(PBYTE(pContext->m_objDataBuf.GetBuffer()));
	/*数据包基本校验（边界校验/包大小校验/数据包类型校验）...*/
	if (!lpHead)
	{
		myLogConsoleW("%s lpHead为空", __FUNCTION__);
		return FALSE;
	}
	if (lpHead->uiPacketLen > BASE_DATA_BUF_SIZE || lpHead->uiPacketLen <= 0)
	{
		myLogConsoleW("%s 数据包的长度%d校验失败", __FUNCTION__, lpHead->uiPacketLen);
		return FALSE;
	}
	if (lpHead->bUseCRC32)
	{
		/*要求校验CRC32，确认收发数据是否一致*/
	}
	return TRUE;
}

BOOL CIocpServer::OnHandleData(CSocketContext* pContext, CSocketBuffer* pBuffer, CSessionData& rSessionData)
{
	/*数据包处理*/
	LPPACKET_HEAD lpHead = (LPPACKET_HEAD)(PBYTE(pContext->m_objDataBuf.GetBuffer()));
	UINT uiFullDataLen = sizeof(PACKET_HEAD) + lpHead->uiPacketLen;
	PBYTE pFullData = new BYTE[uiFullDataLen];
	pContext->m_objDataBuf.Read(pFullData, uiFullDataLen);

	/*将数据封装送入消息处理队列*/
	LPCONTEXT_HEAD lpContext = new CONTEXT_HEAD;
	LPREQUEST lpRequest = new REQUEST;
	lpContext->hSocket = pContext->m_hSocket;
	lpContext->uiTokenID = pContext->m_lToken;
	lpContext->uiSessionID = pContext->m_nSessionID;
	LPREQUEST_HEAD lpRequestHead = (LPREQUEST_HEAD)(PBYTE(pFullData + sizeof(PACKET_HEAD)));
	lpRequest->m_stHead.nRepeated = lpRequestHead->nRepeated;
	lpRequest->m_stHead.nRequest = lpRequestHead->nRequest;
	lpRequest->m_pDataPtr = (void*)(pFullData);
	lpRequest->m_nDataLen = uiFullDataLen;
	PutRequestToQueue(lpRequest->m_nDataLen, pContext->m_hSocket, lpContext, lpRequest);

	return TRUE;
}

BOOL CIocpServer::SendData(CSocketContext* pContext, LPVOID pData, UINT nDataLen)
{
	DWORD dwError = 0;
	CSocketBuffer* pBuffer = AllocateSocketBuffer(nDataLen);
	if (pBuffer)
	{
		memcpy(pBuffer->m_pBuffer, pData, nDataLen);
		return PostSend(pContext, pBuffer, dwError);
	}
	return FALSE;
}

void CIocpServer::HandleIo(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	if (!pBuffer)
	{
		myLogConsoleW("%s pBuffer为空!!!", __FUNCTION__);
		return;
	}
	else
	{
		CSocketContext* pContext = (CSocketContext*)dwKey;
		if (pContext)
		{
			/*更新该套接字上待处理I/O操作计数*/
			::EnterCriticalSection(&(pContext->m_csLock));
			if (enIoWrite == pBuffer->m_ioType)
			{
				pContext->m_nPostedSend--;
			}
			else if (enIoRead == pBuffer->m_ioType)
			{
				pContext->m_nPostedRecv--;
			}
			::LeaveCriticalSection(&(pContext->m_csLock));
			/*检查该套接字是否已经被关闭，关闭了需要释放该套接字上待处理的I/O操作*/
			if (pContext->m_bClose)
			{
				myLogConsoleW("%s 套接字%d已经被关闭", __FUNCTION__, pContext->m_hSocket);
				if (0 == pContext->m_nPostedRecv && 0 == pContext->m_nPostedSend)
				{
					ReleaseSocketContext(pContext);
				}
				ReleaseSocketBuffer(pBuffer);
				return;
			}
		}
		else
		{
			/*新套接字连接建立时，dwKey = 0 dwTrans = 0 dwError = 0，此时pContext会为NULL，更新待处理的连接链表*/
			RemovePendingAccepts(pBuffer);
		}
		/*检查该套接字上是否发生错误*/
		if (NO_ERROR != dwError)
		{
			if (enIoAccept != pBuffer->m_ioType)
			{
				/*套接字发生错误，断开连接，释放相关资源*/
				myLogConsoleW("%s 套接字%d发生错误", __FUNCTION__, pBuffer->m_hSocket);
				CloseConnectionContext(pContext);
				if (0 == pContext->m_nPostedRecv && 0 == pContext->m_nPostedSend)
				{
					ReleaseSocketContext(pContext);
				}
			}
			else
			{
				/*监听的套接字上发生错误，服务端主动关闭监听的该socket套接字*/
				if (INVALID_SOCKET != pBuffer->m_hSocket)
				{
					SAFE_RELEASE_SOCKET(pBuffer->m_hSocket);
				}
			}
		}
		/*分别处理I/O操作*/
		switch (pBuffer->m_ioType)
		{
		case enIoAccept:
			HandleIoAccept(dwKey, pBuffer, dwTrans, dwError);
			break;
		case enIoRead:
			HandleIoRead(dwKey, pBuffer, dwTrans, dwError);
			break;
		case enIoWrite:
			HandleIoWrite(dwKey, pBuffer, dwTrans, dwError);
			break;
		default:
			HandleIoDefault(dwKey, pBuffer, dwTrans, dwError);
			break;
		}
	}
}

void CIocpServer::HandleIoDefault(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	// 暂时不做处理
}

void CIocpServer::HandleIoAccept(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*处理Accept请求*/
	CSocketContext* pContext = AllocateSocketContext(pBuffer->m_hSocket);
	if (pContext)
	{
		BOOL bRet = InsertConnectionContext(pContext);
		if (bRet)
		{
			LPSOCKADDR lpLocalAddr = NULL;
			LPSOCKADDR lpRemoteAddr = NULL;
			int nLocalLen = sizeof(SOCKADDR_IN);
			int nRemoteLen = sizeof(SOCKADDR_IN);

			m_pListenContext->m_lpfnGetAcceptExSockaddrs(
				pBuffer->m_pBuffer,
				/*不在Accept的时候接收来自新连接的第一份数据*/
				/*pBuffer->m_nBufferLen - ((sizeof(SOCKADDR_IN) + ACCEPTEX_BYTES_OFFSET_SIZE) * 2)*/0,
				sizeof(SOCKADDR_IN) + ACCEPTEX_BYTES_OFFSET_SIZE,
				sizeof(SOCKADDR_IN) + ACCEPTEX_BYTES_OFFSET_SIZE,
				(LPSOCKADDR*)&lpLocalAddr,
				&nLocalLen,
				(LPSOCKADDR*)&lpRemoteAddr,
				&nRemoteLen);
			memcpy(&pContext->m_local, lpLocalAddr, nLocalLen);
			memcpy(&pContext->m_remote, lpRemoteAddr, nRemoteLen);
			/*将新建连接的套接字socket和完成端口绑定*/
			::CreateIoCompletionPort((HANDLE)pContext->m_hSocket, m_hCompletionPort, (DWORD)pContext, 0);
			myLogConsoleI("%s:新套接字%d连接建立", __FUNCTION__, pContext->m_hSocket);
			/*为新连接投递一个Read请求*/
			CSocketBuffer* pNewBuffer = AllocateSocketBuffer(DATA_BUF_SIZE);
			if (pNewBuffer)
			{
				DWORD dwError = 0;
				if (!PostRecv(pContext, pNewBuffer, dwError))
				{
					/*投递失败，关闭新连接*/
					CloseConnectionContext(pContext);
					myLogConsoleW("%s 新连接套接字%d投递Read请求失败，断开连接", __FUNCTION__, pContext->m_hSocket);
				}
			}
		}
		else
		{
			/*达到规定最多连接限制，关闭连接，释放其他相关资源*/
			myLogConsoleW("%s 达到规定最多连接限制数%d，关闭连接", __FUNCTION__, m_nMaxConnections);
			CloseConnectionContext(pContext);
			ReleaseSocketContext(pContext);
		}
	}
	else
	{
		/*资源申请失败，关闭该连接*/
		if (INVALID_SOCKET != pBuffer->m_hSocket)
		{
			SAFE_RELEASE_SOCKET(pBuffer->m_hSocket);
		}
	}

	/*Accept I/O操作完成，释放pBuffer*/
	ReleaseSocketBuffer(pBuffer);
	/*通知Accept线程中的m_hRepostHandle事件重新投递一个Accept操作*/
	::InterlockedIncrement(&m_pListenContext->m_nRepostCount);
	::SetEvent(m_pListenContext->m_hRepostHandle);
}

void CIocpServer::HandleIoRead(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*处理Read请求*/
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (pContext)
	{
		if (0 == dwTrans)
		{
			/*套接字断开，释放相关资源*/
			myLogConsoleW("%s 套接字断开", __FUNCTION__);
			CloseConnectionContext(pContext);
			if (0 == pContext->m_nPostedRecv && 0 == pContext->m_nPostedSend)
			{
				ReleaseSocketContext(pContext);
			}
			ReleaseSocketBuffer(pBuffer);
		}
		else
		{
			pBuffer->m_nBufferLen = dwTrans;
			CSocketBuffer* pHandleBuffer = GetSocketBuffer(pContext, pBuffer);
			while (pHandleBuffer)
			{
				/*移动读标记为到下一个CSocketBuffer， 释放完成的CSocketBuffer*/
				::InterlockedIncrement(&pContext->m_nCurrentReadSerialNo);
				CSessionData session;
				OnReceiveData(pContext, pBuffer, session);
				ReleaseSocketBuffer(pHandleBuffer);
				pHandleBuffer = GetSocketBuffer(pContext, NULL);
				myLogConsoleI("%s:读", __FUNCTION__);
			}

			/*投递一个新的Read消息， 投递失败关闭连接*/
			pBuffer = AllocateSocketBuffer(DATA_BUF_SIZE);
			DWORD dwError = 0;
			if (!pBuffer || !PostRecv(pContext, pBuffer, dwError))
			{
				CloseConnectionContext(pContext);
				if (0 == pContext->m_nPostedRecv && 0 == pContext->m_nPostedSend)
				{
					ReleaseSocketContext(pContext);
				}
				ReleaseSocketBuffer(pBuffer);
			}
		}
	}
	else
	{
		myLogConsoleW("%s pContext为空!!!", __FUNCTION__);
		ReleaseSocketBuffer(pBuffer);
	}
}

void CIocpServer::HandleIoWrite(DWORD dwKey, CSocketBuffer* pBuffer, DWORD dwTrans, DWORD dwError)
{
	/*处理Write请求*/
	CSocketContext* pContext = (CSocketContext*)dwKey;
	if (0 == dwTrans)
	{
		/*I/O写操作出现异常，关闭该套接字的连接，释放相关资源*/
		myLogConsoleI("%s I/O写操作出现异常，对方套接字断开", __FUNCTION__);
		CloseConnectionContext(pContext);
		if (0 == pContext->m_nPostedRecv && 0 == pContext->m_nPostedSend)
		{
			ReleaseSocketContext(pContext);
		}
		ReleaseSocketBuffer(pBuffer);
	}
	else
	{
		pBuffer->m_nBufferLen = dwTrans;
		myLogConsoleI("%s 写", __FUNCTION__);
		/*I/O写操作完成，释放CSocketBuffer*/
		ReleaseSocketBuffer(pBuffer);
	}
}

BOOL CIocpServer::InsertPendingAccepts(CSocketContext* pContext, CSocketBuffer* pBuffer)
{
	::EnterCriticalSection(&(m_pListenContext->m_csLock));
	if (!m_pListenContext->m_pAcceptPendingList)
	{
		m_pListenContext->m_pAcceptPendingList = pBuffer;
	}
	else
	{
		pBuffer->m_pNext = m_pListenContext->m_pAcceptPendingList;
		m_pListenContext->m_pAcceptPendingList = pBuffer;
	}
	m_pListenContext->m_nAcceptPendingListCount++;
	::LeaveCriticalSection(&(m_pListenContext->m_csLock));

	return TRUE;
}

BOOL CIocpServer::RemovePendingAccepts(CSocketBuffer* pBuffer)
{
	BOOL bRet = FALSE;

	::EnterCriticalSection(&(m_pListenContext->m_csLock));
	CSocketBuffer* pTemp = m_pListenContext->m_pAcceptPendingList;
	if (pTemp == pBuffer)
	{
		m_pListenContext->m_pAcceptPendingList = pBuffer->m_pNext;
		bRet = TRUE;
	}
	else
	{
		while (pTemp && pTemp->m_pNext != pBuffer)
		{
			pTemp = pTemp->m_pNext;
		}
		if (pTemp)
		{
			pTemp->m_pNext = pBuffer->m_pNext;
			bRet = TRUE;
		}
	}

	if (bRet)
	{
		m_pListenContext->m_nAcceptPendingListCount--;
	}
	::LeaveCriticalSection(&(m_pListenContext->m_csLock));

	return TRUE;
}

BOOL CIocpServer::InsertConnectionContext(CSocketContext* pContext)
{
	/*添加新连接的套接字上下文*/
	CAutoLock lock(&m_csConnectionContext);
	if (m_nConnectionContextCount < m_nMaxConnections)
	{
		pContext->m_pNext = m_pConnectionContext;
		m_pConnectionContext = pContext;
		m_nConnectionContextCount++;
		return TRUE;
	}
	else
	{
		myLogConsoleW("当前已经达到套接字连接上限，禁止连接！");
		return FALSE;
	}
}

BOOL CIocpServer::CloseConnectionContext(CSocketContext* pContext)
{
	/*关闭指定的套接字上下文*/
	{
		CAutoLock lock(&m_csConnectionContext);
		CSocketContext* pTemp = m_pConnectionContext;
		if (pTemp == pContext)
		{
			m_pConnectionContext = pContext->m_pNext;
			m_nConnectionContextCount--;
		}
		else
		{
			while (pTemp && pTemp->m_pNext != pContext)
			{
				pTemp = pTemp->m_pNext;
			}
			if (pTemp)
			{
				pTemp->m_pNext = pContext->m_pNext;
				m_nConnectionContextCount--;
			}
		}
	}
	/*关闭socket套接字*/
	{
		::EnterCriticalSection(&(pContext->m_csLock));
		SAFE_RELEASE_SOCKET(pContext->m_hSocket);
		pContext->m_bClose = TRUE;
		::LeaveCriticalSection(&(pContext->m_csLock));
	}

	return TRUE;
}

BOOL CIocpServer::ClearConnectionContext()
{
	/*释放当前所有连接套接字上下文*/
	CAutoLock lock(&m_csConnectionContext);
	CSocketContext* pContext = m_pConnectionContext;
	while (pContext)
	{
		::EnterCriticalSection(&(pContext->m_csLock));
		SAFE_RELEASE_SOCKET(pContext->m_hSocket);
		pContext->m_bClose = TRUE;
		pContext = pContext->m_pNext;
		::LeaveCriticalSection(&(pContext->m_csLock));
	}

	m_pConnectionContext = NULL;
	m_nConnectionContextCount = 0;

	return TRUE;
}

CSocketBuffer* CIocpServer::AllocateSocketBuffer(UINT nBufferLen)
{
	/*申请I/O操作结构，优先在空闲I/O操作结构列表中取*/
	CSocketBuffer* pBuffer = NULL;
	if (nBufferLen <= 0 || nBufferLen > DATA_BUF_SIZE)
	{
		return NULL;
	}

	CAutoLock lock(&m_csSocketBufferList);
	if (m_pSocketBufferList)
	{
		pBuffer = m_pSocketBufferList;
		m_pSocketBufferList = m_pSocketBufferList->m_pNext;
		pBuffer->m_pNext = NULL;
		m_nSocketBufferListCount--;
	}
	else
	{
		pBuffer = (CSocketBuffer*)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CSocketBuffer) + DATA_BUF_SIZE);
	}

	if (pBuffer)
	{
		/*I/O缓存区设置*/
		pBuffer->m_pBuffer = (PBYTE)(pBuffer + 1);
		pBuffer->m_nBufferLen = nBufferLen;
	}

	return pBuffer;
}

void CIocpServer::ReleaseSocketBuffer(CSocketBuffer* pBuffer)
{
	/*释放I/O操作结构，添加到空闲I/O操作结构列表中用以重复利用*/
	CAutoLock lock(&m_csSocketBufferList);
	if (m_nSocketBufferListCount <= m_nMaxSocketBufferListCount)
	{
		memset(pBuffer, 0, sizeof(CSocketBuffer) + DATA_BUF_SIZE);
		pBuffer->m_pNext = m_pSocketBufferList;
		m_pSocketBufferList = pBuffer;
		m_nSocketBufferListCount++;
	}
	else
	{
		::HeapFree(::GetProcessHeap(), 0, pBuffer);
	}
}

void CIocpServer::FreeSocketBuffer()
{
	/*释放空闲I/O操作结构列表*/
	CAutoLock lock(&m_csSocketBufferList);
	CSocketBuffer* pTemp = m_pSocketBufferList;
	CSocketBuffer* pNext;
	while (pTemp)
	{
		pNext = pTemp->m_pNext;
		::HeapFree(::GetProcessHeap(), 0, pTemp);
		pTemp = pNext;
	}
	m_pSocketBufferList = NULL;
	m_nSocketBufferListCount = 0;
}

CSocketContext* CIocpServer::AllocateSocketContext(SOCKET socket)
{
	/*申请套接字上下文，优先从空闲套接字上下文列表中取，没有再申请内存*/
	CSocketContext* pContext = NULL;
	CAutoLock lock(&m_csSocketContextList);
	if (m_pSocketContextList)
	{
		pContext = m_pSocketContextList;
		m_pSocketContextList = m_pSocketContextList->m_pNext;
		pContext->m_pNext = NULL;
		m_nSocketContextListCount--;
	}
	else
	{
		pContext = (CSocketContext*)::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CSocketContext));
		::InitializeCriticalSection(&(pContext->m_csLock));
	}
	/*给申请好的套接字上下文设置socket套接字*/
	if (pContext)
	{
		pContext->m_hSocket = socket;
	}

	return pContext;
}

void CIocpServer::ReleaseSocketContext(CSocketContext* pContext)
{
	/*关闭该套接字的socket*/
	SAFE_RELEASE_SOCKET(pContext->m_hSocket);
	/*释放该套接字上待处理的I/O操作*/
	CSocketBuffer* pTemp = NULL;
	while (pContext->m_pWaitingRecv)
	{
		pTemp = pContext->m_pWaitingRecv->m_pNext;
		ReleaseSocketBuffer(pContext->m_pWaitingRecv);
		pContext->m_pWaitingRecv = pTemp;
	}
	while (pContext->m_pWaitingSend)
	{
		pTemp = pContext->m_pWaitingSend->m_pNext;
		ReleaseSocketBuffer(pContext->m_pWaitingSend);
		pContext->m_pWaitingSend = pTemp;
	}
	/*将释放掉的套接字添加到空闲列表重新利用*/
	CAutoLock lock(&m_csSocketContextList);
	if (m_nSocketContextListCount <= m_nMaxSocketContextListCount)
	{
		CRITICAL_SECTION csLock = pContext->m_csLock;
		memset(pContext, 0, sizeof(CSocketContext));
		pContext->m_csLock = csLock;
		pContext->m_pNext = m_pSocketContextList;
		m_pSocketContextList = pContext;
		m_nSocketContextListCount++;
	}
	else
	{
		::DeleteCriticalSection(&(pContext->m_csLock));
		::HeapFree(::GetProcessHeap(), 0, pContext);
	}
}

void CIocpServer::FreeSocketContext()
{
	/*释放空闲套接字上下文列表*/
	CAutoLock lock(&m_csSocketContextList);
	CSocketContext* pTemp = m_pSocketContextList;
	CSocketContext* pNext;
	while (pTemp)
	{
		pNext = pTemp->m_pNext;
		::DeleteCriticalSection(&(pTemp->m_csLock));
		::HeapFree(::GetProcessHeap(), 0, pTemp);
		pTemp = pNext;
	}

	m_pSocketContextList = NULL;
	m_nSocketContextListCount = 0;
}

CSocketBuffer* CIocpServer::GetSocketBuffer(CSocketContext* pContext, CSocketBuffer* pBuffer)
{
	/*按顺序获得当前应该读取的I/O操作缓冲区，对于一个已经建立了连接的套接字，该套接字所拥有的所有I/O操作结构应该是先后顺序*/
	if (pBuffer)
	{
		/*该套接字下一个读的I/O操作的序列号与当前I/O操作的序列号相同，直接读取该I/O操作的数据缓存区*/
		::EnterCriticalSection(&(pContext->m_csLock));
		if (pBuffer->m_nSerialNo == pContext->m_nCurrentReadSerialNo)
		{
			::LeaveCriticalSection(&(pContext->m_csLock));
			return pBuffer;
		}
		/*否则代表乱序了，将当前I/O操作按顺序排好序*/
		pBuffer->m_pNext = NULL;
		CSocketBuffer* pHead = pContext->m_pWaitingRecv;
		CSocketBuffer* pTemp = NULL;
		while (pHead)
		{
			if (pBuffer->m_nSerialNo > pHead->m_nSerialNo)
			{
				break;
			}
			pTemp = pHead;
			pHead = pHead->m_pNext;
		}
		if (!pTemp)
		{
			/*pBuffer应该插在表头*/
			pBuffer->m_pNext = pContext->m_pWaitingRecv;
			pContext->m_pWaitingRecv = pBuffer;
		}
		else
		{
			/*pBuffer应该插在表中*/
			pBuffer->m_pNext = pTemp->m_pNext;
			pTemp->m_pNext = pBuffer;
		}
		::LeaveCriticalSection(&(pContext->m_csLock));
	}
	/*取得当前按顺序应该取到的I/O操作的数据缓存区*/
	CSocketBuffer* pRet = pContext->m_pWaitingRecv;
	if (pRet && (pRet->m_nSerialNo == pContext->m_nCurrentReadSerialNo))
	{
		pContext->m_pWaitingRecv = pRet->m_pNext;
		return pRet;
	}
	return NULL;
}

unsigned __stdcall CIocpServer::AcceptThreadFunc(LPVOID lpParam)
{
	CIocpServer* pIocpServer = (CIocpServer*)lpParam;
	CSocketBuffer* pBuffer = NULL;
	DWORD dwError = 0;
	// 最开始先投递初始数量的Accept请求
	for (int i = 0; i < pIocpServer->m_pListenContext->m_nInitAccpets; i++)
	{
		pBuffer = pIocpServer->AllocateSocketBuffer(DATA_BUF_SIZE);
		if (!pBuffer)
		{
			myLogConsoleI("%s 线程%d退出...", __FUNCTION__, GetCurrentThreadId());
			return THREAD_EXIT;
		}
		pIocpServer->InsertPendingAccepts(NULL, pBuffer);
		pIocpServer->PostAccept(NULL, pBuffer, dwError);
	}

	// 创建工作线程，推荐线程数：处理器数*2
	for (int i = 0; i < pIocpServer->m_nWorkerThreads; i++)
	{
		unsigned int uiWorkerThread = 0;
		pIocpServer->m_hWorkerHandle[i] = (HANDLE)::_beginthreadex(NULL, 0, WorkerThreadFunc, (void*)pIocpServer, 0, &uiWorkerThread);
		if (INVALID_HANDLE_VALUE == pIocpServer->m_hWorkerHandle[i])
		{
			myLogConsoleE("工作线程创建失败...");
			return THREAD_EXIT;
		}
	}

	int nEventCount = 0;
	pIocpServer->m_hWaitHandle[nEventCount++] = pIocpServer->m_pListenContext->m_hAcceptHandle;
	pIocpServer->m_hWaitHandle[nEventCount++] = pIocpServer->m_pListenContext->m_hRepostHandle;

	while (TRUE)
	{
		/*检测服务关闭，退出线程*/
		if (pIocpServer->m_bShutdown)
		{
			myLogConsoleI("%s Accept线程%d退出...", __FUNCTION__, GetCurrentThreadId());
			return THREAD_EXIT;
		}
		/*定时对m_hWaitHandle进行监听，在预留的已经投递AcceptEx句柄即将用完之前及时添加*/
		DWORD dwWaitRet = ::WSAWaitForMultipleEvents(nEventCount, pIocpServer->m_hWaitHandle, FALSE, 1000, FALSE);
		/*监听返回失败*/
		if (WSA_WAIT_FAILED == dwWaitRet)
		{
			myLogConsoleI("%s Accept线程%d退出...", __FUNCTION__, GetCurrentThreadId());
			pIocpServer->Shutdown();
			return THREAD_EXIT;
		}
		/*定时检查当前挂起的所有AcceptEx I/O建立时长*/
		else if (WSA_WAIT_TIMEOUT == dwWaitRet)
		{
			pBuffer = pIocpServer->m_pListenContext->m_pAcceptPendingList;
			while (pBuffer)
			{
				int nTimes = 0;
				int nTimesLen = sizeof(int);
				/*获取socket连接建立时长，时间长的话直接断开*/
				::getsockopt(pBuffer->m_hSocket, SOL_SOCKET, SO_CONNECT_TIME, (char*)&nTimes, &nTimesLen);
				if (-1 != nTimes && nTimes > 2)
				{
					SAFE_RELEASE_SOCKET(pBuffer->m_hSocket);
				}
				pBuffer = pBuffer->m_pNext;
			}
		}
		/*查询Accept事件处理*/
		else
		{
			/*dwWaitRet返回值区间位于[WSA_WAIT_EVENT_0， (WSA_WAIT_EVENT_0+ nEventCount - 1)]，对应索引，超出代表发生错误*/
			dwWaitRet = dwWaitRet - WSA_WAIT_EVENT_0;
			int nAddAcceptCounts = 0;
			/*对应m_hAcceptHandle事件*/
			if (0 == dwWaitRet)
			{
				WSANETWORKEVENTS wsaNetEvent;
				::WSAEnumNetworkEvents(pIocpServer->m_pListenContext->m_hSocket, pIocpServer->m_hWaitHandle[dwWaitRet], &wsaNetEvent);
				/*发送FD_ACCEPT网络事件，意味着投递的Accept不够，需要增加*/
				if (FD_ACCEPT & wsaNetEvent.lNetworkEvents)
				{
					nAddAcceptCounts = ADD_ACCEPT_COUNT;
				}
			}
			else if (1 == dwWaitRet)
			{
				nAddAcceptCounts = ::InterlockedExchange(&pIocpServer->m_pListenContext->m_nRepostCount, 0);
			}
			/*网络事件索引超出，发生错误，关闭服务*/
			else if (dwWaitRet >= nEventCount)
			{
				pIocpServer->Shutdown();
				myLogConsoleI("%s Accept线程%d退出...", __FUNCTION__, GetCurrentThreadId());
				return THREAD_EXIT;
			}
			/*增加Accept投递*/
			int nCount = 0;
			while ((nCount++ < nAddAcceptCounts) && (pIocpServer->m_pListenContext->m_nAcceptPendingListCount < pIocpServer->m_nMaxAccepts))
			{
				pBuffer = pIocpServer->AllocateSocketBuffer(DATA_BUF_SIZE);
				if (pBuffer)
				{
					pIocpServer->InsertPendingAccepts(NULL, pBuffer);
					pIocpServer->PostAccept(NULL, pBuffer, dwError);
				}
			}
		}
	}
	myLogConsoleI("%s Accept线程%d退出...", __FUNCTION__, GetCurrentThreadId());
	return THREAD_EXIT;
}

unsigned __stdcall CIocpServer::WorkerThreadFunc(LPVOID lpParam)
{
	CIocpServer* pIocpServer = (CIocpServer*)lpParam;
	CSocketBuffer* pBuffer = NULL;
	DWORD dwKey = 0;
	DWORD dwTrans = 0;
	LPOVERLAPPED lpOverlapped = NULL;
	while (TRUE)
	{
		/*获取完成端口队列数据*/
		BOOL bRet = ::GetQueuedCompletionStatus(pIocpServer->m_hCompletionPort, &dwTrans, (PULONG_PTR)&dwKey, &lpOverlapped, WSA_INFINITE);
		if (dwTrans < 0 || !lpOverlapped)
		{
			myLogConsoleW("%s 工作线程%d退出", __FUNCTION__, GetCurrentThreadId());
			return THREAD_EXIT;
		}
		/*取出对应lpOverlapped的CSocketBuffer进行I/O操作*/
		pBuffer = CONTAINING_RECORD(lpOverlapped, CSocketBuffer, m_ol);
		DWORD dwError = NO_ERROR;
		DWORD dwFlags = 0;
		if (!bRet)
		{
			/*处理GetQueuedCompletionStatus返回错误*/
			SOCKET socket = INVALID_SOCKET;
			if (pBuffer)
			{
				if (enIoAccept == pBuffer->m_ioType)
				{
					socket = pIocpServer->m_pListenContext->m_hSocket;
				}
				else
				{
					if (0 == dwKey)
					{
						myLogConsoleW("%s 工作线程%d退出", __FUNCTION__, GetCurrentThreadId());
						return THREAD_EXIT;
					}
					socket = ((CSocketContext*)dwKey)->m_hSocket;
				}
				/*查询该套接口上一个重叠操作的操作结果*/
				BOOL bResult = ::WSAGetOverlappedResult(socket, &(pBuffer->m_ol), &dwError, FALSE, &dwFlags);
				if (!bResult)
				{
					/*重叠操作失败*/
					dwError = ::WSAGetLastError();
					myLogConsoleW("%s WSAGetOverlappedResult返回失败", __FUNCTION__);
					myLogConsoleW("%s GetQueuedCompletionStatus返回False，错误码dwError：%d", __FUNCTION__, dwError);
				}
			}
			else
			{
				myLogConsoleW("%s pBuffer为空!!!");
				continue;
			}
		}
		/*处理I/O操作*/
		pIocpServer->HandleIo(dwKey, pBuffer, dwTrans, dwError);
	}
	myLogConsoleI("%s 工作线程%d退出...", __FUNCTION__, GetCurrentThreadId());
	return THREAD_EXIT;
}