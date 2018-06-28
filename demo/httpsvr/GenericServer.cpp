/****************************************************************************************
* ///////////////////////////////////////////////////////////////////////////////////////
*	Original Filename: 	genericserver.cpp
*
*	History:
*	Created/Modified by				Date			Main Purpose/Changes
*	Souren M. Abeghyan				2001/05/25		Implementation of the CGenericServer class.
*	
*	Comments:	
* \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
****************************************************************************************/

#include "stdafx.h"
#include "GenericServer.h"

#pragma comment(lib,"ws2_32.lib")

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGenericServer::CGenericServer()
{
	m_hRunMutex = 0;
}

CGenericServer::~CGenericServer()
{
}




void CGenericServer::GetStats(StatisticsTag &st)
{
	st.nTotalRecv = Stats.nTotalRecv;
	st.nTotalSent = Stats.nTotalSent;
	st.nTotalHits = Stats.nTotalHits;
	st.nVisitors  = Visitors.size();
	EnterCriticalSection(&_cs);
	st.nClientsConnected = ThreadList.size();
	LeaveCriticalSection(&_cs);
}



BOOL CGenericServer::AddClient(SOCKET s, char* ClientAddress, int port)
{
	GotConnection(ClientAddress, port);

	STRVECT::iterator it;
	it = find(Visitors.begin(), Visitors.end(), ClientAddress);
	if(it == Visitors.end())
		Visitors.push_back(ClientAddress);

	InterlockedIncrement(&Stats.nTotalHits);
	
	ThreadTag		Thread;
	HANDLE			hThread;
	unsigned int	threadID;

	EnterCriticalSection(&cs);
	NewConnectionTag *NewConn	= new NewConnectionTag;
	NewConn->pGenericServer		= this;
	NewConn->s					= s;
	hThread = (HANDLE)_beginthreadex(NULL, 0, ClientThread, NewConn, 0, &threadID);
	if(hThread)
	{
		Thread.threadID = threadID;
		Thread.hThread = hThread;
		ThreadList.push_back(Thread);
	}
	else
		LogMessage(LOGFILENAME, _T("_beginthreadex(...) failure"), _T("AddClient"), errno);
	LeaveCriticalSection(&cs);

	return TRUE;
}



BOOL CGenericServer::Run(int Port, int PersTO)
{
	m_hRunMutex = CreateMutex(NULL, TRUE, _T("name.mutex.generic.server"));
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(m_hRunMutex);
		m_hRunMutex = 0;
		LogMessage(LOGFILENAME, _T("_beginthreadex(...) failure, for Launch Thread"), _T("Run"), errno);
		return FALSE;
	}
	
	ServerPort = Port;
	PersistenceTO = PersTO;

	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&_cs);
	
	Reset();
	
	ThreadLaunchedEvent	= CreateEvent(NULL, FALSE, TRUE, NULL);

	// Launch Accept Thread
	ResetEvent(ThreadLaunchedEvent);
	ThreadA = (HANDLE)_beginthreadex(NULL, 0, AcceptThread, this, 0, &ThreadA_ID);
	if(!ThreadA)
	{
		LogMessage(LOGFILENAME, _T("_beginthreadex(...) failure, for Launch Thread"), _T("Run"), errno);
		return FALSE;
	}

	if(WaitForSingleObject(ThreadLaunchedEvent, THREADWAIT_TO) != WAIT_OBJECT_0)
	{
		LogMessage(LOGFILENAME, _T("Unable to get response from Accept Thread withing specified Timeout ->"), _T("Run"), THREADWAIT_TO);
		CloseHandle(ThreadLaunchedEvent);
		return FALSE;
	}

	// Launch Helper Thread
	ResetEvent(ThreadLaunchedEvent);
	ThreadC = (HANDLE)_beginthreadex(NULL, 0, HelperThread, this, 0, &ThreadC_ID);
	if(!ThreadC)
	{
		LogMessage(LOGFILENAME, _T("_beginthreadex(...) failure, for Helper Thread"), _T("Run"), errno);
		return FALSE;
	}
	
	if(WaitForSingleObject(ThreadLaunchedEvent, THREADWAIT_TO) != WAIT_OBJECT_0)
	{
		LogMessage(LOGFILENAME, _T("Unable to get response from Helper Thread within specified Timeout ->"), _T("Run"), THREADWAIT_TO);
		CloseHandle(ThreadLaunchedEvent);
		return FALSE;
	}
	
	CloseHandle(ThreadLaunchedEvent);
	

	return TRUE;
}



