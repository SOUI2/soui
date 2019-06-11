#include "souistd.h"
#include "control/SRichEdit.h"
#include "SApp.h"
#include "helper/SMenu.h"
#include "helper/SplitString.h"
#include "helper/mybuffer.h"
#include <gdialpha.h>
#include <WinSCard.h>

#pragma comment(lib,"imm32.lib")

#ifndef LY_PER_INCH
#define LY_PER_INCH 1440
#endif

#ifndef HIMETRIC_PER_INCH
#define HIMETRIC_PER_INCH 2540
#endif

namespace SOUI
{

    //////////////////////////////////////////////////////////////////////////
    //  STextServiceHelper
    
    STextServiceHelper::STextServiceHelper()
    {
        m_rich20=LoadLibrary(_T("riched20.dll"));
        if(m_rich20) m_funCreateTextServices= (PCreateTextServices)GetProcAddress(m_rich20,"CreateTextServices");
    }

    STextServiceHelper::~STextServiceHelper()
    {
        if(m_rich20) FreeLibrary(m_rich20);
        m_funCreateTextServices=NULL;
    }

    HRESULT STextServiceHelper::CreateTextServices(IUnknown *punkOuter, ITextHost *pITextHost, IUnknown **ppUnk)
    {
        if(!m_funCreateTextServices) return E_NOTIMPL;
        return m_funCreateTextServices(punkOuter,pITextHost,ppUnk);
    }

    
    //////////////////////////////////////////////////////////////////////////
    //
    class SRicheditDropTarget : public IDropTarget
    {
    public:
        SRicheditDropTarget(ITextServices *pTxtSvr)
            :nRef(1)
            ,pserv(pTxtSvr)
        {
            SASSERT(pserv);
            pserv->AddRef();
        }

        ~SRicheditDropTarget()
        {
            SASSERT(pserv);
            pserv->Release();
        }

        //IUnkown
        virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject)
        {
            HRESULT hr=E_NOINTERFACE;
            if(riid==__uuidof(IUnknown))
                *ppvObject=(IUnknown*) this,hr=S_OK;
            else if(riid==__uuidof(IDropTarget))
                *ppvObject=(IDropTarget*)this,hr=S_OK;
            if(SUCCEEDED(hr)) AddRef();
            return hr;
        }

        virtual ULONG STDMETHODCALLTYPE AddRef( void){return ++nRef;}

        virtual ULONG STDMETHODCALLTYPE Release( void) { 
            ULONG uRet= -- nRef;
            if(uRet==0) delete this;
            return uRet;
        }

        //IDropTarget
        virtual HRESULT STDMETHODCALLTYPE DragEnter( 
            /* [unique][in] */ IDataObject *pDataObj,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ POINTL pt,
            /* [out][in] */ DWORD *pdwEffect)
        {
            HRESULT hr=S_FALSE;
            IDropTarget *pDropTarget=NULL;
            hr=pserv->TxGetDropTarget(&pDropTarget);
            if(SUCCEEDED(hr))
            {
                hr=pDropTarget->DragEnter(pDataObj,grfKeyState,pt,pdwEffect);
                *pdwEffect = DROPEFFECT_COPY;
                pDropTarget->Release();
            }
            return hr;
        }

        virtual HRESULT STDMETHODCALLTYPE DragOver( 
            /* [in] */ DWORD grfKeyState,
            /* [in] */ POINTL pt,
            /* [out][in] */ DWORD *pdwEffect)    
        {
            HRESULT hr=S_FALSE;
            IDropTarget *pDropTarget=NULL;
            hr=pserv->TxGetDropTarget(&pDropTarget);
            if(SUCCEEDED(hr))
            {
                hr=pDropTarget->DragOver(grfKeyState,pt,pdwEffect);
                *pdwEffect = DROPEFFECT_COPY;
                pDropTarget->Release();
            }
            return hr;
        }


        virtual HRESULT STDMETHODCALLTYPE DragLeave( void) 
        {
            HRESULT hr=S_FALSE;
            IDropTarget *pDropTarget=NULL;
            hr=pserv->TxGetDropTarget(&pDropTarget);
            if(SUCCEEDED(hr))
            {
                hr=pDropTarget->DragLeave();
                pDropTarget->Release();
            }
            return hr;
        }


        virtual HRESULT STDMETHODCALLTYPE Drop( 
            /* [unique][in] */ IDataObject *pDataObj,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ POINTL pt,
            /* [out][in] */ DWORD *pdwEffect)
        {
            if(*pdwEffect == DROPEFFECT_NONE) return S_FALSE;
            HRESULT hr=S_FALSE;
            IDropTarget *pDropTarget=NULL;
            hr=pserv->TxGetDropTarget(&pDropTarget);
            if(SUCCEEDED(hr))
            {
                hr=pDropTarget->Drop(pDataObj,grfKeyState,pt,pdwEffect);
                pDropTarget->Release();
            }
            return hr;
        }



    protected:
        ITextServices    *pserv;            // pointer to Text Services object
        LONG            nRef;
    };



const LONG cInitTextMax = (32 * 1024) - 1;
#define FValidCF(_pcf) ((_pcf)->cbSize == sizeof(CHARFORMAT2W))
#define FValidPF(_ppf) ((_ppf)->cbSize == sizeof(PARAFORMAT2))
#define TIMER_INVALIDATE    6

