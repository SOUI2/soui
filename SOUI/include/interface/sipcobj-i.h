#pragma once

#include <unknown/obj-ref-i.h>

#define UM_CALL_FUN (WM_USER+1000)

namespace SOUI
{
	enum {
		FUN_ID_CONNECT = 100,
		FUN_ID_DISCONNECT,
		FUN_ID_START,
	};

	struct IShareBuffer {
		enum SEEK {
			seek_set= 0,            /* seek to an absolute position */
			seek_cur,               /* seek relative to current position */
			seek_end                /* seek relative to end of file */
		};
		virtual int Write(const void * data, UINT nLen) = 0;
		virtual int Read(void * buf, UINT nLen) = 0;
		virtual UINT Tell() const = 0;
		virtual UINT Seek(SEEK mode, int nOffset) = 0;
		virtual void SetTail(UINT uPos) = 0;
	};


	class SParamStream
	{
	public:
		SParamStream(IShareBuffer *pBuf) :m_pBuffer(pBuf)
		{
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

	struct IIpcConnection;
	struct IIpcHandle : IObjRef
	{
		virtual void SetIpcConnection(IIpcConnection *pConn) = 0;

		virtual IIpcConnection * GetIpcConnection() const = 0;

		virtual LRESULT OnMessage(ULONG_PTR idLocal, UINT uMsg, WPARAM wp, LPARAM lp, BOOL &bHandled) = 0;

		virtual HRESULT ConnectTo(ULONG_PTR idLocal, ULONG_PTR idRemote) = 0;

		virtual HRESULT Disconnect() = 0;

		virtual bool CallFun(IFunParams * pParam) const = 0;

		virtual ULONG_PTR GetLocalId() const = 0;

		virtual ULONG_PTR GetRemoteId() const = 0;

		virtual IShareBuffer * GetSendBuffer()  = 0;

		virtual IShareBuffer * GetRecvBuffer()  = 0;

		virtual BOOL InitShareBuf(ULONG_PTR idLocal, ULONG_PTR idRemote, UINT nBufSize, void* pSa) = 0;

		virtual BOOL ToStream4Input(IFunParams * pParams,IShareBuffer * pBuf)  const = 0;

		virtual BOOL FromStream4Input(IFunParams * pParams,IShareBuffer * pBuf)  const = 0;

		virtual BOOL ToStream4Output(IFunParams * pParams,IShareBuffer * pBuf)  const = 0;

		virtual BOOL FromStream4Output(IFunParams * pParams,IShareBuffer * pBuf)  const = 0;
	};

	struct IIpcConnection : IObjRef
	{
		virtual IIpcHandle * GetIpcHandle() = 0;
		virtual bool HandleFun(UINT uFunID, SParamStream & ps) = 0;
		virtual void BuildShareBufferName(ULONG_PTR idLocal, ULONG_PTR idRemote, TCHAR szBuf[MAX_PATH]) const = 0;
	};

	struct IIpcSvrCallback 
	{
		virtual void OnNewConnection(IIpcHandle * pIpcHandle, IIpcConnection ** ppConn) = 0;
		virtual void OnConnected(IIpcConnection * pConn) =0;
		virtual void OnDisconnected(IIpcConnection * pConn) =0;
		virtual int GetBufSize() const = 0;
		virtual void * GetSecurityAttr() const = 0;
		virtual void ReleaseSecurityAttr(void* psa) const = 0;
	};

	typedef void(*FunEnumConnection)(IIpcConnection *pConn, ULONG_PTR data);

	struct IIpcServer : IObjRef
	{
		virtual HRESULT Init(ULONG_PTR idSvr, IIpcSvrCallback * pCallback) =0;
		virtual void CheckConnectivity() =0;
		virtual LRESULT OnMessage(ULONG_PTR idLocal, UINT uMsg, WPARAM wp, LPARAM lp,BOOL &bHandled) =0;
		virtual void EnumClient(FunEnumConnection funEnum,ULONG_PTR data) = 0;
	};

	struct IIpcFactory : IObjRef
	{
		virtual HRESULT CreateIpcServer(IIpcServer ** ppServer) =0;
		virtual HRESULT CreateIpcHandle(IIpcHandle ** ppHandle) =0;
	};


}