BOOL CGenericServer::Shutdown()
{
	if (!m_hRunMutex)
		return FALSE;
	
	BOOL bResult = TRUE;
	HANDLE	hArray[2];

	hArray[0] = ThreadA;
	hArray[1] = ThreadC;

	//
	// Killing Accept and Helper threads
	//
	SetEvent(ShutdownEvent);
	DWORD n = WaitForMultipleObjects(2, hArray, TRUE, THREADKILL_TO);

	if(n == WAIT_TIMEOUT || n == WAIT_FAILED)
	{
		LogMessage(LOGFILENAME, _T("WaitForMultipleObjects(...) timed out"), _T("Shutdown"));
		if(!TerminateThread(ThreadA, THREADEXIT_SUCCESS))
			LogMessage(LOGFILENAME, _T("TerminateThread(.ThreadA.) failure, probably it is already terminated"), _T("Shutdown"), GetLastError());
		if(!TerminateThread(ThreadC, THREADEXIT_SUCCESS))
			LogMessage(LOGFILENAME, _T("TerminateThread(.ThreadC.) failure, probably it is already terminated"), _T("Shutdown"), GetLastError());
		bResult = FALSE;
	}

	CloseHandle(ThreadA); 
	CloseHandle(ThreadC); 

	//
	// Make sure all client threads are dead
	//
	THREADLIST::iterator it;
	while(ThreadList.size())
	{
		Sleep(100);
	}
	
	DeleteCriticalSection(&cs);
	DeleteCriticalSection(&_cs);

	CloseHandle(m_hRunMutex);
	m_hRunMutex = 0;
	return bResult;
}



void CGenericServer::Reset()
{
	//
	// Reset statistic values
	//
	Stats.nClientsConnected		= 0;
	Stats.nErrosCount			= 0;
	Stats.nTotalSent			= 0;
	Stats.nTotalRecv			= 0;
	Stats.nTotalHits			= 0;
	Stats.nVisitors				= 0;
}




void CGenericServer::CleanupThread(WSAEVENT Event, SOCKET s, NewConnectionTag* pNewConn, DWORD dwThreadID)
{
	if(Event)
		WSACloseEvent(Event);
	closesocket(s);
	EnterCriticalSection(&cs);
	delete pNewConn;
	LeaveCriticalSection(&cs);
	
	THREADLIST::iterator it;
	it = find_if(ThreadList.begin(), ThreadList.end(), bind2nd(IDCompare(), dwThreadID));
	if(it != ThreadList.end())
	{
		EnterCriticalSection(&_cs);
		HandleList.push_back((*it).hThread);
		ThreadList.erase(it);
		LeaveCriticalSection(&_cs);
	}
	else
		LogMessage(LOGFILENAME, _T("Thread not found in the list"), _T("ClientThread"));
}




void CGenericServer::CleanupThread(WSAEVENT Event, WSAEVENT ShutdownEvent, SOCKET s)
{
	if(Event)
		WSACloseEvent(Event);

	if(ShutdownEvent)
		WSACloseEvent(ShutdownEvent);

	if(s)
		closesocket(s);

	WSACleanup();
}




