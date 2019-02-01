#include "SIpcObject.h"

namespace SOUI
{

	SIpcObject::SIpcObject()
	{
	}


	SIpcObject::~SIpcObject()
	{
	}


	///////////////////////////////////////////////////////////////////////
	void SIpcServer::SetCallback(IIpcSvrCallback * pCallback)
	{
		m_pCallback = pCallback;
	}
	void SIpcServer::CheckConectivity()
	{
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