EXTERN_C const IID IID_ITextServices =   // 8d33f740-cf58-11ce-a89d-00aa006cadc5
{
    0x8d33f740,
    0xcf58,
    0x11ce,
    {0xa8, 0x9d, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

EXTERN_C const IID IID_ITextHost =   /* c5bdd8d0-d26e-11ce-a89e-00aa006cadc5 */
{
    0xc5bdd8d0,
    0xd26e,
    0x11ce,
    {0xa8, 0x9e, 0x00, 0xaa, 0x00, 0x6c, 0xad, 0xc5}
};

// Convert Device Pixels to Himetric
LONG DtoHimetric(LONG d, LONG dPerInch)
{
    return (LONG) MulDiv(d, HIMETRIC_PER_INCH, dPerInch);
}

// Convert Himetric Device pixels
LONG HimetrictoD(LONG lHimetric, LONG dPerInch)
{
    return (LONG) MulDiv(lHimetric, dPerInch, HIMETRIC_PER_INCH);
}


STextHost::STextHost(void)
    :m_pRichEdit(NULL)
    ,cRefs(0)
    ,m_fUiActive(FALSE)
    ,pserv(NULL)
{
}

STextHost::~STextHost(void)
{
    if(pserv) pserv->Release();
}

//////////////////////////////////////////////////////////////////////////
// IUnknown
HRESULT _stdcall STextHost::QueryInterface( REFIID riid, void **ppvObject )
{
    HRESULT hr = E_NOINTERFACE;
    *ppvObject = NULL;

    if (IsEqualIID(riid, IID_IUnknown)
            || IsEqualIID(riid, IID_ITextHost))
    {
        AddRef();
        *ppvObject = (ITextHost *) this;
        hr = S_OK;
    }

    return hr;
}

ULONG _stdcall STextHost::AddRef( void )
{
    return ++cRefs;
}

ULONG _stdcall STextHost::Release( void )
{
    ULONG c_Refs = --cRefs;

    if (c_Refs == 0)
    {
        delete this;
    }

    return c_Refs;
}


//////////////////////////////////////////////////////////////////////////
// ITextHost
HRESULT STextHost::TxGetViewInset( LPRECT prc )
{
    *prc=m_pRichEdit->m_rcInset;
    return S_OK;
}

HRESULT STextHost::TxGetCharFormat( const CHARFORMATW **ppCF )
{
    *ppCF=&m_pRichEdit->m_cfDef;
    return S_OK;
}


HRESULT STextHost::TxGetParaFormat( const PARAFORMAT **ppPF )
{
    *ppPF=&m_pRichEdit->m_pfDef;
    return S_OK;
}

HRESULT STextHost::TxGetClientRect( LPRECT prc )
{
    m_pRichEdit->GetClientRect(prc);
    return S_OK;
}

HRESULT STextHost::TxDeactivate( LONG lNewState )
{
    m_fUiActive=FALSE;
    return S_OK;
}

HRESULT STextHost::TxActivate( LONG * plOldState )
{
    *plOldState = m_fUiActive;
    m_fUiActive=TRUE;
    return S_OK;
}

BOOL STextHost::TxClientToScreen( LPPOINT lppt )
{
    return ::ClientToScreen(m_pRichEdit->GetContainer()->GetHostHwnd(),lppt);
}

BOOL STextHost::TxScreenToClient( LPPOINT lppt )
{
    return ::ScreenToClient(m_pRichEdit->GetContainer()->GetHostHwnd(),lppt);
}

void STextHost::TxSetCursor( HCURSOR hcur, BOOL fText )
{
    ::SetCursor(hcur);
}

void STextHost::TxSetFocus()
{
    m_pRichEdit->SetFocus();
}

void STextHost::TxSetCapture( BOOL fCapture )
{
    if(fCapture)
        m_pRichEdit->SetCapture();
    else
        m_pRichEdit->ReleaseCapture();
}

void STextHost::TxScrollWindowEx( INT dx, INT dy, LPCRECT lprcScroll, LPCRECT lprcClip, HRGN hrgnUpdate, LPRECT lprcUpdate, UINT fuScroll )
{
    m_pRichEdit->Invalidate();
}

void STextHost::TxKillTimer( UINT idTimer )
{
   m_pRichEdit->KillTimer2(idTimer);
}

BOOL STextHost::TxSetTimer( UINT idTimer, UINT uTimeout )
{
   return m_pRichEdit->SetTimer2(idTimer,uTimeout);
}

BOOL STextHost::TxSetCaretPos( INT x, INT y )
{
    m_ptCaret.x=x,m_ptCaret.y=y;
    return m_pRichEdit->GetContainer()->OnSetCaretPos(x,y);
}

BOOL STextHost::TxShowCaret( BOOL fShow )
{
    if(fShow && !m_fUiActive) return FALSE;
    return m_pRichEdit->GetContainer()->OnShowCaret(fShow);
}

BOOL STextHost::TxCreateCaret( HBITMAP hbmp, INT xWidth, INT yHeight )
{
    return m_pRichEdit->CreateCaret(hbmp,xWidth,yHeight);
}

HDC STextHost::TxGetDC()
{
    return ::GetDC(NULL);
}

INT STextHost::TxReleaseDC( HDC hdc )
{
    return ::ReleaseDC(NULL,hdc);
}

BOOL STextHost::TxShowScrollBar( INT fnBar, BOOL fShow )
{
    int wBar=0;
    switch(fnBar)
    {
    case SB_BOTH:
        wBar=SSB_BOTH;
        break;
    case SB_VERT:
        wBar=SSB_VERT;
        break;
    case SB_HORZ:
        wBar=SSB_HORZ;
        break;
    }
    return m_pRichEdit->ShowScrollBar(wBar,fShow);
}

BOOL STextHost::TxEnableScrollBar( INT fuSBFlags, INT fuArrowflags )
{
    int wBar=0;
    switch(fuSBFlags)
    {
    case SB_BOTH:
        wBar=SSB_BOTH;
        break;
    case SB_VERT:
        wBar=SSB_VERT;
        break;
    case SB_HORZ:
        wBar=SSB_HORZ;
        break;
    }
    return m_pRichEdit->EnableScrollBar(wBar,fuArrowflags==ESB_ENABLE_BOTH);
}

BOOL STextHost::TxSetScrollRange( INT fnBar, LONG nMinPos, INT nMaxPos, BOOL fRedraw )
{
    return m_pRichEdit->SetScrollRange(fnBar!=SB_HORZ,nMinPos,nMaxPos,fRedraw);
}

BOOL STextHost::TxSetScrollPos( INT fnBar, INT nPos, BOOL fRedraw )
{
    BOOL bRet=FALSE;
    if(m_pRichEdit->m_fScrollPending) return TRUE;
    m_pRichEdit->m_fScrollPending=TRUE;
    bRet= m_pRichEdit->SetScrollPos(fnBar!=SB_HORZ,nPos,fRedraw);
    m_pRichEdit->m_fScrollPending=FALSE;
    return bRet;
}

void STextHost::TxInvalidateRect( LPCRECT prc, BOOL fMode )
{
    if(prc)
    {
        m_pRichEdit->InvalidateRect(prc);
    }
    else
    {
        m_pRichEdit->Invalidate();
    }
}

void STextHost::TxViewChange( BOOL fUpdate )
{
    if(fUpdate)
    {
        m_pRichEdit->InvalidateRect(NULL);
    }
}

COLORREF STextHost::TxGetSysColor( int nIndex )
{
    return ::GetSysColor(nIndex);
}

HRESULT STextHost::TxGetBackStyle( TXTBACKSTYLE *pstyle )
{
    *pstyle=TXTBACK_TRANSPARENT;
    return S_OK;
}

HRESULT STextHost::TxGetMaxLength( DWORD *plength )
{
    *plength = m_pRichEdit->m_cchTextMost;
    return S_OK;
}

HRESULT STextHost::TxGetScrollBars( DWORD *pdwScrollBar )
{
    *pdwScrollBar =  m_pRichEdit->m_dwStyle & (WS_VSCROLL | WS_HSCROLL | ES_AUTOVSCROLL |
                     ES_AUTOHSCROLL | ES_DISABLENOSCROLL);

    return S_OK;
}

HRESULT STextHost::TxGetPasswordChar( TCHAR *pch )
{
    *pch=m_pRichEdit->m_chPasswordChar;
    return S_OK;
}

HRESULT STextHost::TxGetAcceleratorPos( LONG *pcp )
{
    *pcp=m_pRichEdit->m_lAccelPos;
    return S_OK;
}

HRESULT STextHost::TxGetExtent( LPSIZEL lpExtent )
{
    *lpExtent=m_pRichEdit->m_sizelExtent;
    return S_OK;
}

HRESULT STextHost::OnTxCharFormatChange( const CHARFORMATW * pcf )
{
    return S_OK;
}

HRESULT STextHost::OnTxParaFormatChange( const PARAFORMAT * ppf )
{
    return S_OK;
}

HRESULT STextHost::TxGetPropertyBits( DWORD dwMask, DWORD *pdwBits )
{
    DWORD dwProperties = 0;

    if (m_pRichEdit->m_fRich)
    {
        dwProperties = TXTBIT_RICHTEXT;
    }

    if (m_pRichEdit->m_dwStyle & ES_MULTILINE)
    {
        dwProperties |= TXTBIT_MULTILINE;
    }

    if (m_pRichEdit->m_dwStyle & ES_READONLY)
    {
        dwProperties |= TXTBIT_READONLY;
    }


    if (m_pRichEdit->m_dwStyle & ES_PASSWORD)
    {
        dwProperties |= TXTBIT_USEPASSWORD;
    }

    if (!(m_pRichEdit->m_dwStyle & ES_NOHIDESEL))
    {
        dwProperties |= TXTBIT_HIDESELECTION;
    }

    if (m_pRichEdit->m_fEnableAutoWordSel)
    {
        dwProperties |= TXTBIT_AUTOWORDSEL;
    }

    if (m_pRichEdit->m_fVertical)
    {
        dwProperties |= TXTBIT_VERTICAL;
    }

    if (m_pRichEdit->m_fWordWrap)
    {
        dwProperties |= TXTBIT_WORDWRAP;
    }

    if (m_pRichEdit->m_fAllowBeep)
    {
        dwProperties |= TXTBIT_ALLOWBEEP;
    }

    if (m_pRichEdit->m_fSaveSelection)
    {
        dwProperties |= TXTBIT_SAVESELECTION;
    }

    *pdwBits = dwProperties & dwMask;
    return NOERROR;
}

HRESULT STextHost::TxNotify( DWORD iNotify, void *pv )
{   
    return m_pRichEdit->OnTxNotify(iNotify,pv);
}

HIMC STextHost::TxImmGetContext()
{
    return ImmGetContext(m_pRichEdit->GetContainer()->GetHostHwnd());
}

void STextHost::TxImmReleaseContext( HIMC himc )
{
    ImmReleaseContext(m_pRichEdit->GetContainer()->GetHostHwnd(),himc);
}

HRESULT STextHost::TxGetSelectionBarWidth( LONG *plSelBarWidth )
{
    *plSelBarWidth=0;
    return S_OK;
}

BOOL STextHost::Init(SRichEdit* pRichEdit)
{
    IUnknown *pUnk;
    HRESULT hr;

    m_pRichEdit=pRichEdit;

    // Create Text Services component
    if(FAILED(STextServiceHelper::getSingleton().CreateTextServices(NULL, this, &pUnk))) return FALSE;

    hr = pUnk->QueryInterface(IID_ITextServices,(void **)&pserv);

    pUnk->Release();

    return SUCCEEDED(hr);
}


//////////////////////////////////////////////////////////////////////////
// dui interface

SRichEdit::SRichEdit()
    :m_pTxtHost(NULL)
    ,m_fTransparent(0)
    ,m_fRich(1)
    ,m_fSaveSelection(TRUE)
    ,m_fVertical(FALSE)
    ,m_fWordWrap(FALSE)
    ,m_fAllowBeep(FALSE)
    ,m_fEnableAutoWordSel(TRUE)
    ,m_fWantTab(FALSE)
    ,m_fSingleLineVCenter(TRUE)
    ,m_fScrollPending(FALSE)
    ,m_fEnableDragDrop(FALSE)
    ,m_fAutoSel(FALSE)
    ,m_fNotifyChange(FALSE)
    ,m_cchTextMost(cInitTextMax)
    ,m_chPasswordChar(_T('*'))
    ,m_lAccelPos(-1)
    ,m_dwStyle(ES_LEFT|ES_AUTOHSCROLL)
    ,m_byDbcsLeadByte(0)
	, m_hCurIMC(NULL)
{
    m_pNcSkin = GETBUILTINSKIN(SKIN_SYS_BORDER);

    m_bFocusable  = TRUE;
    m_bClipClient = TRUE;
    m_sizelExtent.cx=m_sizelExtent.cy=0;
    m_evtSet.addEvent(EVENTID(EventRENotify));
    m_evtSet.addEvent(EVENTID(EventREMenu));
	m_evtSet.addEvent(EVENTID(EventKeyDown));
}


int SRichEdit::OnCreate( LPVOID )
{
    if(0 != __super::OnCreate(NULL)) return 1;

    InitDefaultCharFormat(&m_cfDef);
    InitDefaultParaFormat(&m_pfDef);

    m_pTxtHost=new STextHost;
    m_pTxtHost->AddRef();
    if(!m_pTxtHost->Init(this))
    {
        m_pTxtHost->Release();
        m_pTxtHost=NULL;
        return 1;
    }

    if(!m_fTransparent && m_style.m_crBg==CR_INVALID && !m_pBgSkin) 
        m_style.m_crBg=0xFFFFFF; 
    //inplace activate
    m_pTxtHost->GetTextService()->OnTxInPlaceActivate(NULL);
    //默认没有焦点
    m_pTxtHost->m_fUiActive=FALSE;
    m_pTxtHost->GetTextService()->OnTxUIDeactivate();
    m_pTxtHost->GetTextService()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);

    // set IME
    DWORD dw = (DWORD)SSendMessage(EM_GETLANGOPTIONS);
    dw |= IMF_AUTOKEYBOARD | IMF_DUALFONT | IMF_UIFONTS;
    dw &= ~IMF_AUTOFONT;
    SSendMessage(EM_SETLANGOPTIONS, 0, dw);
    
    if(m_fNotifyChange)
    {//receive enm_change event
        SSendMessage(EM_SETEVENTMASK,0,ENM_CHANGE);
    }
    
    if(m_strRtfSrc.IsEmpty())
        SetWindowText(SWindow::GetWindowText());
    else
        SetAttribute(L"rtf",m_strRtfSrc,FALSE);
    //register droptarget
    OnEnableDragDrop( !(m_dwStyle&ES_READONLY) & m_fEnableDragDrop);
    return 0;
}

void SRichEdit::OnDestroy()
{
    KillFocus();
    OnEnableDragDrop(FALSE);
    
    if(m_pTxtHost)
    {
        m_pTxtHost->GetTextService()->OnTxInPlaceDeactivate();
        m_pTxtHost->Release();
        m_pTxtHost=NULL;
    }
    __super::OnDestroy();
}


void SRichEdit::OnPaint( IRenderTarget * pRT )
{
    CRect rcClient;
    GetClientRect(&rcClient);
    pRT->PushClipRect(&rcClient);
    HDC hdc=pRT->GetDC(0);
	int nOldMode = ::SetGraphicsMode(hdc, GM_COMPATIBLE);	//richedit需要将GraphicMode强制设置为GM_COMPATIBLE

    ALPHAINFO ai;
    CGdiAlpha::AlphaBackup(hdc,&rcClient,ai);

    LONG lPos =0;
    m_pTxtHost->GetTextService()->TxGetVScroll(NULL,NULL,&lPos,NULL,NULL);
    RECTL rcL= {rcClient.left,rcClient.top,rcClient.right,rcClient.bottom};
    m_pTxtHost->GetTextService()->TxDraw(
        DVASPECT_CONTENT,          // Draw Aspect
        /*-1*/0,                        // Lindex
        NULL,                    // Info for drawing optimazation
        NULL,                    // target device information
        hdc,            // Draw device HDC
        NULL,                        // Target device HDC
        &rcL,            // Bounding client rectangle
        NULL,             // Clipping rectangle for metafiles
        &rcClient,        // Update rectangle
        NULL,                        // Call back function
        NULL,                    // Call back parameter
        TXTVIEW_ACTIVE);

    CGdiAlpha::AlphaRestore(ai);
	::SetGraphicsMode(hdc, nOldMode);

    pRT->ReleaseDC(hdc);
    pRT->PopClip();
}

void SRichEdit::OnSetFocus(SWND wndOld)
{
    __super::OnSetFocus(wndOld);

    if(m_pTxtHost && IsVisible(TRUE) && !IsDisabled(TRUE))
    {
        m_pTxtHost->m_fUiActive=TRUE;
        m_pTxtHost->GetTextService()->OnTxUIActivate();
        m_pTxtHost->GetTextService()->TxSendMessage(WM_SETFOCUS, 0, 0, 0);
        if(m_fAutoSel) SetSel((DWORD)MAKELONG(0,-1),TRUE);
    }

	if (ES_PASSWORD & m_dwStyle || ES_NUMBER & m_dwStyle)
	{
		m_hCurIMC = ImmAssociateContext(GetContainer()->GetHostHwnd(), NULL);
	}
}

void SRichEdit::OnKillFocus(SWND wndFocus)
{
    __super::OnKillFocus(wndFocus);
    if(m_pTxtHost)
    {
        m_pTxtHost->m_fUiActive=FALSE;
        m_pTxtHost->GetTextService()->OnTxUIDeactivate();
        m_pTxtHost->GetTextService()->TxSendMessage(WM_KILLFOCUS, 0, 0, 0);
        m_pTxtHost->TxShowCaret(FALSE);
    }

	if (ES_PASSWORD & m_dwStyle || ES_NUMBER & m_dwStyle)
	{
		ImmAssociateContext(GetContainer()->GetHostHwnd(), m_hCurIMC);
	}
}

void SRichEdit::OnTimer( char idEvent )
{
    if(idEvent==TIMER_INVALIDATE)
    {
        Invalidate();
        KillTimer(idEvent);
    }
    else
    {
        __super::OnTimer(idEvent);
    }
}

void SRichEdit::OnTimer2( UINT_PTR idEvent )
{
    m_pTxtHost->GetTextService()->TxSendMessage(WM_TIMER,idEvent,0,NULL);
}

/**
* SRichEdit::OnGetDlgCode
* @brief    获取窗口消息码
* @return   返回UINT
*
* Describe  获取窗口消息码
*/

UINT SRichEdit::OnGetDlgCode()
{
	UINT uRet = SC_WANTCHARS | SC_WANTARROWS;
	if (m_fWantTab) uRet |= SC_WANTTAB;
	if (m_dwStyle&ES_WANTRETURN) uRet |= SC_WANTRETURN;
	return uRet;
}

BOOL SRichEdit::OnScroll( BOOL bVertical,UINT uCode,int nPos )
{
    if(m_fScrollPending) return FALSE;
    LRESULT lresult=-1;
    m_fScrollPending=TRUE;
    SPanel::OnScroll(bVertical,uCode,nPos);
       
    m_pTxtHost->GetTextService()->TxSendMessage(bVertical?WM_VSCROLL:WM_HSCROLL,MAKEWPARAM(uCode,nPos),0,&lresult);
    LONG lPos=0;
    if(bVertical)
    {
        m_pTxtHost->GetTextService()->TxGetVScroll(NULL,NULL,&lPos,NULL,NULL);
    }else
    {
        m_pTxtHost->GetTextService()->TxGetHScroll(NULL,NULL,&lPos,NULL,NULL);
    }
    if(lPos != GetScrollPos(bVertical))
        SetScrollPos(bVertical,lPos,TRUE);
    
    m_fScrollPending=FALSE;
    if(uCode==SB_THUMBTRACK)
        ScrollUpdate();
    return lresult==0;
}

BOOL SRichEdit::OnSetCursor(const CPoint &pt)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    if(!rcClient.PtInRect(pt))
        return FALSE;

    HDC hdc=GetDC(GetContainer()->GetHostHwnd());
    m_pTxtHost->GetTextService()->OnTxSetCursor(
        DVASPECT_CONTENT,
        -1,
        NULL,
        NULL,
        hdc,
        NULL,
        &rcClient,
        pt.x,
        pt.y);
    ReleaseDC(GetContainer()->GetHostHwnd(),hdc);
    return TRUE;
}