UINT __stdcall CGenericServer::AcceptThread(LPVOID pParam)
{
	CGenericServer *pGenericServer = (CGenericServer*)pParam;
	SOCKET s; // Main Listen Socket
	WORD wVersionRequested;
	WSADATA wsaData;
	sockaddr_in saLocal;
	WSAEVENT Handles[2];
	WSANETWORKEVENTS	NetworkEvents;
	sockaddr ClientAddr;
	INT addrlen = sizeof(ClientAddr);
	sockaddr_in sain;
	char cAddr[50];
	int result;
	
	saLocal.sin_family		= AF_INET;
	saLocal.sin_port		= htons(pGenericServer->ServerPort);
	saLocal.sin_addr.s_addr = INADDR_ANY;
	
	wVersionRequested = MAKEWORD(2, 2);
	
	result = WSAStartup(wVersionRequested, &wsaData);
	if(result != 0)
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("WSAStartup(...) failure"), _T("AcceptThread"), result);
		return THREADEXIT_SUCCESS;
	}
	
	if(	LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2) 
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("Requested Socket version not exist"), _T("AcceptThread"));
		pGenericServer->CleanupThread(NULL, NULL, NULL);
		return THREADEXIT_SUCCESS; 
	}
	
	s = WSASocket(AF_INET, SOCK_STREAM, 0, (LPWSAPROTOCOL_INFO)NULL, 0, WSA_FLAG_OVERLAPPED);
	if(s == INVALID_SOCKET)
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("WSASocket(...) failure"), _T("AcceptThread"), WSAGetLastError());
		pGenericServer->CleanupThread(NULL, NULL, NULL);
		return THREADEXIT_SUCCESS;
	}
	
	//
	//	B I N D
	//
	result = ::bind(s, (struct sockaddr *)&saLocal, sizeof(saLocal));
	if(result == SOCKET_ERROR)
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("bind(...) failure"), _T("AcceptThread"), WSAGetLastError());
		pGenericServer->CleanupThread(NULL, NULL, s);
		return THREADEXIT_SUCCESS;
	}	

	//
	//	L I S T E N
	//
	result = ::listen(s, SOMAXCONN);
	if(result == SOCKET_ERROR)
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("listen(...) failure"), _T("AcceptThread"), WSAGetLastError());
		pGenericServer->CleanupThread(NULL, NULL, s);
		return THREADEXIT_SUCCESS;
	}	
	
 	pGenericServer->ShutdownEvent = WSACreateEvent();
	if(pGenericServer->ShutdownEvent == WSA_INVALID_EVENT)
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("WSACreateEvent(...) failure for ShutdownEvent"), _T("AcceptThread"), WSAGetLastError());
		pGenericServer->CleanupThread(NULL, NULL, NULL, s);
		return THREADEXIT_SUCCESS;
	}		

	WSAEVENT Event = WSACreateEvent();
	if(Event == WSA_INVALID_EVENT)
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("WSACreateEvent(...) failure for Event"), _T("AcceptThread"), WSAGetLastError());
		pGenericServer->CleanupThread(NULL, pGenericServer->ShutdownEvent, s);
		return THREADEXIT_SUCCESS;
	}		
	
	Handles[0] = pGenericServer->ShutdownEvent;
	Handles[1] = Event;

	result = WSAEventSelect(s, Event, FD_ACCEPT);
	if(result == SOCKET_ERROR)
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("WSAEventSelect(...) failure"), _T("AcceptThread"), WSAGetLastError());
		pGenericServer->CleanupThread(Event, pGenericServer->ShutdownEvent, s);
		return THREADEXIT_SUCCESS;
	}

	SetEvent(pGenericServer->ThreadLaunchedEvent);
	
	for(;;)
	{
		DWORD EventCaused = WSAWaitForMultipleEvents(
			2,
			Handles,  
			FALSE,                  
			WSA_INFINITE, 
			FALSE);

		if(EventCaused == WAIT_FAILED || EventCaused == WAIT_OBJECT_0)
		{
			if(EventCaused == WAIT_FAILED)
				pGenericServer->LogMessage(LOGFILENAME, _T("WaitForMultipleObjects(...) failure"), _T("AcceptThread"), GetLastError());
			pGenericServer->CleanupThread(Event, pGenericServer->ShutdownEvent, s);
			return THREADEXIT_SUCCESS;
		}

		result = WSAEnumNetworkEvents(
			s,                           
			Event,              
			&NetworkEvents);
		
		if(result == SOCKET_ERROR)						 
		{
			pGenericServer->LogMessage(LOGFILENAME, _T("WSAEnumNetworkEvents(...) failure"), _T("AcceptThread"), WSAGetLastError());
			pGenericServer->CleanupThread(Event, pGenericServer->ShutdownEvent, s);
			return THREADEXIT_SUCCESS;
		}

		if(NetworkEvents.lNetworkEvents == FD_ACCEPT)
		{
			SOCKET ClientSocket = WSAAccept(s, &ClientAddr, &addrlen, NULL, NULL);
			memcpy(&sain, &ClientAddr, addrlen);
			sprintf(cAddr, "%d.%d.%d.%d", 
				sain.sin_addr.S_un.S_un_b.s_b1, 
				sain.sin_addr.S_un.S_un_b.s_b2, 
				sain.sin_addr.S_un.S_un_b.s_b3, 
				sain.sin_addr.S_un.S_un_b.s_b4);

			if(INVALID_SOCKET == ClientSocket)
			{
				pGenericServer->LogMessage(LOGFILENAME, _T("WSAAccept(...) failure"), _T("AcceptThread"), WSAGetLastError());
				// I think there is no reason to shutdown whole server if just one connection failed
				continue; 
			}
			else
			{
				if(!pGenericServer->AddClient(ClientSocket, cAddr, sain.sin_port))
				{
					pGenericServer->LogMessage(LOGFILENAME, _T("AddClient(...) failure"), _T("AcceptThread"));
					continue; // I think there is no reason to shutdown whole server if just one connection failed
				}
			}
		}
	}

	pGenericServer->CleanupThread(Event, pGenericServer->ShutdownEvent, s);
	return THREADEXIT_SUCCESS; 
}	


									  




