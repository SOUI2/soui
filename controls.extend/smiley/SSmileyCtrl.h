#ifndef SoSmileyCtrl_h__
#define SoSmileyCtrl_h__

// SoSmileyCtrl.h : Declaration of the CSoSmileyCtrl
#pragma once
#include <Richedit.h>
#include <RichOle.h>

//ITimerHandler
interface  __declspec( uuid("8A0E5678-792F-439b-AEDD-E8D4AB602041") ) ITimerHandler : IUnknown{
    virtual HRESULT STDMETHODCALLTYPE OnTimer(HDC hdc) PURE;
    virtual HRESULT STDMETHODCALLTYPE Clear() PURE;
    virtual HRESULT STDMETHODCALLTYPE GetRect(LPRECT pRect) PURE;
};

//IReOleSource
interface  __declspec( uuid("E9FFF8D9-7585-42ce-B6CE-33336283994E") )ISmileySource : IUnknown{
    virtual HRESULT STDMETHODCALLTYPE Stream_Load( LPSTREAM pStm) PURE;
    virtual HRESULT STDMETHODCALLTYPE Stream_Save( LPSTREAM pStm) PURE;
    virtual HRESULT STDMETHODCALLTYPE LoadFromID(UINT uID) PURE;
    virtual HRESULT STDMETHODCALLTYPE LoadFromFile(LPCWSTR pszFilePath) PURE;
    virtual HRESULT STDMETHODCALLTYPE GetID( UINT *pID) PURE;
    virtual HRESULT STDMETHODCALLTYPE GetFile( BSTR * bstrFile) PURE;
    virtual HRESULT STDMETHODCALLTYPE GetFrameCount( int *pFrameCount) PURE;
    virtual HRESULT STDMETHODCALLTYPE GetFrameDelay( int iFrame,  int *pFrameDelay) PURE;
    virtual HRESULT STDMETHODCALLTYPE GetSize( LPSIZE pSize) PURE;
    virtual HRESULT STDMETHODCALLTYPE Draw( HDC hdc, LPCRECT pRect, int iFrame) PURE;
};

//IReOleHost
interface  __declspec( uuid("0F3687B9-333F-48a4-9001-C994455B430D") )ISmileyHost : IUnknown{
    virtual HRESULT STDMETHODCALLTYPE SendMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT *pRet) PURE;
    virtual HRESULT STDMETHODCALLTYPE GetHostRect( LPRECT prcHost) PURE;
    virtual HRESULT STDMETHODCALLTYPE InvalidateRect( LPCRECT pRect) PURE;
    virtual HRESULT STDMETHODCALLTYPE CreateSource(ISmileySource **ppSource) PURE;
    virtual HRESULT STDMETHODCALLTYPE SetTimer( ITimerHandler * pTimerHander, int nInterval) PURE;
    virtual HRESULT STDMETHODCALLTYPE KillTimer( ITimerHandler * pTimerHander) PURE;
    virtual HRESULT STDMETHODCALLTYPE OnTimer( int nInterval) PURE;
    virtual HRESULT STDMETHODCALLTYPE ClearTimer() PURE;
};  

// IReOleObject
interface  __declspec( uuid("3286141B-C87F-4052-B6A2-376391DCDAF7") )ISmileyCtrl: IUnknown{
    virtual HRESULT STDMETHODCALLTYPE Insert2Richedit( IRichEditOle * ole) PURE;
    virtual HRESULT STDMETHODCALLTYPE SetSource( ISmileySource * pSource) PURE;
    virtual HRESULT STDMETHODCALLTYPE GetSource( ISmileySource ** ppSource) PURE;
};


EXTERN_C const CLSID CLSID_SSmileyCtrl;

    template <class Base>
    class SComObject : public Base
    {
    public:
        typedef Base _BaseClass;
        SComObject(void* = NULL) throw()
        {
        }
        // Set refcount to -(LONG_MAX/2) to protect destruction and 
        // also catch mismatched Release in debug builds
        virtual ~SComObject() throw()
        {
            m_dwRef = -(LONG_MAX/2);
            FinalRelease();
        }
        //If InternalAddRef or InternalRelease is undefined then your class
        //doesn't derive from CComObjectRoot
        STDMETHOD_(ULONG, AddRef)() {return InternalAddRef();}
        STDMETHOD_(ULONG, Release)()
        {
            ULONG l = InternalRelease();
            if (l == 0)
                delete this;
            return l;
        }
        //if _InternalQueryInterface is undefined then you forgot BEGIN_COM_MAP
        STDMETHOD(QueryInterface)(REFIID iid, void ** ppvObject) throw()
        {
            HRESULT hr = _InternalQueryInterface(iid, ppvObject);
            if(iid == IID_IOleObject)
            {
                IOleObject *pOle = *(IOleObject**)ppvObject;
                pOle->SetClientSite(NULL);
            }else if(iid == IID_IPersistStorage)
            {
                IPersistStorage *pStg = *(IPersistStorage**)ppvObject;
                (pStg);
            }
            return hr;
        }
        template <class Q>
        HRESULT STDMETHODCALLTYPE QueryInterface(Q** pp) throw()
        {
            return QueryInterface(__uuidof(Q), (void**)pp);
        }
    };
    
    HRESULT CreateSmiley(REFIID riid,void ** ppUnk);
    
#endif // SoSmileyCtrl_h__