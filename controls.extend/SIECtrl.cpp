#include "StdAfx.h"
#include "SIECtrl.h"
#include "SDocHostUIHandler.h"
namespace SOUI
{

    SIECtrl::SIECtrl(void):m_dwCookie(0),m_eventDispatch(NULL),m_hIEWnd(NULL)
        ,m_bDisableScrollbar(FALSE)
        ,m_bDisable3DBorder(FALSE)
        ,m_bDisableContextMenu(FALSE)
		,m_bDisableTextSelect(FALSE)
    {
        m_clsid=CLSID_WebBrowser;
    }

    SIECtrl::~SIECtrl(void)
    {
        RegisterEventHandler(FALSE);
    }

    HRESULT SIECtrl::RegisterEventHandler( BOOL inAdvise )
    {
        SComQIPtr<IConnectionPointContainer>  pCPC=m_pIE;
        if(!pCPC) return S_FALSE;
        SComQIPtr<IConnectionPoint> pCP;
        pCPC->FindConnectionPoint(DIID_DWebBrowserEvents2,&pCP);
        if(!pCP) return S_FALSE;
        HRESULT hr=S_FALSE;
        if (inAdvise)
        {
            hr = pCP->Advise(&m_eventDispatch, &m_dwCookie);
        }
        else
        {
            hr = pCP->Unadvise(m_dwCookie);
        }
        return hr; 
    }

    STDMETHODIMP SIECtrlEvtDispatch::Invoke( /* [in] */ DISPID dispIdMember, /* [in] */ REFIID riid, /* [in] */ LCID lcid, /* [in] */ WORD wFlags, /* [out][in] */ DISPPARAMS *pDispParams, /* [out] */ VARIANT *pVarResult, /* [out] */ EXCEPINFO *pExcepInfo, /* [out] */ UINT *puArgErr )
    {
        if ((riid != IID_NULL) || !m_pEventHandler)
            return E_INVALIDARG;

        switch(dispIdMember)
        {
        case DISPID_BEFORENAVIGATE2:
            m_pEventHandler->BeforeNavigate2(
                pDispParams->rgvarg[6].pdispVal,
                pDispParams->rgvarg[5].pvarVal,
                pDispParams->rgvarg[4].pvarVal,
                pDispParams->rgvarg[3].pvarVal,
                pDispParams->rgvarg[2].pvarVal,
                pDispParams->rgvarg[1].pvarVal,
                pDispParams->rgvarg[0].pboolVal);
            break;
        case DISPID_COMMANDSTATECHANGE:
            m_pEventHandler->CommandStateChange(
                pDispParams->rgvarg[1].lVal,
                pDispParams->rgvarg[0].boolVal);
            break;
        case DISPID_NAVIGATECOMPLETE2:
            m_pEventHandler->NavigateComplete2(
                pDispParams->rgvarg[1].pdispVal,
                pDispParams->rgvarg[0].pvarVal);
            break;
        case DISPID_NAVIGATEERROR:
            m_pEventHandler->NavigateError(
                pDispParams->rgvarg[4].pdispVal,
                pDispParams->rgvarg[3].pvarVal,
                pDispParams->rgvarg[2].pvarVal,
                pDispParams->rgvarg[1].pvarVal,
                pDispParams->rgvarg[0].pboolVal);
            break;
        case DISPID_DOCUMENTCOMPLETE:
            m_pEventHandler->DocumentComplete(
                pDispParams->rgvarg[1].pdispVal,
                pDispParams->rgvarg[0].pvarVal);
            break;
        case DISPID_STATUSTEXTCHANGE:		
			m_pEventHandler->StatusTextChange(
				pDispParams->rgvarg->pdispVal,
				pDispParams->rgvarg);
           /* m_pEventHandler->StatusTextChange(
                pDispParams->rgvarg[1].pdispVal,
                pDispParams->rgvarg[0].pvarVal);*/
            break;
        case DISPID_NEWWINDOW2:
            break;
        case DISPID_NEWWINDOW3:
            break;
        default:
            return DISP_E_MEMBERNOTFOUND;
        }
        return S_OK;
    }

    void SIECtrl::OnAxActivate( IUnknown *pUnknwn )
    {
        m_pIE=pUnknwn;
        if(m_pIE)
        {
            RegisterEventHandler(TRUE);

			SDocHostUIHandler *pDocHostUIHandler = new SDocHostUIHandler;
			pDocHostUIHandler->SetDisableScrollBar(m_bDisableScrollbar);
			pDocHostUIHandler->SetDisable3DBorder(m_bDisable3DBorder);
            pDocHostUIHandler->SetDisableContextMenu(m_bDisableContextMenu);
			pDocHostUIHandler->SetDisableTextSelect(m_bDisableTextSelect);
			SetExternalUIHandler(pDocHostUIHandler);
			pDocHostUIHandler->Release();

			Navigate(m_strUrl);            
            SComQIPtr<IOleWindow> ole_window=m_pIE;
            SASSERT(ole_window);
            ole_window->GetWindow(&m_hIEWnd);
        }
    }

    int SIECtrl::OnCreate( LPVOID )
    {
        int nRet=__super::OnCreate(NULL);
        if(GetContainer()->IsTranslucent())
        {
            STRACE(_T("warning!!! create iectrl failed bacause of host is translucent!"));
            return -1;
        }
        GetContainer()->GetMsgLoop()->AddMessageFilter(this);
        return nRet;
    }

    void SIECtrl::OnDestroy()
    {
        GetContainer()->GetMsgLoop()->RemoveMessageFilter(this);
		__super::OnDestroy();
    }

    BOOL SIECtrl::PreTranslateMessage( MSG* pMsg )
    {
        BOOL bRet = FALSE;
        if(!m_pIE) return FALSE;
        if(!IsVisible(TRUE)) return FALSE;       

        HWND hHost = GetContainer()->GetHostHwnd();
        //检查宿主窗口可见
        if(!::IsWindowVisible(hHost)) return FALSE;
        //检查宿主窗口激活
        if(::GetActiveWindow() != hHost) return FALSE;
        //检查当前焦点窗口是IE窗口或者子窗口
        HWND hFocus =::GetFocus();
        while(hFocus)
        {
            if(hFocus == m_hIEWnd) break;
            hFocus = ::GetParent(hFocus);
        }
        if(!hFocus ) return FALSE;
        
        // give HTML page a chance to translate this message
        SComQIPtr<IOleInPlaceActiveObject> spInPlaceActiveObject(m_pIE);
        if(spInPlaceActiveObject)
        {
            bRet=(spInPlaceActiveObject->TranslateAccelerator(pMsg) == S_OK);
        }
        return bRet;
    }

	BOOL SIECtrl::Navigate( const SStringW & strUrl )
	{
		m_strUrl = strUrl;
		m_pIE->put_Silent(VARIANT_TRUE);//
		return S_OK == m_pIE->Navigate(bstr_t(strUrl),NULL,NULL,NULL,NULL);
	}

}