unsigned __stdcall CGenericServer::ClientThread(LPVOID pParam)
{
	NewConnectionTag *pNewConn = (NewConnectionTag*)pParam;
	CGenericServer *pGenericServer = pNewConn->pGenericServer;
	SOCKET s = pNewConn->s;

	int					result;
	WSAEVENT			EventArray[2];
	WSANETWORKEVENTS	NetworkEvents;

	BOOL				bResend = FALSE;
	WSABUF				Buffer;
	DWORD				NumberOfBytesSent;
	DWORD				dwBytesSent;
	BOOL				bKeepAlive = FALSE;
	string				szRequest;
	string				szResponse;
				
	WSAEVENT			Event = WSACreateEvent();

	if(Event == WSA_INVALID_EVENT)
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("WSACreateEvent(...) failure"), _T("ClientThread"), WSAGetLastError());
		pGenericServer->CleanupThread(NULL, s, pNewConn, GetCurrentThreadId());
		return THREADEXIT_SUCCESS;
	}

	result = WSAEventSelect(s, Event, FD_READ | FD_WRITE | FD_CLOSE);
	if(result == SOCKET_ERROR)
	{
		pGenericServer->LogMessage(LOGFILENAME, _T("WSAEventSelect(...) failure"), _T("ClientThread"), WSAGetLastError());
		pGenericServer->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
		return THREADEXIT_SUCCESS;
	}
	
	EventArray[0] = Event;
	EventArray[1] = pGenericServer->ShutdownEvent;

	for(;;)
	{
		DWORD EventCaused = WSAWaitForMultipleEvents(
			2,
			EventArray,  
			FALSE,                  
			pGenericServer->PersistenceTO ? pGenericServer->PersistenceTO : WSA_INFINITE, 
			FALSE);
		
		if(WSA_WAIT_FAILED == EventCaused)
		{
			pGenericServer->LogMessage(LOGFILENAME, _T("WSAWaitForMultipleEvents(...) failure"), _T("ClientThread"), WSAGetLastError());
			pGenericServer->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
			return THREADEXIT_SUCCESS;
		}

		//
		// Check if our Server mission is over or Client established connection, but no request so far
		//
		if(EventCaused == 1 || EventCaused == WSA_WAIT_TIMEOUT)
		{
			pGenericServer->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
			return THREADEXIT_SUCCESS;
		}
		
		//
		//	Determine which network event occured 
		//
		result = WSAEnumNetworkEvents(
			s,                           
			Event,              
			&NetworkEvents);

		if(result == SOCKET_ERROR)
		{
			pGenericServer->LogMessage(LOGFILENAME, _T("WSAEnumNetworkEvents(...) failure"), _T("ClientThread"), WSAGetLastError());
			continue; 
		}
		
		//
		// No idea where zero came from...
		//
		if(!NetworkEvents.lNetworkEvents)
			continue;

		//
		// Process Events
		// 
		if(NetworkEvents.lNetworkEvents & FD_READ)
		{
			//
			//	Do not collect Incomming data, just pass it away to inherited class
			//
			DWORD NumberOfBytesRecvd;
			WSABUF Buffers;
			DWORD dwBufferCount = 1;
			char szBuffer[MAX_BUFFER];
			DWORD Flags = 0;
			Buffers.buf = szBuffer;
			Buffers.len = MAX_BUFFER;
			
			result = WSARecv(
				s,
				&Buffers,
				dwBufferCount,
				&NumberOfBytesRecvd,
				&Flags,
				NULL,
				NULL);
			
			if(result != SOCKET_ERROR)
			{
				pGenericServer->Stats.nTotalRecv += (double)NumberOfBytesRecvd / 1024;

				//
				// Chech if we got complete request
				//
				szRequest += string(szBuffer, NumberOfBytesRecvd);
				if(!pGenericServer->IsComplete(szRequest))
					continue;

				if(!pGenericServer->ParseRequest(szRequest, szResponse, bKeepAlive))
				{
					pGenericServer->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
					return THREADEXIT_SUCCESS;
				}

				//
				// Send Response to client
				//
				NumberOfBytesSent = 0;
				dwBytesSent = 0;
				do
				{
					Buffer.len = (szResponse.size() - dwBytesSent) >= SENDBLOCK ? SENDBLOCK : szResponse.size() - dwBytesSent;	
					Buffer.buf = (char*)((DWORD)szResponse.c_str() + dwBytesSent);

					result = WSASend(
						s,                                                
						&Buffer,                                     
						1,                                    
						&NumberOfBytesSent,
						0,                                          
						0,                           
						NULL);
					
					if(SOCKET_ERROR != result)
						dwBytesSent += NumberOfBytesSent;
				}
				while((dwBytesSent < szResponse.size()) && SOCKET_ERROR != result);

				if(WSAGetLastError() == WSAEWOULDBLOCK)
				{
					bResend = TRUE;
					continue;
				}

				if(SOCKET_ERROR != result)
				{
					pGenericServer->Stats.nTotalSent += (double)dwBytesSent / 1024;
					pGenericServer->DataSent(dwBytesSent);
				}
				else
				{
					pGenericServer->LogMessage(LOGFILENAME, _T("WSASend(...) failure"), _T("ClientThread, Primary Send"), WSAGetLastError());
					bKeepAlive = FALSE;
				}
				
				if(!bKeepAlive) 
				{
					pGenericServer->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
					return THREADEXIT_SUCCESS;
				}

				//
				// We are finished with this request, clear buffer for next one
				//
				szRequest.erase(0, string::npos);
			}
			else
				pGenericServer->LogMessage(LOGFILENAME, _T("WSARecv(...) failure"), _T("ClientThread"), WSAGetLastError());
		}
		
		
		if((NetworkEvents.lNetworkEvents & FD_WRITE) && bResend)
		{
			//
			// Send Response to client
			//
			do
			{
				Buffer.len = (szResponse.size() - dwBytesSent) >= SENDBLOCK ? SENDBLOCK : szResponse.size() - dwBytesSent;	
				Buffer.buf = (char*)((DWORD)szResponse.c_str() + dwBytesSent);
				
				result = WSASend(
					s,                                                
					&Buffer,                                     
					1,                                    
					&NumberOfBytesSent,
					0,                                          
					0,                           
					NULL);

				if(SOCKET_ERROR != result)				
					dwBytesSent += NumberOfBytesSent;
			}
			while((dwBytesSent < szResponse.size()) && SOCKET_ERROR != result);
			
			if(WSAGetLastError() == WSAEWOULDBLOCK)
			{
				bResend = TRUE;
				continue;
			}

			if(SOCKET_ERROR != result)
			{
				pGenericServer->Stats.nTotalSent += (double)dwBytesSent / 1024;
				pGenericServer->DataSent(dwBytesSent);
			}
			else
			{
				pGenericServer->LogMessage(LOGFILENAME, _T("WSASend(...) failure"), _T("ClientThread, Primary Send"), WSAGetLastError());
				bKeepAlive = FALSE;
			}
			
			if(!bKeepAlive)
			{
				pGenericServer->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
				return THREADEXIT_SUCCESS;
			}
			
			bResend = FALSE;

			//
			// We are finished with this request, clear the buffer for next one
			//
			szRequest.erase(0, string::npos);
		}	
		
		
		if(NetworkEvents.lNetworkEvents & FD_CLOSE)
		{
			pGenericServer->CleanupThread(Event, s, pNewConn, GetCurrentThreadId());
			return THREADEXIT_SUCCESS;
		}
	}

	return THREADEXIT_SUCCESS; // We never reach this point
}	






