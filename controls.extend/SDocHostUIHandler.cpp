#include "StdAfx.h"
#include "SDocHostUIHandler.h"
#include <mshtml.h>
#include <activex/SBStr.h>

namespace SOUI
{
    SDocHostUIHandler::SDocHostUIHandler(IScriptCaller *sc)
		: m_disContextMenu(FALSE)
		, m_disF5(FALSE)
		, m_bDisableScrollBar(FALSE)
		, m_bDisable3DBorder(FALSE)
		, m_bDisableTextSelect(FALSE)
    {
        m_disp = new SJsCallCppIDispatch(sc);
    }

    SDocHostUIHandler::~SDocHostUIHandler(void)
    {
    }

    //////////////////////////////////////////////////////////////////////////
    // IDocHostUIHandler
    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::ShowContextMenu(DWORD dwID, POINT *ppt, IUnknown *pcmdtReserved, IDispatch *pdispReserved)
    {
		if (m_disContextMenu)
		{
			return S_OK;
		}

		return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::GetHostInfo(DOCHOSTUIINFO *pInfo)
    {
        if(!pInfo) return E_INVALIDARG;
        pInfo->cbSize = sizeof(DOCHOSTUIINFO);
		pInfo->dwFlags = DOCHOSTUIFLAG_DISABLE_HELP_MENU;
		if(m_bDisableTextSelect) pInfo->dwFlags |= DOCHOSTUIFLAG_DIALOG;
		if(m_bDisable3DBorder) pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER;
		if(m_bDisableScrollBar) pInfo->dwFlags |= DOCHOSTUIFLAG_SCROLL_NO;
            
        pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::ShowUI(DWORD dwID, IOleInPlaceActiveObject *pActiveObject, IOleCommandTarget *pCommandTarget, IOleInPlaceFrame *pFrame, IOleInPlaceUIWindow *pDoc)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::HideUI(void)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::UpdateUI(void)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::EnableModeless(BOOL fEnable)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::OnDocWindowActivate(BOOL fActivate)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::OnFrameWindowActivate(BOOL fActivate)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::ResizeBorder(LPCRECT prcBorder, IOleInPlaceUIWindow *pUIWindow, BOOL fRameWindow)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::TranslateAccelerator(LPMSG lpMsg, const GUID *pguidCmdGroup, DWORD nCmdID)
    {
		if (lpMsg->message == WM_KEYDOWN || lpMsg->message == WM_KEYUP)
		{
			if (lpMsg->wParam == VK_F5 && m_disF5)
			{
				return S_OK;
			}
			else 
			{
				return S_FALSE;
			}
		}

        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::GetOptionKeyPath(__out LPOLESTR *pchKey, DWORD dw)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::GetDropTarget(IDropTarget *pDropTarget, IDropTarget **ppDropTarget)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::GetExternal(IDispatch **ppDispatch)
    {
        *ppDispatch = m_disp;
        (*ppDispatch)->AddRef();
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::TranslateUrl(DWORD dwTranslate, __in __nullterminated OLECHAR *pchURLIn, __out OLECHAR **ppchURLOut)
    {
        return E_NOTIMPL;
    }

    HRESULT STDMETHODCALLTYPE SDocHostUIHandler::FilterDataObject(IDataObject *pDO, IDataObject **ppDORet)
    {
        return E_NOTIMPL;
    }


    //////////////////////////////////////////////////////////////////////////
    // CImpIDispatch
    bool CallCpp(LPCWSTR pszParam)
    {
        return MessageBoxW(GetActiveWindow(),pszParam,L"jscall",MB_OKCANCEL)==IDOK?true:false;
    }

    STDMETHODIMP SJsCallCppIDispatch::Invoke(/* [in] */ DISPID dispIdMember, /* [in] */ REFIID /*riid*/, /* [in] */ LCID /*lcid*/, /* [in] */ WORD wFlags, /* [out][in] */ DISPPARAMS* pDispParams, /* [out] */ VARIANT* pVarResult, /* [out] */ EXCEPINFO* /*pExcepInfo*/, /* [out] */ UINT* puArgErr)
    {
		if ((wFlags & DISPATCH_PROPERTYGET) && pVarResult)
		{
			VariantInit(pVarResult);
			V_VT(pVarResult) = VT_BOOL;
			V_BOOL(pVarResult) = TRUE;
		}

		if ((wFlags & DISPATCH_METHOD) && m_sc)
		{
			std::vector<std::wstring> args;
			if (pDispParams && (pDispParams->cArgs > 0))
			{
				for (int i = pDispParams->cArgs - 1; i >= 0; --i)
				{
					VARIANTARG &rgvarg = pDispParams->rgvarg[i];
					if (rgvarg.vt == VT_BSTR)
					{
						SStringW arg(rgvarg.bstrVal);
						arg.Trim();
						args.push_back(std::wstring(arg));
					}
					else
					{
						args.push_back(std::wstring(L""));
					}
				}
			}

			m_sc->CallCPlusPlus(dispIdMember, args, pVarResult);
		}

        return S_OK;
    }

    STDMETHODIMP SJsCallCppIDispatch::GetIDsOfNames(/* [in] */ REFIID riid, /* [size_is][in] */ OLECHAR** rgszNames, /* [in] */ UINT cNames, /* [in] */ LCID lcid, /* [size_is][out] */ DISPID* rgDispId)
    {
        HRESULT hr = S_OK;
        // Assume some degree of success
        for (UINT i = 0; i < cNames; i++)
		{
			if (!m_sc || !m_sc->FunctionNameToDispatchId(rgszNames[i], rgDispId + i))
			{
				// One or more are unknown so set the return code accordingly
				hr = ResultFromScode(DISP_E_UNKNOWNNAME);
				rgDispId[i] = DISPID_UNKNOWN;
			}
        }
        return hr;
    }

    STDMETHODIMP SJsCallCppIDispatch::GetTypeInfo(/* [in] */ UINT /*iTInfo*/, /* [in] */ LCID /*lcid*/, /* [out] */ ITypeInfo** /*ppTInfo*/)
    {
        return E_NOTIMPL;
    }

    STDMETHODIMP SJsCallCppIDispatch::GetTypeInfoCount(UINT* /*pctinfo*/)
    {
        return E_NOTIMPL;
    }

    VARIANT ExecuteScript(IWebBrowser2 *pWebBrowser, const SStringW & fun,SArray<SStringW> & params)
    {
        VARIANT varErr;
        VariantInit( &varErr );

        //get document dispatch interface
        IDispatch* pDisp = NULL;

        HRESULT hr = pWebBrowser->get_Document( &pDisp );

        if ( FAILED( hr ) || pDisp == NULL )
        {
            return varErr;
        }

        IHTMLDocument2* pDoc = NULL;
        pDisp->QueryInterface( IID_IHTMLDocument2,(void**)&pDoc );
        pDisp->Release();

        IDispatch* pScript = NULL;
        hr = pDoc->get_Script( &pScript );
        pDoc->Release();

        if ( FAILED( hr ) || pScript == NULL )
        {
            return varErr;
        }

        sbstr bstrMember((int)fun.GetLength(),fun );
        DISPID dispid = 0;
        BSTR bstr = (BSTR)bstrMember;
        hr = pScript->GetIDsOfNames( IID_NULL,&(bstr),1,LOCALE_SYSTEM_DEFAULT,&dispid );

        if ( FAILED( hr ) )
        {
            return varErr;
        }

        DISPPARAMS dispparams;
        ::ZeroMemory( &dispparams,sizeof( DISPPARAMS ) );
        dispparams.cArgs = (UINT)params.GetCount();
        dispparams.rgvarg = new VARIANT[dispparams.cArgs];
        dispparams.cNamedArgs = 0;

        for ( size_t i = 0;i < params.GetCount();i++ )
        {
            size_t indx = params.GetCount() - i - 1;
            sbstr bstrParam((int)params[indx].GetLength(),params[indx]);
            dispparams.rgvarg[i].bstrVal = bstrParam.Release();
            dispparams.rgvarg[i].vt = VT_BSTR;
        }

        EXCEPINFO excepinfo;
        ::ZeroMemory( &excepinfo,sizeof( EXCEPINFO ) );
        VARIANT varRet;
        UINT nArgErr = (UINT)-1;	//initialize to invalid arg

        hr = pScript->Invoke( dispid,IID_NULL,0,DISPATCH_METHOD,&dispparams,&varRet,&excepinfo,&nArgErr );

        delete []dispparams.rgvarg;
        pScript->Release();

        if ( FAILED( hr ) )
        {
            return varErr;
        }

        return varRet;
    }

}
