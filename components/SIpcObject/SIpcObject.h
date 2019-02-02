#pragma once
#include <windows.h>
#include <interface/SIpcObj-i.h>
#include <unknown/obj-ref-impl.hpp>
#include <map>
#include "ShareMemBuffer.h"

namespace SOUI
{
	class SIpcConnection : public TObjRefImpl<IIpcConnection>
	{
		friend class SIpcServer;
	public:
		SIpcConnection() :m_pCallback(NULL) {}
		virtual ~SIpcConnection() {}

	public:
		virtual void SetCallback(IIpcConnCallback *pCallback);

		virtual HRESULT ConnectTo(ULONG_PTR idLocal, ULONG_PTR idRemote);

		virtual HRESULT Disconnect();

		virtual HRESULT CallFun(IFunParams * pParam) const;

	public:
		HRESULT HandleFun(UINT uFunID, SParamStream * ps);

	private:
		static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	public:
		CShareMemBuffer * GetLocalBuffer() { return &m_localBuf; }
		CShareMemBuffer * GetRemoteBuffer() { return &m_RemoteBuf; }
	protected:
		BOOL OpenShareBuffer(HWND hLocal, HWND hRemote, DWORD uBufSize);
	protected:
		HWND	m_hLocalId;
		mutable CShareMemBuffer	m_localBuf;
		HWND	m_hRemoteId;
		CShareMemBuffer	m_RemoteBuf;

		IIpcConnCallback * m_pCallback;
		WNDPROC			  m_prevWndProc;
	};


	class SIpcServer : public TObjRefImpl<IIpcServer>
	{
	public:
		SIpcServer();


	public:
		// 通过 TObjRefImpl 继承
		virtual HRESULT Init(ULONG_PTR idSvr, IIpcSvrCallback * pCallback) override;
		virtual void CheckConectivity() override;

	private:
		static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		LRESULT WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		LRESULT OnConnect(HWND hClient);
		LRESULT OnDisconnect(HWND hClient);
		LRESULT OnClientMsg(UINT uMsg, WPARAM wp, LPARAM lp);
	private:
		WNDPROC			  m_prevWndProc;
		IIpcSvrCallback * m_pCallback;
		HWND			  m_hSvr;
		std::map<HWND, SIpcConnection *> m_mapClients;
	};

	class SIpcFactory : public TObjRefImpl<IIpcFactory>
	{
	public:
		// 通过 TObjRefImpl 继承
		virtual HRESULT CreateIpcServer(IIpcServer ** ppServer) override;
		virtual HRESULT CreateIpcConnection(IIpcConnection ** ppConn) override;
	};
	namespace IPC
	{
		SOUI_COM_C BOOL SOUI_COM_API SCreateInstance(IObjRef **ppIpcFactory);
	}
}