BOOL SRichEdit::SwndProc( UINT uMsg,WPARAM wParam,LPARAM lParam,LRESULT & lResult )
{
    if(m_pTxtHost && m_pTxtHost->GetTextService())
    {
        if(uMsg == EM_GETRECT)
        {
            SetMsgHandled(TRUE);
            GetClientRect((LPRECT)lParam);
            return TRUE;
        }
        if(m_pTxtHost->GetTextService()->TxSendMessage(uMsg,wParam,lParam,&lResult)==S_OK)
        {
            SetMsgHandled(TRUE);
            return TRUE;
        }
    }
    return __super::SwndProc(uMsg,wParam,lParam,lResult);
}

HRESULT SRichEdit::InitDefaultCharFormat( CHARFORMAT2W* pcf ,IFont *pFont)
{
    CAutoRefPtr<IRenderTarget> pRT;
    GETRENDERFACTORY->CreateRenderTarget(&pRT,0,0);
    SASSERT(pRT);
    BeforePaintEx(pRT);
    
    CAutoRefPtr<IFont> oldFont;
    if(pFont==NULL) pFont=(IFont *)pRT->GetCurrentObject(OT_FONT);
    SIZE szTxt;
    pRT->MeasureText(_T("A"),1,&szTxt);
	m_nFontHeight=szTxt.cy;

    memset(pcf, 0, sizeof(CHARFORMAT2W));
    pcf->cbSize = sizeof(CHARFORMAT2W);
    pcf->dwMask = CFM_SIZE | CFM_OFFSET | CFM_FACE | CFM_CHARSET | CFM_COLOR | CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE;

    pcf->crTextColor = pRT->GetTextColor() & 0x00ffffff;
    HDC hdc=GetDC(NULL);
    LONG yPixPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL,hdc);
    const LOGFONT *plf=pFont->LogFont();
    pcf->yHeight = abs(MulDiv(pFont->TextSize(), LY_PER_INCH, yPixPerInch));
    pcf->yOffset = 0;
    pcf->dwEffects = 0;
    if(pFont->IsBold())
        pcf->dwEffects |= CFE_BOLD;
    if(pFont->IsItalic())
        pcf->dwEffects |= CFE_ITALIC;
    if(pFont->IsUnderline())
        pcf->dwEffects |= CFE_UNDERLINE;
    pcf->bCharSet = plf->lfCharSet;
    pcf->bPitchAndFamily = plf->lfPitchAndFamily;
