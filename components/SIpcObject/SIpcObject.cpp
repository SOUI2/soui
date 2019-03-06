#include "stdafx.h"
#include "SIpcObject.h"

namespace SOUI
{
	SIpcHandle::SIpcHandle() :m_pConn(NULL), m_hLocalId(0),m_hRemoteId(0)
	{
	}

	SIpcHandle::~SIpcHandle() {}

	BOOL SIpcHandle::InitShareBuf(ULONG_PTR idLocal, ULONG_PTR idRemote, UINT uBufSize, void* pSa)
	{
		assert(m_hRemoteId == NULL);
		assert(m_hLocalId == NULL);

		TCHAR szName[MAX_PATH];
		
		GetIpcConnection()->BuildShareBufferName(idLocal, idRemote, szName);

		if (!m_SendBuf.OpenMemFile(szName, uBufSize, pSa))
		{
			return FALSE;
		}

		GetIpcConnection()->BuildShareBufferName(idRemote, idLocal, szName);

		if (!m_RecvBuf.OpenMemFile(szName, uBufSize, pSa))
		{
			m_SendBuf.Close();
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
		SParamStream ps(GetRecvBuffer(), false);
		return m_pConn->HandleFun((UINT)wp, ps)?1:0;
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
		m_RecvBuf.Close();
		m_hLocalId = NULL;
		m_SendBuf.Close();
		return S_OK;
	}

	bool SIpcHandle::CallFun(IFunParams * pParam) const
	{
		if (m_hRemoteId == NULL)
			return false;

		SParamStream ps(&m_SendBuf, true);
		pParam->ToStream4Input(ps);
		LRESULT lRet = SendMessage(m_hRemoteId, UM_CALL_FUN, pParam->GetID(), (LPARAM)m_hLocalId);
		if (lRet != 0)
		{
			SParamStream ps2(&m_SendBuf, false);
			pParam->FromStream4Output(ps2);
		}
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
		return &m_SendBuf;
	}

	IShareBuffer * SIpcHandle::GetRecvBuffer()
	{
		return &m_RecvBuf;
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
		SParamStream ps(it->second->GetIpcHandle()->GetRecvBuffer(), false);
		return it->second->HandleFun((UINT)wp, ps) ? 1 : 0;
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
