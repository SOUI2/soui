#include "stdafx.h"
#include "SIpcObject.h"

#define UM_CALL_FUN (WM_USER+1000)

namespace SOUI
{
	void SIpcConnection::SetCallback(IIpcConnCallback * pCallback) {
		m_pCallback = pCallback;
	}

	LRESULT SIpcConnection::_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SIpcConnection * _this = (SIpcConnection*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		return _this->WndProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT SIpcConnection::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (UM_CALL_FUN != uMsg)
		{
			return m_prevWndProc(hWnd, uMsg, wParam, lParam);
		}
		return 0;
	}

	BOOL SIpcConnection::OpenShareBuffer(HWND hLocal, HWND hRemote, DWORD uBufSize) {
		assert(m_pCallback);
		assert(m_hRemoteId == NULL);
		assert(m_hLocalId == NULL);

		TCHAR szName[MAX_PATH];
		m_pCallback->BuildShareBufferName((ULONG_PTR)hLocal, (ULONG_PTR)hRemote, szName);

		void * psa = m_pCallback->GetSecurityAttr();
		if (!m_localBuf.OpenMemFile(szName, uBufSize, psa))
		{
			m_pCallback->ReleaseSecurityAttr(psa);
			return FALSE;
		}
		m_hLocalId = hLocal;

		m_pCallback->BuildShareBufferName((ULONG_PTR)hRemote, (ULONG_PTR)hLocal, szName);

		if (!m_RemoteBuf.OpenMemFile(szName, uBufSize))
		{
			m_pCallback->ReleaseSecurityAttr(psa);
			return FALSE;
		}
		m_pCallback->ReleaseSecurityAttr(psa);

		m_hRemoteId = hRemote;

		return TRUE;
	}

	HRESULT SIpcConnection::ConnectTo(ULONG_PTR idLocal, ULONG_PTR idRemote)
	{
		if (!m_pCallback)
			return E_ABORT;
		HWND hLocal = (HWND)idLocal;
		HWND hRemote = (HWND)idRemote;
		if (!IsWindow(hLocal))
			return E_INVALIDARG;
		if (!IsWindow(hRemote))
			return E_INVALIDARG;

		if (::GetWindowLongPtr(hLocal, GWLP_USERDATA) != 0)
			return E_NOT_VALID_STATE;

		LRESULT lRet = ::SendMessage(hRemote, UM_CALL_FUN, FUN_ID_CONNECT, (LPARAM)hLocal);
		if (lRet == 0)
		{
			return 0;
		}
		m_prevWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(hLocal, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&SIpcConnection::_WndProc)));
		::SetWindowLongPtr(hLocal, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		OpenShareBuffer(hLocal, hRemote, 0);
		return S_OK;
	}

	HRESULT SIpcConnection::Disconnect()
	{
		if (m_hLocalId == NULL)
			return E_NOT_VALID_STATE;
		if (m_hRemoteId == NULL)
			return E_NOT_VALID_STATE;
		::SendMessage(m_hRemoteId, UM_CALL_FUN, FUN_ID_DISCONNECT, (LPARAM)m_hLocalId);
		m_hRemoteId = NULL;
		m_RemoteBuf.Close();
		m_hLocalId = NULL;
		m_localBuf.Close();
		return S_OK;
	}

	HRESULT SIpcConnection::CallFun(IFunParams * pParam) const
	{
		if (m_hRemoteId == NULL)
			return 0;

		SParamStream ps(&m_localBuf, true);
		pParam->ToStream4Input(ps);
		LRESULT lRet = SendMessage(m_hRemoteId, UM_CALL_FUN, pParam->GetID(), (LPARAM)m_hLocalId);
		if (lRet != 0)
		{
			SParamStream ps2(&m_localBuf, false);
			pParam->FromStream4Output(ps2);
		}
		return lRet;
	}

	HRESULT SIpcConnection::HandleFun(UINT uFunID, SParamStream * ps)
	{
		return m_pCallback->HandleFun(uFunID, ps);
	}

	///////////////////////////////////////////////////////////////////////
	SIpcServer::SIpcServer() 
		:m_pCallback(NULL) 
		, m_hSvr(NULL)
	{}

	LRESULT CALLBACK SIpcServer::_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		SIpcServer * _this = (SIpcServer*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		return _this->WndProc(hWnd, uMsg, wParam, lParam);
	}

	LRESULT SIpcServer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (UM_CALL_FUN != uMsg)
		{
			return m_prevWndProc(hWnd, uMsg, wParam, lParam);
		}
		return OnClientMsg(uMsg, wParam, lParam);
	}


	LRESULT SIpcServer::OnClientMsg(UINT uMsg, WPARAM wp, LPARAM lp)
	{
		if (wp == FUN_ID_CONNECT)
			return OnConnect((HWND)lp);
		else if (wp == FUN_ID_DISCONNECT)
			return OnDisconnect((HWND)lp);
		HWND hClient = (HWND)lp;
		std::map<HWND, SIpcConnection*>::iterator it = m_mapClients.find(hClient);
		if (it == m_mapClients.end())
			return 0;
		SParamStream ps(it->second->GetRemoteBuffer(), false);
		return it->second->HandleFun((UINT)wp, &ps) ? 1 : 0;
	}

	LRESULT SIpcServer::OnConnect(HWND hClient)
	{
		if (m_mapClients.find(hClient) != m_mapClients.end()) return 0;

		SIpcConnection * pConnection = new SIpcConnection();
		pConnection->SetCallback(m_pCallback);
		if (!pConnection->OpenShareBuffer(m_hSvr, hClient, m_pCallback->GetBufSize()))
			goto error;
		m_mapClients[hClient] = pConnection;
		return 1;
	error:
		if (pConnection)
		{
			pConnection->Release();
		}
		return 0;
	}

	LRESULT SIpcServer::OnDisconnect(HWND hClient)
	{
		if (m_mapClients.find(hClient) == m_mapClients.end())
			return 0;

		IIpcConnection *pClient = m_mapClients[hClient];
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
			return E_NOT_VALID_STATE;
		
		if (::GetWindowLongPtr(hSvr, GWLP_USERDATA) != 0)
			return E_NOT_VALID_STATE;

		m_prevWndProc = reinterpret_cast<WNDPROC>(::SetWindowLongPtr(hSvr,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(&SIpcServer::_WndProc)));
		::SetWindowLongPtr(hSvr, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		m_hSvr = hSvr;
		m_pCallback = pCallback;

		return S_OK;
	}

	void SIpcServer::CheckConectivity()
	{
		std::map<HWND, SIpcConnection *>::iterator it = m_mapClients.begin();
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
	HRESULT SIpcFactory::CreateIpcConnection(IIpcConnection ** ppConn)
	{
		return E_NOTIMPL;
	}
}

SOUI_COM_C BOOL SOUI_COM_API SOUI::IPC::SCreateInstance(IObjRef ** ppIpcFactory)
{
	*ppIpcFactory = new SOUI::SIpcFactory();
	return TRUE;
}