#ifdef _UNICODE
    wcscpy(pcf->szFaceName, plf->lfFaceName);
#else
    //need to thunk pcf->szFaceName to a standard char string.in this case it's easy because our thunk is also our copy
    MultiByteToWideChar(CP_ACP, 0, plf->lfFaceName, LF_FACESIZE, pcf->szFaceName, LF_FACESIZE) ;
#endif

    return S_OK;
}

HRESULT SRichEdit::InitDefaultParaFormat( PARAFORMAT2* ppf )
{
    memset(ppf, 0, sizeof(PARAFORMAT2));
    ppf->cbSize = sizeof(PARAFORMAT2);
    ppf->dwMask = PFM_ALL;
    ppf->cTabCount = 1;
    ppf->rgxTabs[0] = lDefaultTab;

    if(m_dwStyle&ES_CENTER)
        ppf->wAlignment=PFA_CENTER;
    else if(m_dwStyle&ES_RIGHT)
        ppf->wAlignment=PFA_RIGHT;
    else
        ppf->wAlignment = PFA_LEFT;

    return S_OK;
}



HRESULT SRichEdit::OnTxNotify( DWORD iNotify,LPVOID pv )
{
    EventRENotify evt(this);
    evt.iNotify=iNotify;
    evt.pv=pv;
    evt.hr = S_OK;
    FireEvent(evt);
    return evt.hr;
}
//////////////////////////////////////////////////////////////////////////
//    richedit interfaces
BOOL SRichEdit::GetWordWrap( void )
{
    return m_fWordWrap;
}

