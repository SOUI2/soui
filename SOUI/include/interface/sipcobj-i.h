#pragma once

#include <unknown/obj-ref-i.h>

#define SOUI_EXP

namespace SOUI
{
	struct IParamStream {
		virtual int Write(const void * data, int nLen) = 0;
		virtual int Read(void * buf, int nLen) = 0;
	};

	struct SOUI_EXP FunParams_Base
	{
		virtual UINT GetID() = 0;
		virtual void ToStream4Input(IParamStream *  ps) {}
		virtual void FromStream4Input(IParamStream *  ps) {}
		virtual void ToStream4Output(IParamStream *  ps) {}
		virtual void FromStream4Output(IParamStream *  ps) {}
	};

	struct IIpcConnCallback
	{
		virtual void BuildShareBufferName(ULONG_PTR idLocal, ULONG_PTR idRemote, char *szBuf[MAX_PATH]) const = 0;
		virtual HRESULT HandleFun(FunParams_Base * pParam) = 0;
	};

	struct IIpcConnection : IObjRef
	{
		virtual void SetCallback(IIpcConnCallback *pCallback) = 0;

		virtual HRESULT ConnectTo(ULONG_PTR idLocal, ULONG_PTR idRemote) = 0;

		virtual HRESULT Disconnect() = 0;

		virtual HRESULT CallFun(FunParams_Base * pParam) const = 0;

	};

	struct IIpcSvrCallback
	{
		virtual void OnConnected(IIpcConnection * pConnection) = 0;
		virtual void OnDisconnected(IIpcConnection * pConnection) = 0;
		virtual void BuildShareBufferName(ULONG_PTR idLocal, ULONG_PTR idRemote, char *szBuf[MAX_PATH]) const = 0;
		virtual ULONG_PTR GetSvrID() const = 0;
		virtual int GetServerBufSize() const = 0;
		virtual int GetClientBufSize() const = 0;
	};

	struct IIpcServer : IObjRef
	{
		virtual void SetCallback(IIpcSvrCallback * pCallback) = 0;
		virtual void CheckConectivity() = 0;
	};

	struct IIpcFactory : IObjRef
	{
		virtual HRESULT CreateIpcServer(IIpcServer ** ppServer) = 0;
		virtual HRESULT CreateIpcConnection(IIpcConnection ** ppConn) = 0;
	};


}