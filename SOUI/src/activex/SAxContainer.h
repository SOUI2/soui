/*
** Copyright 2008-2009, Ernest Laurentin (http://www.ernzo.com/)
**
** Licensed under the Apache License, Version 2.0 (the "License"); 
** you may not use this file except in compliance with the License. 
** You may obtain a copy of the License at 
**
**     http://www.apache.org/licenses/LICENSE-2.0 
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
**
** File:        AtlActiveXContainer.hpp
** Version:     1.1 - Support for windowless keyboard events
**                1.2 - Reconstructed by hjx 2013-11-05
*/
#ifndef ATLACTIVEXCONTAINER_HPP
#define ATLACTIVEXCONTAINER_HPP

#pragma once

#include "SAxUtil.h"
#include "atl.mini/scomcli.h"
#include <MsHtmHst.h>

namespace SOUI
{

struct IAxHostDelegate
{
    virtual HWND GetAxHostWindow() const = 0;
    virtual void OnAxActivate(IUnknown *pCtrl) = 0;
    virtual void OnAxInvalidate(LPCRECT pRect,BOOL bErase) = 0;
    virtual void OnAxSetCapture(BOOL fCapture) = 0;
    virtual HRESULT OnAxGetDC(LPCRECT pRect, DWORD grfFlags, HDC *phDC)=0;
    virtual HRESULT OnAxReleaseDC(HDC hdc)=0;
};

/**
 * ActiveXSite class
 */
template<class T>
class S_NO_VTABLE ActiveXSite :   public IOleClientSite,
                                    public IOleControlSite,
                                    public IOleInPlaceSiteWindowless,
                                    public IAdviseSink
{
    friend T;

    public:
    ActiveXSite()
    : m_pAxHostDelegate(NULL)
    , m_bVisible(true)
    , m_bInplaceActive(false)
    , m_dwMiscStatus(0)
    , m_dwOleObjSink(0)
    , m_dwViewObjectType(0)
    , m_grfFlags(0)
    , m_bCaptured(FALSE)
    , m_bFocused(FALSE)
    {
        memset(&m_rcPos, 0, sizeof(m_rcPos));
    }

    virtual ~ActiveXSite()
    {
        Clear();
    }

    void SetAxHost(IAxHostDelegate *pAxHost)
    {
        m_pAxHostDelegate=pAxHost;
    }

    void SetExternalUIHandler(IDocHostUIHandler *pUiHandler)
    {
        m_spDocHostUIHandler=pUiHandler;
    }

    IUnknown* GetActiveXControl()
    {
        return m_spControl;
    }

    void Init(IUnknown* pControl)
    {
        m_spControl = pControl;
        m_spOleObject = pControl;
        m_spInPlaceObject = pControl;
    }

    void Clear()
    {
        if (m_spControl != NULL)
        {
            if ( m_spViewObject != NULL )
            {
                m_spViewObject->SetAdvise(DVASPECT_CONTENT, 0, NULL);
                m_spViewObject.Release();
            }
            m_spOleObjectWindowless.Release();
            if (m_spInPlaceObject != NULL)
            {
                if (m_bInplaceActive)
                {
                    m_spInPlaceObject->InPlaceDeactivate();
                    m_bInplaceActive = false;
                }
                m_spInPlaceObject.Release();
            }
            if (m_spOleObject != NULL)
            {
                m_spOleObject->Unadvise(m_dwOleObjSink);
                m_spOleObject->SetClientSite(NULL);
                m_spOleObject->Close(OLECLOSE_NOSAVE);
                m_spOleObject.Release();
            }
            m_spControl = NULL;
        }
    }

    HRESULT InitControl(IStream* pStream = NULL)
    {
        if( m_spControl == 0) return E_NOINTERFACE;
        HRESULT hr = E_POINTER;
        SComQIPtr<IPersistStreamInit> spPSI = m_spControl;
        if ( spPSI != NULL )
        {
            if ( pStream != NULL )
                hr = spPSI->Load( pStream );
            else
                hr = spPSI->InitNew();
        }
        else if (pStream)
        {
            SComQIPtr<IPersistStream> spPS(m_spOleObject);
            if (spPS)
                hr = spPS->Load(pStream);
        }
        return hr;
    }

    HRESULT ActivateAx(IStream* pStream = NULL)
    {
        if( m_spOleObject == 0) return E_UNEXPECTED;
        HRESULT hr;

        // this must be set even before calling IPersistStreamInit::InitNew
        hr = m_spOleObject->GetMiscStatus(DVASPECT_CONTENT, &m_dwMiscStatus);
        if (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) {
            hr = m_spOleObject->SetClientSite(static_cast<IOleClientSite*>(this));
            if (FAILED(hr)) return hr;
        }

        hr = InitControl(pStream);
        if ( FAILED(hr) )
        {
            if (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)
                m_spOleObject->SetClientSite(NULL);
            return hr;
        }

		if (0 == (m_dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST)) {
			hr = m_spOleObject->SetClientSite(static_cast<IOleClientSite*>(this));
			if (FAILED(hr)) return hr;
		}

        hr = DoInplaceActivate();
        return hr;
    }

    bool HitTest(const POINT& pt) const
    {
        if ( m_spViewObject != NULL && m_dwViewObjectType == 7) {
            DWORD dwHitResult = HITRESULT_OUTSIDE;
            m_spViewObject->QueryHitPoint(DVASPECT_CONTENT, &m_rcPos, pt, 0, &dwHitResult);
            return (dwHitResult == HITRESULT_HIT);
        }
        // else
        return (::PtInRect(&m_rcPos, pt) != FALSE);
    }

    bool InsideClippingRegion(const RECT& rcClip) const
    {
        // true when top/let or bottom/right corners intersect into client area
        const POINT* pts = (const POINT*)(&m_rcPos);
        const POINT* ptc = (const POINT*)(&rcClip);
        return ( ::PtInRect(&rcClip, pts[0]) || ::PtInRect(&rcClip, pts[1]) ||
                 ::PtInRect(&m_rcPos,ptc[0]) || ::PtInRect(&m_rcPos, ptc[1] ));
    }

    HRESULT DoVerb(long verb)
    {
        HRESULT hr = E_FAIL;
        if ((m_dwMiscStatus & (OLEMISC_INVISIBLEATRUNTIME|OLEMISC_NOUIACTIVATE)) != 0 )
        {
            hr = S_FALSE;
        }
        else if (m_spOleObject != NULL)
        {
            hr = m_spOleObject->DoVerb(verb, NULL, static_cast<IOleClientSite*>(this), 0,m_pAxHostDelegate->GetAxHostWindow(), &m_rcPos);
            if (verb == OLEIVERB_INPLACEACTIVATE && SUCCEEDED(hr))
            {
                m_bInplaceActive = true;
            } else {
                hr = E_UNEXPECTED;
            }
        }
        return hr;
    }

    LRESULT Draw(HDC hDC, LPCRECT lpClipRect)
    {
        LRESULT lResult = S_FALSE;
        if ( lpClipRect && !InsideClippingRegion(*lpClipRect)) {
            return lResult;
        }
        if ( m_bVisible && m_spViewObject != NULL ) {
            RECTL rcPos={m_rcPos.left,m_rcPos.top,m_rcPos.right,m_rcPos.bottom};
            lResult = m_spViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC,
                                     &rcPos, NULL, NULL, 0);
        }
        return lResult;
    }