void SRichEdit::SetWordWrap( BOOL fWordWrap )
{
    m_fWordWrap = fWordWrap;
    m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_WORDWRAP, fWordWrap ? TXTBIT_WORDWRAP : 0);
}

BOOL SRichEdit::GetReadOnly()
{
    return (m_dwStyle & ES_READONLY) != 0;
}

BOOL SRichEdit::SetReadOnly(BOOL bReadOnly)
{
    return 0 != SSendMessage(EM_SETREADONLY, bReadOnly);
}

LONG SRichEdit::GetLimitText()
{
    return m_cchTextMost;
}

BOOL SRichEdit::SetLimitText(int nLength)
{
    return 0 != SSendMessage(EM_EXLIMITTEXT, nLength);
}

WORD SRichEdit::GetDefaultAlign()
{
    return m_pfDef.wAlignment;
}

void SRichEdit::SetDefaultAlign( WORD wNewAlign )
{
    m_pfDef.wAlignment = wNewAlign;

    // Notify control of property change
    m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL SRichEdit::GetRichTextFlag()
{
    return m_fRich;
}

void SRichEdit::SetRichTextFlag( BOOL fRich )
{
    m_fRich = fRich;

    m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_RICHTEXT,
            fRich ? TXTBIT_RICHTEXT : 0);
}

LONG SRichEdit::GetDefaultLeftIndent()
{
    return m_pfDef.dxOffset;
}

void SRichEdit::SetDefaultLeftIndent( LONG lNewIndent )
{
    m_pfDef.dxOffset = lNewIndent;

    m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
}

BOOL SRichEdit::SetSaveSelection( BOOL fSaveSelection )
{
    BOOL fResult = fSaveSelection;

    m_fSaveSelection = fSaveSelection;

    // notify text services of property change
    m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_SAVESELECTION,
            m_fSaveSelection ? TXTBIT_SAVESELECTION : 0);

    return fResult;
}

HRESULT SRichEdit::DefAttributeProc(const SStringW & strAttribName,const SStringW & strValue, BOOL bLoading)
{
    HRESULT hRet=S_FALSE;
    DWORD dwBit=0,dwMask=0;
    //hscrollbar
    if(strAttribName.CompareNoCase(L"hscrollBar")==0)
    {
        if(strValue==L"0")
            m_dwStyle&=~WS_HSCROLL;
        else
            m_dwStyle|=WS_HSCROLL;
        dwBit|=TXTBIT_SCROLLBARCHANGE;
        dwMask|=TXTBIT_SCROLLBARCHANGE;
    }
    //vscrollbar
    else if(strAttribName.CompareNoCase(L"vscrollBar")==0)
    {
        if(strValue==L"0")
            m_dwStyle&=~WS_VSCROLL;
        else
            m_dwStyle|=WS_VSCROLL;
        dwBit|=TXTBIT_SCROLLBARCHANGE;
        dwMask|=TXTBIT_SCROLLBARCHANGE;
    }
    //auto hscroll
    else if(strAttribName.CompareNoCase(L"autoHscroll")==0)
    {
        if(strValue==L"0")
            m_dwStyle&=~ES_AUTOHSCROLL;
        else
            m_dwStyle|=ES_AUTOHSCROLL;
        dwBit|=TXTBIT_SCROLLBARCHANGE;
        dwMask|=TXTBIT_SCROLLBARCHANGE;
    }
    //auto hscroll
    else if(strAttribName.CompareNoCase(L"autoVscroll")==0)
    {
        if(strValue==L"0")
            m_dwStyle&=~ES_AUTOVSCROLL;
        else
            m_dwStyle|=ES_AUTOVSCROLL;
        dwBit|=TXTBIT_SCROLLBARCHANGE;
        dwMask|=TXTBIT_SCROLLBARCHANGE;
    }
    //multilines
    else if(strAttribName.CompareNoCase(L"multiLines")==0)
    {
        if(strValue==L"0")
            m_dwStyle&=~ES_MULTILINE;
        else
            m_dwStyle|=ES_MULTILINE,dwBit|=TXTBIT_MULTILINE;
        dwMask|=TXTBIT_MULTILINE;
    }
    //readonly
    else if(strAttribName.CompareNoCase(L"readOnly")==0)
    {
        if(strValue==L"0")
            m_dwStyle&=~ES_READONLY;
        else
            m_dwStyle|=ES_READONLY,dwBit|=TXTBIT_READONLY;
        dwMask|=TXTBIT_READONLY;
        if(!bLoading)
        {//update dragdrop
            OnEnableDragDrop(!(m_dwStyle&ES_READONLY) && m_fEnableDragDrop);
        }
    }
    //want return
    else if(strAttribName.CompareNoCase(L"wantReturn")==0)
    {
        if(strValue==L"0")
            m_dwStyle&=~ES_WANTRETURN;
        else
            m_dwStyle|=ES_WANTRETURN;
    }
    //password
    else if(strAttribName.CompareNoCase(L"password")==0)
    {
        if(strValue==L"0")
            m_dwStyle&=~ES_PASSWORD;
        else
            m_dwStyle|=ES_PASSWORD,dwBit|=TXTBIT_USEPASSWORD;
        dwMask|=TXTBIT_USEPASSWORD;
    }
    //number
    else if(strAttribName.CompareNoCase(L"number")==0)
    {
        if(strValue==L"0")
            m_dwStyle&=~ES_NUMBER;
        else
            m_dwStyle|=ES_NUMBER;
    }
    //password char
    else if(strAttribName.CompareNoCase(L"passwordChar")==0)
    {
        SStringT strValueT=S_CW2T(strValue);
        m_chPasswordChar=strValueT[0];
    }
    //enabledragdrop
    else if(strAttribName.CompareNoCase(L"enableDragdrop")==0)
    {
        if(strValue==L"0")
        {
            m_fEnableDragDrop=FALSE;
        }else
        {
            m_fEnableDragDrop=TRUE;
        }
        if(!bLoading)
        {
            OnEnableDragDrop( !(m_dwStyle&ES_READONLY) & m_fEnableDragDrop);
        }
    }
    //auto Sel
    else if(strAttribName.CompareNoCase(L"autoSel")==0)
    {
        if(strValue==L"0")
        {
            m_fAutoSel=FALSE;
        }else
        {
            m_fAutoSel=TRUE;
        }
    }
    else
    {
        hRet=__super::DefAttributeProc(strAttribName,strValue,bLoading);
    }
    if(!bLoading)
    {
        m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(dwMask,dwBit);
        hRet=TRUE;
    }
    return hRet;
}

void SRichEdit::OnLButtonDown( UINT nFlags, CPoint point )
{
	SetCapture();
    if(!IsFocused())
    {
        SetFocus();
        if(!m_fAutoSel) m_pTxtHost->GetTextService()->TxSendMessage(GetCurMsg()->uMsg,GetCurMsg()->wParam,GetCurMsg()->lParam,NULL);
    }else
    {
        m_pTxtHost->GetTextService()->TxSendMessage(GetCurMsg()->uMsg,GetCurMsg()->wParam,GetCurMsg()->lParam,NULL);
    }
}


void SRichEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_pTxtHost->GetTextService()->TxSendMessage(GetCurMsg()->uMsg,GetCurMsg()->wParam,GetCurMsg()->lParam,NULL);
	ReleaseCapture();
}

LRESULT SRichEdit::OnButtonClick(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
    m_pTxtHost->GetTextService()->TxSendMessage(uMsg,wParam,lParam,NULL);
    return 0;
}


