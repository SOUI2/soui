#include "stdafx.h"
#include "SIpcObject.h"

namespace SOUI
{
	SIpcHandle::SIpcHandle() :m_pConn(NULL), m_hLocalId(0),m_hRemoteId(0)
	{
		m_uCallSeq = 0;
	}

	SIpcHandle::~SIpcHandle() {}

	BOOL SIpcHandle::InitShareBuf(ULONG_PTR idLocal, ULONG_PTR idRemote, UINT uBufSize, void* pSa)
	{
		assert(m_hRemoteId == NULL);
		assert(m_hLocalId == NULL);

		TCHAR szName[MAX_PATH];
		
		GetIpcConnection()->BuildShareBufferName(idLocal, idRemote, szName);

		if (!m_sendBuf.OpenMemFile(szName, uBufSize, pSa))
		{
			return FALSE;
		}

		GetIpcConnection()->BuildShareBufferName(idRemote, idLocal, szName);

		if (!m_recvBuf.OpenMemFile(szName, uBufSize, pSa))
		{
			m_sendBuf.Close();
			return FALSE;
		}

		m_hLocalId = (HWND)idLocal;
		m_hRemoteId = (HWND)idRemote;

		return TRUE;
	}

	LRESULT SIpcHandle::OnMessage(ULONG_PTR idLocal, UINT uMsg, WPARAM wp, LPARAM lp, BOOL &bHandled)
	{
		bHandled = FALSE;
		if ((HWND)idLocal != m_hLocalId)
			return 0;
		if (UM_CALL_FUN != uMsg)
			return 0;
		bHandled = TRUE;
		IShareBuffer *pBuf = GetRecvBuffer();
		assert(pBuf->Tell()>= 4); //4=sizeof(int)
		pBuf->Seek(IShareBuffer::seek_cur,-4);
		int nLen;
		pBuf->Read(&nLen, 4);
		assert(pBuf->Tell()>=(UINT)(nLen+ 4));
		pBuf->Seek(IShareBuffer::seek_cur,-(nLen+ 4));
		int nCallSeq = 0;
		pBuf->Read(&nCallSeq,4);
		UINT uFunId = 0;
		pBuf->Read(&uFunId,4);
		SParamStream ps(pBuf);

		bool bReqHandled = m_pConn->HandleFun(uFunId, ps);
		return  bReqHandled?1:0;
	}

	HRESULT SIpcHandle::ConnectTo(ULONG_PTR idLocal, ULONG_PTR idRemote)
	{
		HWND hLocal = (HWND)idLocal;
		HWND hRemote = (HWND)idRemote;
		if (!IsWindow(hLocal))
			return E_INVALIDARG;
		if (!IsWindow(hRemote))
			return E_INVALIDARG;

		LRESULT lRet = ::SendMessage(hRemote, UM_CALL_FUN, FUN_ID_CONNECT, (LPARAM)hLocal);
		if (lRet == 0)
		{
			return E_FAIL;
		}
		InitShareBuf(idLocal, idRemote, 0, NULL);
		return S_OK;
	}

	HRESULT SIpcHandle::Disconnect()
	{
		if (m_hLocalId == NULL)
			return E_UNEXPECTED;
		if (m_hRemoteId == NULL)
			return E_UNEXPECTED;
		::SendMessage(m_hRemoteId, UM_CALL_FUN, FUN_ID_DISCONNECT, (LPARAM)m_hLocalId);
		m_hRemoteId = NULL;
		m_recvBuf.Close();
		m_hLocalId = NULL;
		m_sendBuf.Close();
		return S_OK;
	}