    HRESULT FireAmbientPropertyChange(DISPID dispChanged)
    {
        HRESULT hr = S_OK;
        SComQIPtr<IOleControl, &__uuidof(IOleControl)> spOleControl(m_spControl);
        if (spOleControl != NULL)
            hr = spOleControl->OnAmbientPropertyChange(dispChanged);
        return hr;
    }

    LRESULT OnWindowMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        LRESULT lResult = 0;
        if (m_bInplaceActive && m_spOleObjectWindowless != NULL)
        {
            m_spOleObjectWindowless->OnWindowMessage(uMsg, wParam, lParam, &lResult);
        }
        return lResult;
    }

    STDMETHOD(QueryInterface2)(REFIID iid, void** object)
    {
        HRESULT hr = S_OK;
        *object = NULL;
        if (iid == IID_IOleClientSite) {
            *object = static_cast<IOleClientSite*>(this);
        } else if (iid == IID_IOleControlSite) {
            *object = static_cast<IOleControlSite*>(this);
        } else if (iid == IID_IOleInPlaceSite) {
            *object = static_cast<IOleInPlaceSite*>(this);
        } else if (iid == IID_IOleInPlaceSiteEx) {
            *object = static_cast<IOleInPlaceSiteEx*>(this);
        } else if (iid == IID_IOleInPlaceSiteWindowless) {
            *object = static_cast<IOleInPlaceSiteWindowless*>(this);
        } else if (iid == IID_IAdviseSink ) {
            *object = static_cast<IAdviseSink*>(this);
        } else if( iid == IID_IDocHostUIHandler && m_spDocHostUIHandler){
            *object = m_spDocHostUIHandler;
        } else {
            hr = E_NOINTERFACE;
        }
        return hr;
    }

    ///////////////////////////////////////////////////////////////////////////
    // IOleClientSite
    STDMETHOD(SaveObject)(void)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::SaveObject\n"));
    }

    STDMETHOD(GetMoniker)(DWORD /*dwAssign*/, DWORD dwWhichMoniker, IMoniker** ppmk)
    {
        return E_NOTIMPL;
    }

    STDMETHOD(GetContainer)(IOleContainer **ppContainer)
    {
        HRESULT hr = E_POINTER;
        if ( ppContainer != NULL ) {
            hr = static_cast<T*>(this)->QueryInterface(__uuidof(IOleContainer), reinterpret_cast<void**>(ppContainer));
        }
        return hr;
    }

    STDMETHOD(ShowObject)(void)
    {
        HWND hWnd = m_pAxHostDelegate->GetAxHostWindow();
        HRESULT hr = E_FAIL;
        if ( ::IsWindow(hWnd) && ::IsRectEmpty(&m_rcPos) == FALSE)
        {
            m_bVisible = true;
            if ( m_bInplaceActive )
            {
                m_pAxHostDelegate->OnAxInvalidate(&m_rcPos,TRUE);
                hr = S_OK;
            }
        }
        return hr;
    }

    STDMETHOD(OnShowWindow)(BOOL fShow)
    {
        m_bVisible = ( fShow != FALSE );
        return S_OK;
    }

    STDMETHOD(RequestNewObjectLayout)(void)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::RequestNewObjectLayout\n"));
    }


    ///////////////////////////////////////////////////////////////////////////
    // IOleControlSite
    STDMETHOD(OnControlInfoChanged)(void)
    {
        return S_OK;
    }

    STDMETHOD(LockInPlaceActive)(BOOL /*fLock*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::LockInPlaceActive\n"));
    }

    STDMETHOD(GetExtendedControl)(IDispatch**)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::GetExtendedControl\n"));
    }

    STDMETHOD(TransformCoords)(POINTL* /*pPtlHimetric*/, POINTF* /*pPtfContainer*/, DWORD /*dwFlags*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::TransformCoords\n"));
    }

    STDMETHOD(TranslateAccelerator)(MSG* /*pMsg*/, DWORD /*grfModifiers*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::TranslateAccelerator\n"));
    }

    STDMETHOD(OnFocus)(BOOL fGotFocus)
    {
        // ignore this, controls should use SetFocus(flag) instead
        return S_OK;
    }

    STDMETHOD(ShowPropertyFrame)(void)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::ShowPropertyFrame\n"));
    }


    ///////////////////////////////////////////////////////////////////////////
    // IOleInPlaceSiteWindowless
    STDMETHOD(CanWindowlessActivate)(void)
    {
        // support Windowless activation
        return S_OK;
    }

    STDMETHOD(GetCapture)(void)
    {
        HRESULT hr = m_bCaptured?S_OK:S_FALSE;
        return hr;
    }

    STDMETHOD(SetCapture)(BOOL fCapture)
    {
        if(fCapture!=m_bCaptured)
        {
            m_bCaptured=fCapture;
            m_pAxHostDelegate->OnAxSetCapture(fCapture);
        }
        return S_OK;
    }

    STDMETHOD(GetFocus)(void)
    {
        return m_bFocused?S_OK: S_FALSE;
    }

    STDMETHOD(SetFocus)(BOOL fFocus)
    {
        if(m_bFocused!=fFocus)
        {
            m_bFocused=fFocus;
            InvalidateRect(NULL,true);
        }

        return S_OK;
    }

    STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC *phDC)
    {
        return m_pAxHostDelegate->OnAxGetDC(pRect,grfFlags,phDC);
    }

    STDMETHOD(ReleaseDC)(HDC hDC)
    {
        return m_pAxHostDelegate->OnAxReleaseDC(hDC);
    }

    STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase)
    {
        // Blindly honor the specified region
        // well behaving control should pass a valid intersect rect or NULL
        if ( pRect == NULL ) pRect = &m_rcPos;
        m_pAxHostDelegate->OnAxInvalidate(pRect,fErase);
        return S_OK;
    }

    STDMETHOD(InvalidateRgn)(HRGN /*hRGN*/, BOOL fErase)
    {
        return InvalidateRect(NULL, fErase);
    }

    STDMETHOD(ScrollRect)(INT /*dx*/, INT /*dy*/, LPCRECT /*pRectScroll*/, LPCRECT /*pRectClip*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::ScrollRect\n"));
    }

    STDMETHOD(AdjustRect)(LPRECT /*prc*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::AdjustRect\n"));
    }

    STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult)
    {
        *plResult = DefWindowProc(m_pAxHostDelegate->GetAxHostWindow(), msg, wParam, lParam);
        return S_OK;
    }

    // IOleInPlaceSiteEx
    STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags)
    {
        HRESULT hr = S_FALSE;
        OleLockRunning(m_spOleObject, TRUE, FALSE);
        if (pfNoRedraw) {
            *pfNoRedraw = FALSE;
        }
                
        if ((dwFlags & ACTIVATE_WINDOWLESS) != 0) {
            m_spOleObjectWindowless = m_spControl;
            
            if ( m_spOleObjectWindowless != NULL ) {
                m_bInplaceActive = true;
                hr = S_OK;
            }
        } else {
            // object is not windowless, ensure we won't dispatch message to this control
            m_bInplaceActive = false;
        }
        return hr;
    }

    STDMETHOD(OnInPlaceDeactivateEx)(BOOL /*fNoRedraw*/)
    {
        m_bInplaceActive = false;
        return S_OK;
    }

    STDMETHOD(RequestUIActivate)(void)
    {
        return S_OK;
    }

    // IOleInPlaceSite
    STDMETHOD(CanInPlaceActivate)(void)
    {
        return S_OK;
    }

    STDMETHOD(OnInPlaceActivate)(void)
    {
        return S_OK;
    }

    STDMETHOD(OnUIActivate)(void)
    {
        return S_OK;
    }

    STDMETHOD(GetWindowContext)(IOleInPlaceFrame **ppFrame,
                               IOleInPlaceUIWindow **ppDoc,
                               LPRECT lprcPosRect,
                               LPRECT lprcClipRect,
                               LPOLEINPLACEFRAMEINFO lpFrameInfo)
    {
        if (ppFrame) { 
            *ppFrame=NULL;
        }
        if (ppDoc) {
            *ppDoc = NULL;
        }
        if (lprcPosRect) {
            memcpy(lprcPosRect, &m_rcPos, sizeof(m_rcPos));
        }
        if (lprcClipRect) {
            memcpy(lprcClipRect, &m_rcPos, sizeof(m_rcPos));
        }
        if (lpFrameInfo) {
            lpFrameInfo->fMDIApp = FALSE;
            lpFrameInfo->hwndFrame = m_pAxHostDelegate->GetAxHostWindow();
            lpFrameInfo->haccel = NULL;
            lpFrameInfo->cAccelEntries = 0;
        }
        return S_OK;
    }

    STDMETHOD(Scroll)(SIZE /*scrollExtant*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::Scroll\n"));
    }

    STDMETHOD(OnUIDeactivate)(BOOL /*fUndoable*/)
    {
        return S_OK;
    }

    STDMETHOD(OnInPlaceDeactivate)(void)
    {
        return OnInPlaceDeactivateEx(TRUE);
    }

    STDMETHOD(DiscardUndoState)(void)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::DiscardUndoState"));
    }

    STDMETHOD(DeactivateAndUndo)(void)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::DeactivateAndUndo"));
    }

    STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect)
    {
        HRESULT hr = E_UNEXPECTED;
        hr = S_OK;
        if (lprcPosRect && memcmp(&m_rcPos, lprcPosRect, sizeof(m_rcPos)) != 0 )
        {
            if (m_spInPlaceObject)
            {
                memcpy(&m_rcPos, lprcPosRect, sizeof(m_rcPos));
                hr = m_spInPlaceObject->SetObjectRects(&m_rcPos, &m_rcPos);
            }
            if ( SUCCEEDED(hr) )
            {
                hr = SetExtent(m_rcPos.right - m_rcPos.left, m_rcPos.bottom - m_rcPos.top);
            }
        }
        return hr;
    }

    // IOleWindow
    STDMETHOD(GetWindow)(HWND *phwnd)
    {
        //
        *phwnd=m_pAxHostDelegate->GetAxHostWindow();
        return S_OK;
    }

    STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXSite::ContextSensitiveHelp\n"));
    }


    // IAdviseSink
    STDMETHOD_(void, OnDataChange)(FORMATETC* /*pFormatetc*/, STGMEDIUM* /*pStgmed*/)
    {
    }

    STDMETHOD_(void, OnViewChange)(DWORD /*dwAspect*/, LONG /*lindex*/)
    {
    }

    STDMETHOD_(void, OnRename)(IMoniker* /*pmk*/)
    {
    }

    STDMETHOD_(void, OnSave)(void)
    {
    }

    STDMETHOD_(void, OnClose)(void)
    {
    }

    protected:
    HRESULT SetExtent(int width, int height)
    {
        HRESULT hr = E_UNEXPECTED;
        if (m_spOleObject != NULL)
        {
            SIZEL pxsize = { width, height };
            SIZEL hmsize = { 0 };
            SPixelToHiMetric(&pxsize, &hmsize);
            hr = m_spOleObject->SetExtent(DVASPECT_CONTENT, &hmsize);
            hr = m_spOleObject->GetExtent(DVASPECT_CONTENT, &hmsize);
            SHiMetricToPixel(&hmsize, &pxsize);
            m_rcPos.right = m_rcPos.left + pxsize.cx;
            m_rcPos.bottom = m_rcPos.top + pxsize.cy;
        }
        return hr;
    }

    HRESULT DoInplaceActivate()
    {
        HRESULT hr;
        m_pAxHostDelegate->OnAxActivate(m_spControl);
        m_dwViewObjectType = 0;
        hr = m_spOleObject->QueryInterface(__uuidof(IViewObjectEx), reinterpret_cast<void**>(&m_spViewObject));
        if (FAILED(hr))
        {
            hr = m_spOleObject->QueryInterface(__uuidof(IViewObject2), reinterpret_cast<void**>(&m_spViewObject));
            if (SUCCEEDED(hr))
                m_dwViewObjectType = 3;
        } else {
            m_dwViewObjectType = 7;
        }

        if (FAILED(hr))
        {
            hr = m_spOleObject->QueryInterface(__uuidof(IViewObject), reinterpret_cast<void**>(&m_spViewObject));
            if (SUCCEEDED(hr))
                m_dwViewObjectType = 1;
        }
        SComQIPtr<IAdviseSink> advise_sink=m_spControl;
        m_spOleObject->Advise(advise_sink, &m_dwOleObjSink);

        if (m_spViewObject)
            hr=m_spViewObject->SetAdvise(DVASPECT_CONTENT, 0, this);

        // Do Inplace activate if possible
        hr = DoVerb(OLEIVERB_INPLACEACTIVATE);
        return hr;
    }


    private:
    bool                            m_bVisible;
    bool                            m_bInplaceActive;
    BOOL                            m_bFocused;
    BOOL                            m_bCaptured;
    DWORD                           m_dwMiscStatus;
    DWORD                           m_dwViewObjectType;
    DWORD                            m_dwOleObjSink;
    DWORD                            m_grfFlags;

    RECT                            m_rcPos;
    SComPtr<IUnknown>               m_spControl;
    SComPtr<IViewObjectEx>          m_spViewObject;
    SComQIPtr<IOleObject>           m_spOleObject;
    SComQIPtr<IOleInPlaceObject>    m_spInPlaceObject;
    SComQIPtr<IOleInPlaceObjectWindowless> m_spOleObjectWindowless;
    SComPtr<IDocHostUIHandler>        m_spDocHostUIHandler;
    IAxHostDelegate                *    m_pAxHostDelegate;
};