void SRichEdit::OnRButtonDown( UINT nFlags, CPoint point )
{
    if(FireCtxMenu(point)) return;//用户自己响应右键
    SetFocus();
    //弹出默认编辑窗菜单
    pugi::xml_node xmlMenu = GetMenuTemplate();
    if(xmlMenu)
    {
        SMenu menu;
        if(menu.LoadMenu(xmlMenu))
        {
            CRect rcCantainer=GetContainer()->GetContainerRect();
            point.Offset(rcCantainer.TopLeft());
            HWND hHost=GetContainer()->GetHostHwnd();
            ::ClientToScreen(hHost,&point);
            BOOL canPaste=(BOOL)SSendMessage(EM_CANPASTE,0);
            DWORD dwStart=0,dwEnd=0;
            SSendMessage(EM_GETSEL,(WPARAM)&dwStart,(LPARAM)&dwEnd);
            BOOL hasSel=dwStart<dwEnd;
            UINT uLen=(UINT)SSendMessage(WM_GETTEXTLENGTH ,0,0);
            BOOL bReadOnly=m_dwStyle&ES_READONLY;
            EnableMenuItem(menu.m_hMenu,MENU_CUT,MF_BYCOMMAND|((hasSel&&(!bReadOnly))?0:MF_GRAYED));
            EnableMenuItem(menu.m_hMenu,MENU_COPY,MF_BYCOMMAND|(hasSel?0:MF_GRAYED));
            EnableMenuItem(menu.m_hMenu,MENU_PASTE,MF_BYCOMMAND|((canPaste&&(!bReadOnly))?0:MF_GRAYED));
            EnableMenuItem(menu.m_hMenu,MENU_DEL,MF_BYCOMMAND|((hasSel&&(!bReadOnly))?0:MF_GRAYED));
            EnableMenuItem(menu.m_hMenu,MENU_SELALL,MF_BYCOMMAND|((uLen>0)?0:MF_GRAYED));

            UINT uCmd=menu.TrackPopupMenu(TPM_RETURNCMD|TPM_LEFTALIGN,point.x,point.y,hHost);
            
            EventREMenu evt(this);
            evt.uCmd = uCmd;
            FireEvent(evt);
            if(evt.handled)
                return;
                
            switch(uCmd)
            {
            case MENU_CUT:
                SSendMessage(WM_CUT);
                break;
            case MENU_COPY:
                SSendMessage(WM_COPY);
                break;
            case MENU_PASTE:
                SSendMessage(WM_PASTE);
                break;
            case MENU_DEL:
                SSendMessage(EM_REPLACESEL,0,(LPARAM)_T(""));
                break;
            case MENU_SELALL:
                SSendMessage(EM_SETSEL,0,-1);
                break;
            default:
                break;
            }

        }
    }
}

void SRichEdit::OnMouseMove( UINT nFlags, CPoint point )
{
    m_pTxtHost->GetTextService()->TxSendMessage(GetCurMsg()->uMsg,GetCurMsg()->wParam,GetCurMsg()->lParam,NULL);
}

void SRichEdit::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	EventKeyDown evt(this);
	evt.bCancel = FALSE;
	evt.nChar = nChar;
	evt.nFlags = nFlags;
	FireEvent(evt);
	if (evt.bCancel)
	{
		SetMsgHandled(FALSE);
		return;
	}

    if(nChar==VK_RETURN && !(m_dwStyle&ES_WANTRETURN) && !(GetKeyState(VK_CONTROL)&0x8000))
    {
        SetMsgHandled(FALSE);
        return;
    }
    m_pTxtHost->GetTextService()->TxSendMessage(GetCurMsg()->uMsg,GetCurMsg()->wParam,GetCurMsg()->lParam,NULL);
}

#define CTRL(_ch) (_ch - 'A' + 1)

void SRichEdit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    switch(nChar)
    {
        // Ctrl-Return generates Ctrl-J (LF), treat it as an ordinary return
    case CTRL('J'):
    case VK_RETURN:
        if(!(GetKeyState(VK_CONTROL) & 0x8000)
                && !(m_dwStyle & ES_WANTRETURN))
            return;
        break;

    case VK_TAB:
        if(!m_fWantTab && !(GetKeyState(VK_CONTROL) & 0x8000))
            return;
        break;
    default:
        if(m_dwStyle&ES_NUMBER && !isdigit(nChar) && nChar!='-' && nChar!='.' && nChar!=',')
            return;
#ifndef _UNICODE
        if(m_byDbcsLeadByte==0)
        {
            if(IsDBCSLeadByte(nChar))
            {
                m_byDbcsLeadByte=nChar;
                return;
            }
        }else
        {
            nChar=MAKEWORD(nChar,m_byDbcsLeadByte);
            m_pTxtHost->GetTextService()->TxSendMessage(WM_IME_CHAR,nChar,0,NULL);
            m_byDbcsLeadByte=0;
            return;
        }
#endif//_UNICODE
        break;
    }
    m_pTxtHost->GetTextService()->TxSendMessage(GetCurMsg()->uMsg,GetCurMsg()->wParam,GetCurMsg()->lParam,NULL);
}

LRESULT SRichEdit::OnNcCalcSize( BOOL bCalcValidRects, LPARAM lParam )
{
    __super::OnNcCalcSize(bCalcValidRects,lParam);
    

    CRect rcInsetPixel = GetStyle().GetPadding();

	if(!m_fRich && m_fSingleLineVCenter && !(m_dwStyle&ES_MULTILINE))
    {
        rcInsetPixel.top   =
        rcInsetPixel.bottom=(m_rcClient.Height()-m_nFontHeight)/2;
    }

    m_siHoz.nPage=m_rcClient.Width()-rcInsetPixel.left-rcInsetPixel.right;
    m_siVer.nPage=m_rcClient.Height()-rcInsetPixel.top-rcInsetPixel.bottom;

    if(m_pTxtHost)
    {
        HDC hdc=GetDC(GetContainer()->GetHostHwnd());
        LONG xPerInch = ::GetDeviceCaps(hdc, LOGPIXELSX);
        LONG yPerInch =    ::GetDeviceCaps(hdc, LOGPIXELSY);
        ReleaseDC(GetContainer()->GetHostHwnd(),hdc);

        m_sizelExtent.cx = DtoHimetric(m_rcClient.Width(), xPerInch);
        m_sizelExtent.cy = DtoHimetric(m_rcClient.Height(), yPerInch);

        m_rcInset.left=DtoHimetric(rcInsetPixel.left,xPerInch);
        m_rcInset.right=DtoHimetric(rcInsetPixel.right,xPerInch);
        m_rcInset.top=DtoHimetric(rcInsetPixel.top,yPerInch);
        m_rcInset.bottom=DtoHimetric(rcInsetPixel.bottom,yPerInch);
        
        //窗口有焦点时，需要更新光标位置：先使edit失活用来关闭光标，再激活edit来显示光标。
        //此处不应该直接用setfocus和killfocus，因为这两个消息可能会被外面响应。导致逻辑错误
        BOOL bFocus = IsFocused();
        if(bFocus)
        {
            m_pTxtHost->m_fUiActive=FALSE;
            m_pTxtHost->GetTextService()->OnTxUIDeactivate();
            m_pTxtHost->GetTextService()->TxSendMessage(WM_KILLFOCUS,0,0,NULL);
            m_pTxtHost->TxShowCaret(FALSE);
        }
        m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_EXTENTCHANGE|TXTBIT_CLIENTRECTCHANGE, TXTBIT_EXTENTCHANGE|TXTBIT_CLIENTRECTCHANGE);
        if(bFocus)
        {
            m_pTxtHost->m_fUiActive=TRUE;
            m_pTxtHost->GetTextService()->OnTxUIActivate();
            m_pTxtHost->GetTextService()->TxSendMessage(WM_SETFOCUS,0,0,NULL);
        }
    }
    return 0;
}