	bool SIpcHandle::CallFun(IFunParams * pParam) const
	{
		if (m_hRemoteId == NULL)
			return false;

		//make sure msg queue is empty.
		MSG msg;
		while(::PeekMessage(&msg, m_hLocalId, UM_CALL_FUN, UM_CALL_FUN, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				PostMessage(m_hLocalId,WM_QUIT,0,0);
				return false;
			}
			DispatchMessage(&msg);
		}

		int nCallSeq = m_uCallSeq ++;
		if(m_uCallSeq>100000) m_uCallSeq=0;

		IShareBuffer *pBuf = &m_sendBuf;
		DWORD dwPos = pBuf->Tell();
		pBuf->Write(&nCallSeq,4);//write call seq first.
		UINT uFunId = pParam->GetID();
		pBuf->Write(&uFunId,4);
		if(!ToStream4Input(pParam, pBuf))
		{
			pBuf->Seek(IShareBuffer::seek_set, dwPos);
			m_sendBuf.SetTail(dwPos);
			assert(false);
			return false;
		}
		int nLen = m_sendBuf.Tell()-dwPos;
		m_sendBuf.Write(&nLen,sizeof(int));//write a length of params to stream, which will be used to locate param header.
		LRESULT lRet = SendMessage(m_hRemoteId, UM_CALL_FUN, pParam->GetID(), (LPARAM)m_hLocalId);
		if (lRet != 0)
		{
			m_sendBuf.Seek(IShareBuffer::seek_set,dwPos+nLen+sizeof(int));//output param must be follow input params.
			BOOL bRet = FromStream4Output(pParam,&m_sendBuf);
			assert(bRet);
		}
		//clear params.
		m_sendBuf.Seek(IShareBuffer::seek_set, dwPos);
		m_sendBuf.SetTail(dwPos);

		return lRet!=0;
	}

	void SIpcHandle::SetIpcConnection(IIpcConnection *pConn)
	{
		m_pConn = pConn;
	}

	IIpcConnection * SIpcHandle::GetIpcConnection() const
	{
		return m_pConn;
	}

	ULONG_PTR SIpcHandle::GetLocalId() const
	{
		return (ULONG_PTR)m_hRemoteId;
	}

	ULONG_PTR SIpcHandle::GetRemoteId() const
	{
		return (ULONG_PTR)m_hRemoteId;
	}

	IShareBuffer * SIpcHandle::GetSendBuffer()
	{
		return &m_sendBuf;
	}

	IShareBuffer * SIpcHandle::GetRecvBuffer()
	{
		return &m_recvBuf;
	}

	static const BYTE KInputFlag= 0xf0;
	static const BYTE KOutputFlag= 0xf1;

	BOOL SIpcHandle::ToStream4Input(IFunParams * pParams,IShareBuffer * pBuf) const
	{
		UINT uId = pParams->GetID();
		pBuf->Write(&uId,sizeof(UINT));
		pBuf->Write(&KInputFlag,1);
		SParamStream ps(pBuf);
		pParams->ToStream4Input(ps);
		return TRUE;
	}

	BOOL SIpcHandle::FromStream4Input(IFunParams * pParams,IShareBuffer * pBuf) const
	{
		UINT uFunId=0;
		pBuf->Read(&uFunId,sizeof(UINT));
		assert(uFunId == pParams->GetID());
		BYTE flag=0;
		pBuf->Read(&flag,1);
		assert(flag == KInputFlag);
		SParamStream ps(pBuf);
		pParams->FromStream4Input(ps);
		return TRUE;
	}

	BOOL SIpcHandle::ToStream4Output(IFunParams * pParams,IShareBuffer * pBuf) const
	{
		UINT uId = pParams->GetID();
		pBuf->Write(&uId,sizeof(UINT));
		pBuf->Write(&KOutputFlag,1);
		SParamStream ps(pBuf);
		pParams->ToStream4Output(ps);
		return TRUE;
	}

	BOOL SIpcHandle::FromStream4Output(IFunParams * pParams,IShareBuffer * pBuf) const
	{
		UINT uFunId=0;
		pBuf->Read(&uFunId,sizeof(UINT));
		assert(uFunId == pParams->GetID());
		BYTE flag=0;
		pBuf->Read(&flag,1);
		assert(flag == KOutputFlag);
		SParamStream ps(pBuf);
		pParams->FromStream4Output(ps);
		return TRUE;
	}


	///////////////////////////////////////////////////////////////////////
	SIpcServer::SIpcServer() 
		:m_pCallback(NULL) 
		, m_hSvr(NULL)
	{}

