#pragma once

#include <unknown/obj-ref-i.h>

#define SOUI_EXP

namespace SOUI
{
	enum {
		FUN_ID_CONNECT = 100,
		FUN_ID_DISCONNECT,
		FUN_ID_START,
	};

	struct IShareBuffer {
		virtual void StartRead() = 0;
		virtual void StartWrite() = 0;
		virtual int Write(const void * data, UINT nLen) = 0;
		virtual int Read(void * buf, UINT nLen) = 0;
	};


	class SParamStream
	{
	public:
		SParamStream(IShareBuffer *pBuf, bool bOutStream) :m_pBuffer(pBuf)
		{
			m_pBuffer->StartRead();
			if (bOutStream) m_pBuffer->StartWrite();
		}

		IShareBuffer * GetBuffer() {
			return m_pBuffer;
		}

		template<typename T>
		SParamStream & operator<<(const T & data)
		{
			Write((const void*)&data, sizeof(data));
			return *this;
		}


		template<typename T>
		SParamStream & operator >> (T &data)
		{
			Read((void*)&data, sizeof(data));
			return *this;
		}

	public:
		int Write(const void * data, int nLen)
		{
			return m_pBuffer->Write(data, nLen);
		}
		int Read(void * buf, int nLen) 
		{
			return m_pBuffer->Read(buf, nLen);
		}

	protected:
		IShareBuffer * m_pBuffer;
	};

	struct  IFunParams
	{
		virtual UINT GetID() = 0;
		virtual void ToStream4Input(SParamStream &  ps) = 0;
		virtual void ToStream4Output(SParamStream &  ps) = 0;
		virtual void FromStream4Input(SParamStream &  ps) = 0;
		virtual void FromStream4Output(SParamStream &  ps) = 0;
	};

	struct IIpcConnCallback
	{
		virtual HRESULT HandleFun(UINT uFunID, SParamStream * ps) = 0;
		virtual void BuildShareBufferName(ULONG_PTR idLocal, ULONG_PTR idRemote, TCHAR szBuf[MAX_PATH]) const = 0;
		virtual void * GetSecurityAttr() const = 0;
		virtual void ReleaseSecurityAttr(void* psa) const = 0;
	};

	struct IIpcConnection : IObjRef
	{
		virtual void SetCallback(IIpcConnCallback *pCallback) = 0;

		virtual HRESULT ConnectTo(ULONG_PTR idLocal, ULONG_PTR idRemote) = 0;

		virtual HRESULT Disconnect() = 0;

		virtual HRESULT CallFun(IFunParams * pParam) const = 0;

	};

	struct IIpcSvrCallback : IIpcConnCallback
	{
		virtual void OnConnected(IIpcConnection * pConnection) = 0;
		virtual void OnDisconnected(IIpcConnection * pConnection) = 0;
		virtual int GetBufSize() const = 0;
	};

	struct IIpcServer : IObjRef
	{
		virtual void CheckConectivity() = 0;
		virtual HRESULT Init(ULONG_PTR idSvr, IIpcSvrCallback * pCallback) = 0;
	};

	struct IIpcFactory : IObjRef
	{
		virtual HRESULT CreateIpcServer(IIpcServer ** ppServer) = 0;
		virtual HRESULT CreateIpcConnection(IIpcConnection ** ppConn) = 0;
	};


}