LRESULT SRichEdit::OnSetReadOnly( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return SUCCEEDED(SetAttribute(L"readonly",wParam?L"1":L"0"));
}

LRESULT SRichEdit::OnSetLimitText( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if(wParam==0) m_cchTextMost=cInitTextMax;
    else m_cchTextMost=(DWORD)wParam;
    m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_MAXLENGTHCHANGE, TXTBIT_MAXLENGTHCHANGE);
    return 1;
}

LRESULT SRichEdit::OnSetCharFormat( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if(wParam == SCF_DEFAULT && !FValidCF((CHARFORMAT2W *) lParam))
    {//设置默认字体只支持CHARFORMAT2W
        SLOGFMTD(_T("set default char format failed! only CHARFORMAT2W can be set for default char format"));
        return 0;
    }
    
    m_pTxtHost->GetTextService()->TxSendMessage(uMsg,wParam,lParam,NULL);
    if(wParam == SCF_DEFAULT)
    {
        m_cfDef=*(CHARFORMAT2W *)lParam;
        m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE,TXTBIT_CHARFORMATCHANGE);
    }
    return 1;
}

LRESULT SRichEdit::OnSetParaFormat( UINT uMsg, WPARAM wparam, LPARAM lparam )
{
    if(!FValidPF((PARAFORMAT *) lparam))
    {
        return 0;
    }

    // check to see if we're setting the default.
    // either SCF_DEFAULT will be specified *or* there is no
    // no text in the document (richedit1.0 behaviour).
    if (!(wparam & SCF_DEFAULT))
    {
        HRESULT hr = m_pTxtHost->GetTextService()->TxSendMessage(WM_GETTEXTLENGTH, 0, 0, 0);

        if (hr == 0)
        {
            wparam |= SCF_DEFAULT;
        }
    }

    if(wparam & SCF_DEFAULT)
    {
        m_pfDef=*(PARAFORMAT2 *)lparam;
        m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE,TXTBIT_PARAFORMATCHANGE);
    }
    else
    {
        m_pTxtHost->GetTextService()->TxSendMessage(uMsg,wparam,lparam,NULL);    // Change selection format
    }
    return 1;
}

LRESULT SRichEdit::OnSetText(UINT uMsg,WPARAM wparam,LPARAM lparam)
{
    // For RichEdit 1.0, the max text length would be reset by a settext so
    // we follow pattern here as well.

    HRESULT hr = m_pTxtHost->GetTextService()->TxSendMessage(uMsg, wparam, lparam, 0);

    if (FAILED(hr)) return 0;
    // Update succeeded.
    ULONG cNewText = (ULONG)(lparam?_tcslen((LPCTSTR) lparam):0);

    // If the new text is greater than the max set the max to the new
    // text length.
    if (cNewText > m_cchTextMost)
    {
        m_cchTextMost = cNewText;
    }
    return 1;
}

void SRichEdit::OnSetFont( IFont *pFont, BOOL bRedraw )
{
    if(SUCCEEDED(InitDefaultCharFormat(&m_cfDef, pFont)))
    {
        m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE,
                TXTBIT_CHARFORMATCHANGE);
    }
}

void SRichEdit::SetWindowText( LPCTSTR lpszText )
{
#ifdef _UNICODE
    SSendMessage(WM_SETTEXT,0,(LPARAM)lpszText);
#else
    SStringW str = S_CT2W(lpszText);
    SSendMessage(WM_SETTEXT,0,(LPARAM)(LPCWSTR)str);
#endif
    Invalidate();
}

SStringT SRichEdit::GetWindowText(BOOL bRawText)
{
	(bRawText);
    SStringW strRet;
    int nLen=(int)SSendMessage(WM_GETTEXTLENGTH);
    wchar_t *pBuf=strRet.GetBufferSetLength(nLen+1);
    SSendMessage(WM_GETTEXT,(WPARAM)nLen+1,(LPARAM)pBuf);
    strRet.ReleaseBuffer();
    return S_CW2T(strRet);
}

int SRichEdit::GetWindowTextLength()
{
    return (int)SSendMessage(WM_GETTEXTLENGTH);
}

void SRichEdit::ReplaceSel(LPCWSTR pszText,BOOL bCanUndo)
{
    SSendMessage(EM_REPLACESEL,(WPARAM)bCanUndo,(LPARAM)pszText);
}

void SRichEdit::SetSel(DWORD dwSelection, BOOL bNoScroll)
{
    SSendMessage(EM_SETSEL, LOWORD(dwSelection), HIWORD(dwSelection));
    if(!bNoScroll)
        SSendMessage(EM_SCROLLCARET, 0, 0L);
}

HRESULT SRichEdit::OnAttrTextColor( const SStringW &  strValue,BOOL bLoading )
{
    m_style.SetTextColor(0,GETCOLOR(strValue));
    if(!bLoading)
    {
        SetDefaultTextColor(m_style.GetTextColor(0));
    }
    return S_OK;
}

DWORD CALLBACK EditStreamInCallback_FILE(
                                  DWORD_PTR dwCookie,
                                  LPBYTE pbBuff,
                                  LONG cb,
                                  LONG * pcb 
                                  )
{
    FILE *f=(FILE*)dwCookie;
    LONG nReaded = (LONG)fread(pbBuff,1,cb,f);
    if(pcb) *pcb = nReaded;
    return 0;
}

DWORD CALLBACK EditStreamOutCallback_FILE(
                                       DWORD_PTR dwCookie,
                                       LPBYTE pbBuff,
                                       LONG cb,
                                       LONG * pcb 
                                       )
{
    FILE *f=(FILE*)dwCookie;
    LONG nWrited = (LONG)fwrite(pbBuff,1,cb,f);
    if(pcb) *pcb = nWrited;
    return 0;
}

struct MemBlock{
    LPCBYTE  pBuf;
    LONG     nRemains;
};

DWORD CALLBACK EditStreamInCallback_MemBlock(
                                       DWORD_PTR dwCookie,
                                       LPBYTE pbBuff,
                                       LONG cb,
                                       LONG * pcb 
                                       )
{
    MemBlock *pmb=(MemBlock*)dwCookie;
    if(pmb->nRemains>=cb)
    {
        memcpy(pbBuff,pmb->pBuf,cb);
        pmb->pBuf+=cb;
        pmb->nRemains-=cb;
        if(pcb) *pcb = cb;
        return 0;
    }else
    {
        memcpy(pbBuff,pmb->pBuf,pmb->nRemains);
        pmb->pBuf+=pmb->nRemains;
        if(pcb) *pcb = pmb->nRemains;
        pmb->nRemains =0;
        return 0;
    }
}