	LRESULT SIpcServer::OnMessage(ULONG_PTR idLocal, UINT uMsg, WPARAM wp, LPARAM lp,BOOL &bHandled)
	{
		bHandled = FALSE;
		if ((HWND)idLocal != m_hSvr)
			return 0;
		if (UM_CALL_FUN != uMsg)
			return 0;
		bHandled = TRUE;
		return OnClientMsg(uMsg, wp, lp);
	}

	LRESULT SIpcServer::OnClientMsg(UINT uMsg, WPARAM wp, LPARAM lp)
	{
		if (wp == FUN_ID_CONNECT)
			return OnConnect((HWND)lp);
		else if (wp == FUN_ID_DISCONNECT)
			return OnDisconnect((HWND)lp);
		HWND hClient = (HWND)lp;
		std::map<HWND, IIpcConnection*>::iterator it = m_mapClients.find(hClient);
		if (it == m_mapClients.end())
			return 0;
		BOOL bHandled=FALSE;
		return it->second->GetIpcHandle()->OnMessage((ULONG_PTR)m_hSvr,uMsg,wp,lp,bHandled);
	}

	void SIpcServer::EnumClient(FunEnumConnection funEnum, ULONG_PTR data)
	{
		std::map<HWND, IIpcConnection *> map2 = m_mapClients;
		std::map<HWND, IIpcConnection *>::iterator it = map2.begin();
		while (it != map2.end())
		{
			it->second->AddRef();
			it++;
		}

		it = map2.begin();
		while (it != map2.end())
		{
			funEnum(it->second, data);
			it->second->Release();
			it++;
		}
	}

	LRESULT SIpcServer::OnConnect(HWND hClient)
	{
		if (m_mapClients.find(hClient) != m_mapClients.end()) 
			return 0; // existed.
		if (hClient == m_hSvr)
			return 0; // must be different hwnd.
		CAutoRefPtr<IIpcHandle> pIpcHandle;
		pIpcHandle.Attach(new SIpcHandle);

		IIpcConnection *pConn = NULL;
		m_pCallback->OnNewConnection(pIpcHandle, &pConn);
		assert(pConn);
		pIpcHandle->SetIpcConnection(pConn);

		void * pSa = m_pCallback->GetSecurityAttr();
		if (!pIpcHandle->InitShareBuf((ULONG_PTR)m_hSvr, (ULONG_PTR)hClient, m_pCallback->GetBufSize(), pSa))
		{
			m_pCallback->ReleaseSecurityAttr(pSa);
			pConn->Release();
			return 0;
		}
		m_pCallback->ReleaseSecurityAttr(pSa);

		m_mapClients[hClient] = pConn;
		m_pCallback->OnConnected(pConn);
		return 1;
	}

	LRESULT SIpcServer::OnDisconnect(HWND hClient)
	{
		if (m_mapClients.find(hClient) == m_mapClients.end())
			return 0;

		IIpcConnection *pClient = m_mapClients[hClient];
		m_pCallback->OnDisconnected(pClient);

		pClient->Release();
		m_mapClients.erase(hClient);
		return 1;
	}

	HRESULT SIpcServer::Init(ULONG_PTR idSvr, IIpcSvrCallback * pCallback)
	{
		HWND hSvr = (HWND)idSvr;
		if (!IsWindow(hSvr))
			return E_INVALIDARG;
		if (m_hSvr != NULL)
			return E_UNEXPECTED;

		m_hSvr = hSvr;
		m_pCallback = pCallback;
		return S_OK;
	}

	void SIpcServer::CheckConnectivity()
	{
		std::map<HWND, IIpcConnection *>::iterator it = m_mapClients.begin();
		while (it != m_mapClients.end())
		{
			if (!::IsWindow(it->first))
			{
				it->second->Release();
				it = m_mapClients.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////
	HRESULT SIpcFactory::CreateIpcServer(IIpcServer ** ppServer)
	{
		*ppServer = new SIpcServer;
		return S_OK;
	}
	HRESULT SIpcFactory::CreateIpcHandle(IIpcHandle ** ppHandle)
	{
		*ppHandle = new SIpcHandle;
		return S_OK;
	}
}

SIPC_COM_C BOOL SIPC_API SOUI::IPC::SCreateInstance(IObjRef ** ppIpcFactory)
{
	*ppIpcFactory = new SOUI::SIpcFactory();
	return TRUE;
}