/**
 * ActiveXContainerImpl
 */
class S_NO_VTABLE SAxContainer :   public IOleContainer,
                                             public IBindHost,
                                             public IServiceProvider,
                                             public MinimumIDispatchImpl,
                                             public ActiveXSite<SAxContainer>
{
    public:

    SAxContainer();
    virtual ~SAxContainer();

    BOOL CreateControl(REFGUID guid, DWORD dwClsCtx=CLSCTX_INPROC_SERVER);

    ///////////////////////////////////////////////////////////////////////////
    // IOleContainer
    STDMETHOD(EnumObjects)( DWORD /*grfFlags*/, IEnumUnknown** /*ppenum*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXContainerImpl::EnumObjects\n"));
    }

    STDMETHOD(LockContainer)(BOOL /*fLock*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXContainerImpl::LockContainer\n"));
    }

    // IOleContainer::IParseDisplayName
    STDMETHOD(ParseDisplayName)(IBindCtx* /*pbc*/, LPOLESTR /*pszDisplayName*/,
                                ULONG* /*pchEaten*/,IMoniker** /*ppmkOut*/)
    {
        ATLTRACENOTIMPL(_T("ActiveXContainerImpl::ParseDisplayName\n"));
    }


    // IBindHost
    STDMETHOD(CreateMoniker)(LPOLESTR szName, IBindCtx* pBC,
                             IMoniker** ppmk, DWORD dwReserved);
    STDMETHOD(MonikerBindToStorage)(IMoniker *pMk, IBindCtx *pBC,
                                    IBindStatusCallback *pBSC, REFIID riid, void **ppvObj);
    STDMETHOD(MonikerBindToObject)(IMoniker* pMk, IBindCtx* pBC,
                                   IBindStatusCallback* pBSC, REFIID riid, void** ppvObj);

    // IServiceProvider
    STDMETHOD(QueryService)(REFGUID guidService, REFIID riid, void **ppvObject);

    STDMETHOD(QueryInterface)(REFIID iid, void** object);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

private:
    LONG            m_lRefCnt;
};


}//end of namespace SOUI

#endif //ATLACTIVEXCONTAINER_HPP