HRESULT SRichEdit::OnAttrRTF( const SStringW & strValue,BOOL bLoading )
{
    if(bLoading)
    {
        m_strRtfSrc = strValue;//将数据保存到控件初始化完成再写入控件
        return S_FALSE;
    }else
    {
        SStringTList lstSrc;
        int nSegs = ParseResID(S_CW2T(strValue),lstSrc);

        if(nSegs == 2)
        {//load from resource
            size_t dwSize=GETRESPROVIDER->GetRawBufferSize(lstSrc[0],lstSrc[1]);
            if(dwSize)
            {
                EDITSTREAM es;
                MemBlock mb={NULL,0};
                CMyBuffer<BYTE> mybuf;
                mb.pBuf=mybuf.Allocate(dwSize);
                mb.nRemains=(DWORD)dwSize;
                GETRESPROVIDER->GetRawBuffer(lstSrc[0],lstSrc[1],mybuf,dwSize);
                es.dwCookie=(DWORD_PTR)&mb;
                es.pfnCallback=EditStreamInCallback_MemBlock;
                SSendMessage(EM_STREAMIN,SF_RTF,(LPARAM)&es);
            }
        }else
        {//load from file
            LoadRtf(lstSrc[0]);
        }
        return S_FALSE;
    }
}

COLORREF SRichEdit::SetDefaultTextColor( COLORREF cr )
{
    COLORREF crOld=m_cfDef.crTextColor;
    m_cfDef.crTextColor= cr&0x00ffffff;
    m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_CHARFORMATCHANGE, TXTBIT_CHARFORMATCHANGE);
    return crOld;
}

void SRichEdit::OnEnableDragDrop( BOOL bEnable )
{
    if(bEnable)
    {
        SRicheditDropTarget *pDropTarget=new SRicheditDropTarget(m_pTxtHost->GetTextService());
        GetContainer()->RegisterDragDrop(m_swnd,pDropTarget);
        pDropTarget->Release();
    }else
    {
        GetContainer()->RevokeDragDrop(m_swnd);
    }
}

HRESULT SRichEdit::OnAttrAlign( const SStringW & strValue,BOOL bLoading )
{
    if(!strValue.CompareNoCase(L"center")) m_dwStyle|=ES_CENTER;
    else if(!strValue.CompareNoCase(L"right")) m_dwStyle|=ES_RIGHT;
    else m_dwStyle|=ES_LEFT;
    if(!bLoading)
    {
        if(m_dwStyle&ES_CENTER)
            m_pfDef.wAlignment=PFA_CENTER;
        else if(m_dwStyle&ES_RIGHT)
            m_pfDef.wAlignment=PFA_RIGHT;
        else
            m_pfDef.wAlignment = PFA_LEFT;
        m_pTxtHost->GetTextService()->OnTxPropertyBitsChange(TXTBIT_PARAFORMATCHANGE, 0);
    }
    return bLoading?S_FALSE:S_OK;
}

HRESULT SRichEdit::OnAttrNotifyChange(const SStringW & strValue,BOOL bLoading)
{
    m_fNotifyChange = strValue!=L"0";
    if(!bLoading)
    {
        LPARAM lEvtMask = SSendMessage(EM_GETEVENTMASK);
        if(m_fNotifyChange) lEvtMask |= ENM_CHANGE;
        else lEvtMask &= ~ENM_CHANGE;
        SSendMessage(EM_SETEVENTMASK,0,lEvtMask);
    }
    return S_FALSE;
}


DWORD SRichEdit::SaveRtf( LPCTSTR pszFileName )
{
    FILE *f=_tfopen(pszFileName,_T("wb"));
    if(!f) return 0;
    EDITSTREAM es;
    es.dwCookie=(DWORD_PTR)f;
    es.pfnCallback=EditStreamOutCallback_FILE;
    DWORD dwRet=(DWORD)SSendMessage(EM_STREAMOUT,SF_RTF,(LPARAM)&es);
    fclose(f);
    return dwRet;
}

DWORD SRichEdit::LoadRtf( LPCTSTR pszFileName )
{
    FILE *f=_tfopen(pszFileName,_T("rb"));
    if(!f) return FALSE;
    EDITSTREAM es;
    es.dwCookie=(DWORD_PTR)f;
    es.pfnCallback=EditStreamInCallback_FILE;
    DWORD dwRet=(DWORD)SSendMessage(EM_STREAMIN,SF_RTF,(LPARAM)&es);
    fclose(f);
    return dwRet;
}

void SRichEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
    __super::OnShowWindow(bShow,nStatus);
    if(!IsVisible(TRUE))
    {//防止正在编辑时隐藏了cursor
        HWND hHost = GetContainer()->GetHostHwnd();
        ::PostMessage(hHost,WM_SETCURSOR,(WPARAM)hHost,MAKELPARAM(HTCLIENT,0));
    }
}

void SRichEdit::OnScaleChanged(int nScale)
{
	__super::OnScaleChanged(nScale);
	OnSetFont(NULL,FALSE);//更新默认字体
}

void SRichEdit::OnEnable(BOOL bEnable, UINT nStatus)
{
	__super::OnEnable(bEnable, nStatus);
	COLORREF cr;
	if (bEnable)
	{
		cr = m_style.GetTextColor(0);
		if (CR_INVALID == cr)
			cr = 0;
	}
	else
	{
		cr = m_style.GetTextColor(3);
	}

	if (CR_INVALID != cr)
		SetDefaultTextColor(cr);
}

BOOL SRichEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if(!(m_dwStyle&ES_MULTILINE))
	{
		return SWindow::OnMouseWheel(nFlags, zDelta, pt);
	}else
	{
		return SPanel::OnMouseWheel(nFlags,zDelta,pt);
	}
}

//////////////////////////////////////////////////////////////////////////

SEdit::SEdit() :m_crCue(RGBA(0xcc,0xcc,0xcc,0xff)),m_strCue(this)
{
    m_fRich=0;
    m_fAutoSel=TRUE;
}

void SEdit::OnKillFocus(SWND wndFocus)
{
    SRichEdit::OnKillFocus(wndFocus);
    if(!m_strCue.GetText(FALSE).IsEmpty() && GetWindowTextLength() == 0) Invalidate();
}

void SEdit::OnSetFocus(SWND wndOld)
{
    SRichEdit::OnSetFocus(wndOld);
    if(!m_strCue.GetText(FALSE).IsEmpty() && GetWindowTextLength() == 0) Invalidate();
}

UINT SEdit::GetCueTextAlign()
{
	UINT algin= SWindow::GetTextAlign();
	algin&=~(DT_CENTER|DT_RIGHT);
	if(m_dwStyle&ES_CENTER)
		algin|=DT_CENTER;
	else if(m_dwStyle&ES_RIGHT)
		algin|=DT_RIGHT;	 
	return algin;
}

void SEdit::OnPaint( IRenderTarget * pRT )
{
    SRichEdit::OnPaint(pRT);
    if(!m_strCue.GetText(FALSE).IsEmpty() && GetWindowTextLength() == 0 && !IsFocused())
    {
        SPainter painter;
        BeforePaint(pRT,painter);
        COLORREF crOld = pRT->SetTextColor(m_crCue);
        
        CRect rc;
        GetClientRect(&rc);
        CRect rcInsetPixel = GetStyle().GetPadding();
        rc.DeflateRect(rcInsetPixel.left, rcInsetPixel.top, rcInsetPixel.right, rcInsetPixel.bottom);
		pRT->DrawText(m_strCue.GetText(FALSE),m_strCue.GetText(FALSE).GetLength(),&rc,GetCueTextAlign());
        
        pRT->SetTextColor(crOld);
        AfterPaint(pRT,painter);
    }
}

SStringT SEdit::GetCueText(BOOL bRawText) const
{
    return m_strCue.GetText(bRawText);
}

HRESULT SEdit::OnLanguageChanged()
{
	HRESULT hr = __super::OnLanguageChanged();
	m_strCue.TranslateText();
	return hr;
}

}//namespace SOUI