UINT __stdcall CGenericServer::HelperThread(LPVOID pParam)
{
	CGenericServer *pGenericServer = (CGenericServer*)pParam;
	HANDLELIST::iterator it;

	SetEvent(pGenericServer->ThreadLaunchedEvent);

	for(;;)
	{
		if(WaitForSingleObject(pGenericServer->ShutdownEvent, TICK) == WAIT_TIMEOUT)
		{
			EnterCriticalSection(&pGenericServer->_cs);
			while(pGenericServer->HandleList.size())
			{
				HANDLE h = pGenericServer->HandleList.front(); 
				DWORD n = WaitForSingleObject(h, THREADKILL_TO);
				if(n == WAIT_TIMEOUT)
				{
					pGenericServer->LogMessage(LOGFILENAME, _T("WaitForSingleObject(...) timed out"), _T("HelperThread"));
					if(!TerminateThread(h, THREADEXIT_SUCCESS))
						pGenericServer->LogMessage(LOGFILENAME, _T("TerminateThread(.h.) failure, probably it is already terminated"), _T("HelperThread"), GetLastError());
				}
				CloseHandle(h);
				pGenericServer->HandleList.pop_front();
			}
			LeaveCriticalSection(&pGenericServer->_cs);
		}
		else
			return THREADEXIT_SUCCESS;
		
	}
	
	return THREADEXIT_SUCCESS;
}	









