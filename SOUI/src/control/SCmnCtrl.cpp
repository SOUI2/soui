//////////////////////////////////////////////////////////////////////////
//   File Name: duicmnctrl.h
//////////////////////////////////////////////////////////////////////////

#include "souistd.h"
#include "control/SCmnCtrl.h"
#include <windows.h>
#include <shellapi.h>
#include <algorithm>

#if _MSC_VER < 1500
#include <tmschema.h> //for VS2005
#else
#include <vsstyle.h>
#endif

namespace SOUI
{


//////////////////////////////////////////////////////////////////////////
// Static Control
//
void SStatic::DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat)
{
    if(!m_bMultiLines)
    {
		OnDrawLine(pRT, pszBuf, 0, cchText, pRect, uFormat);
    }
    else
    {
        if(uFormat&(DT_VCENTER|DT_BOTTOM) && !(uFormat & DT_CALCRECT))
        {
            //static 多行控件支持垂直居中及底对齐
            CRect rcText=*pRect;
            DrawMultiLine(pRT,pszBuf,cchText,&rcText,uFormat|DT_CALCRECT);
            CSize szTxt=rcText.Size();
            rcText = *pRect;
            switch(GetStyle().GetTextAlign()&(DT_VCENTER|DT_BOTTOM))
            {
            case DT_VCENTER:
                rcText.DeflateRect(0,(rcText.Height()-szTxt.cy)/2);
                break;
            case DT_BOTTOM:
                rcText.DeflateRect(0,(rcText.Height()-szTxt.cy));
                break;
            }
            DrawMultiLine(pRT,pszBuf,cchText,&rcText,uFormat);
        }else
        {
            DrawMultiLine(pRT,pszBuf,cchText,pRect,uFormat);
        }
    }
}

void SStatic::OnDrawLine(IRenderTarget * pRT, LPCTSTR pszBuf, int iBegin, int cchText, LPRECT pRect, UINT uFormat)
{
	pRT->DrawText(pszBuf + iBegin, cchText, pRect, uFormat);
}

void SStatic::DrawMultiLine(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat)
{
    SIZE szChar;
    int i=0, nLine=1;
    if(cchText==-1) cchText=(int)_tcslen(pszBuf);
    LPCTSTR p1=pszBuf;
    POINT pt= {pRect->left,pRect->top};
    pRT->MeasureText(_T("A"),1,&szChar);
    int nLineHei=szChar.cy;
    int nRight=pRect->right;
    pRect->right=pRect->left;
    
    LPCTSTR pLineHead = p1, pLineTail=p1;
    
    while(i<cchText)
    {
        LPTSTR p2=CharNext(p1);
        if(*p1==_T('\\') && p2 && *p2==_T('n'))
        {
            if(pLineTail > pLineHead && !(uFormat & DT_CALCRECT))
            {
                CRect rcText(pRect->left,pt.y,nRight, pt.y + nLineHei);
				OnDrawLine(pRT,pszBuf, (int)(pLineHead-pszBuf),(int)(pLineTail-pLineHead),&rcText,uFormat);
            }
            pt.y+=nLineHei+m_nLineInter;
            pt.x=pRect->left;
            nLine++;
            i+=(int)(p2-p1);
            p1=CharNext(p2);
            i+=(int)(p1-p2);
            
            pLineHead = p2+1;
            continue;
        }
		else if ((*p1 == _T('\n') && p2))
		{
			if (pLineTail > pLineHead && !(uFormat & DT_CALCRECT))
			{
				CRect rcText(pRect->left, pt.y, nRight, pt.y + nLineHei);
				OnDrawLine(pRT, pszBuf, (int)(pLineHead - pszBuf), (int)(pLineTail - pLineHead), &rcText, uFormat);
			}
			pt.y += nLineHei + m_nLineInter;
			pt.x = pRect->left;
			nLine++;
			i += (int)(p2 - p1);
			p1 = p2;
			pLineHead = p2;
			continue;
		}
        pRT->MeasureText(p1,(int)(p2-p1),&szChar);
        if(pt.x+szChar.cx > nRight && pLineTail>pLineHead)
        {//检测到一行超过边界时还要保证当前行不为空
            if(!(uFormat & DT_CALCRECT))
            {
                CRect rcText(pRect->left,pt.y,nRight, pt.y + nLineHei);
				OnDrawLine(pRT, pszBuf, (int)(pLineHead - pszBuf), (int)(pLineTail - pLineHead), &rcText, uFormat);
			}

			// modify by baozi 20190312 显示多行文本时，如果下一行文字的高度超过了文本框，则不再输出下一行文字内容。
			if(pt.y+nLineHei + m_nLineInter>pRect->bottom)
			{//将绘制限制在有效区。
				pLineHead = pLineTail;
				break;
			}

            pLineHead = p1;
            
            pt.y+=nLineHei+m_nLineInter;
            pt.x=pRect->left;
            nLine++;

            continue;
        }
        pt.x+=szChar.cx;
        if(pt.x>pRect->right && uFormat & DT_CALCRECT) pRect->right=pt.x;
        i+=(int)(p2-p1);
        pLineTail = p1 = p2;
    }

    if(uFormat & DT_CALCRECT)
    {
		if(pRect->bottom>pt.y+nLineHei)
			pRect->bottom=pt.y+nLineHei;

    }else if(pLineTail > pLineHead )
    {
        CRect rcText(pRect->left,pt.y,nRight, pt.y + nLineHei);
		OnDrawLine(pRT, pszBuf, (int)(pLineHead - pszBuf), (int)(pLineTail - pLineHead), &rcText, uFormat);
	}
    
}


//////////////////////////////////////////////////////////////////////////
// Link Control
// Only For Header Drag Test
// Usage: <link>inner text example</link>
//
void SLink::DrawText(IRenderTarget *pRT,LPCTSTR pszBuf,int cchText,LPRECT pRect,UINT uFormat)
{
    if(!(uFormat&DT_CALCRECT))
    {
        CRect rc;        
        pRT->DrawText(pszBuf,cchText,&rc,DT_LEFT|DT_CALCRECT);

        if (m_style.GetTextAlign()&DT_CENTER)
        {
            m_rcText.left = pRect->left + (pRect->right-pRect->left - rc.Width())/2;
            m_rcText.right = m_rcText.left + rc.Width();
        }
        else if (m_style.GetTextAlign()&DT_RIGHT)
        {
            m_rcText.left = pRect->right - rc.Width();
            m_rcText.right = pRect->right;
        }
        else
        {
            m_rcText.left = pRect->left;
            m_rcText.right = pRect->left + rc.Width();
        }

        if(m_style.GetTextAlign()&DT_VCENTER)
        {
            m_rcText.top=pRect->top+ (pRect->bottom-pRect->top-rc.Height())/2;
            m_rcText.bottom=m_rcText.top+rc.Height();
        }else if(m_style.GetTextAlign()&DT_BOTTOM)
        {
            m_rcText.bottom=m_rcText.bottom;
            m_rcText.top=m_rcText.bottom-rc.Height();
        }else
        {
            m_rcText.top=m_rcText.top;
            m_rcText.bottom=m_rcText.top+rc.Height();
        }
    }
    __super::DrawText(pRT,pszBuf,cchText,pRect,uFormat);
}


void SLink::OnInitFinished( pugi::xml_node xmlNode)
{
    __super::OnInitFinished(xmlNode);
    if(m_strToolTipText.GetText(TRUE).IsEmpty()) m_strToolTipText.SetText(m_strLinkUrl);
}

BOOL SLink::OnSetCursor(const CPoint &pt)
{
    if(!m_rcText.PtInRect(pt)) return FALSE;
    HCURSOR hCursor=GETRESPROVIDER->LoadCursor(m_style.m_strCursor);
    ::SetCursor(hCursor);
    return TRUE;
}

void SLink::OnLButtonDown( UINT nFlags,CPoint pt )
{
    if(!m_rcText.PtInRect(pt)) return;
    __super::OnLButtonDown(nFlags,pt);
}

void SLink::OnLButtonUp( UINT nFlags,CPoint pt )
{
    if(!m_rcText.PtInRect(pt))
    {
        ReleaseCapture();
        return;
    }
    __super::OnLButtonUp(nFlags,pt);
    
    if (! m_strLinkUrl.IsEmpty())
    {
        ::ShellExecute(NULL, _T("open"), m_strLinkUrl, NULL, NULL, SW_SHOWNORMAL);
    }

}

void SLink::OnMouseMove( UINT nFlags,CPoint pt )
{
    if(!m_rcText.PtInRect(pt))
    {
        if(m_dwState&WndState_Hover) OnMouseLeave();
    }
    else
    {
        if(!(m_dwState&WndState_Hover)) OnMouseHover(nFlags,pt);
    }
}

void SLink::OnMouseHover( WPARAM wParam, CPoint pt )
{
    if(!m_rcText.PtInRect(pt)) return;
    __super::OnMouseHover(wParam,pt);
}
//////////////////////////////////////////////////////////////////////////
// Button Control
// Use id attribute to process click event
//
// Usage: <button name=xx skin=xx>inner text example</button>
//

SButton::SButton() 
:m_accel(0)
,m_bAnimate(FALSE)
,m_byAlphaAni(0xFF)
, m_nAniStep(25)
, m_bDisableAccelIfInvisible(FALSE)
{
    m_pBgSkin=GETBUILTINSKIN(SKIN_SYS_BTN_NORMAL);
    m_bFocusable=TRUE;
}

void SButton::OnPaint(IRenderTarget *pRT)
{
    if (!m_pBgSkin) return;
    CRect rcClient;
    GetClientRect(&rcClient);

    if(m_byAlphaAni==0xFF)
    {//不在动画过程中
        m_pBgSkin->Draw(
            pRT, rcClient,
            IIF_STATE4(GetState(), 0, 1, 2, 3)
            );
    }
    else
    {//在动画过程中
        BYTE byNewAlpha=(BYTE)(((UINT)m_byAlphaAni*m_pBgSkin->GetAlpha())>>8);
        if(GetState()&WndState_Hover)
        {
            //get hover
            m_pBgSkin->Draw(pRT, rcClient, 0, m_pBgSkin->GetAlpha());
            m_pBgSkin->Draw(pRT, rcClient, 1, byNewAlpha);
        }
        else
        {
            //lose hover
            m_pBgSkin->Draw(pRT, rcClient, 0, m_pBgSkin->GetAlpha());
            m_pBgSkin->Draw(pRT, rcClient, 1, m_pBgSkin->GetAlpha()-byNewAlpha);
        }
    }

    __super::OnPaint(pRT);
}

void SButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(nChar==VK_SPACE || nChar==VK_RETURN)
    {
        ModifyState(WndState_PushDown,0,TRUE);
    }else
    {
        SetMsgHandled(FALSE);
    }
}

void SButton::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
    if(nChar==VK_SPACE || nChar==VK_RETURN)
    {
        if(GetState() & WndState_PushDown)
        {
            ModifyState(0,WndState_PushDown,TRUE);
            FireCommand();
        }
    }else
    {
        SetMsgHandled(FALSE);
    }
}

bool SButton::OnAcceleratorPressed( const CAccelerator& accelerator )
{
    if(IsDisabled(TRUE)) 
		return false;
	if(m_bDisableAccelIfInvisible && !IsVisible(TRUE))
		return false;
    FireCommand();
    return true;
}

BOOL SButton::InitFromXml(pugi::xml_node xmlNode)
{
	BOOL bRet=SWindow::InitFromXml(xmlNode);
	SStringT strText = GetWindowText(TRUE);
	
	if (!strText.IsEmpty()&&(strText[strText.GetLength()-1]==_T(')')))
	{
		int pos=strText.ReverseFind(_T('('));
		if ((pos != -1)&&(strText[++pos]==_T('&')))
		{
			SStringT strAccelT=_T("alt+");
			strAccelT += strText[++pos];
			m_accel = CAccelerator::TranslateAccelKey(strAccelT);
			if (m_accel)
			{
				CAccelerator acc(m_accel);
				GetContainer()->GetAcceleratorMgr()->RegisterAccelerator(acc, this);
			}
		}
	}
	return bRet;
}

void SButton::OnDestroy()
{
    if(m_accel)
    {
        CAccelerator acc(m_accel);
        GetContainer()->GetAcceleratorMgr()->UnregisterAccelerator(acc,this);
    }
    StopCurAnimate();
    __super::OnDestroy();
}

HRESULT SButton::OnAttrAccel( SStringW strAccel,BOOL bLoading )
{
    SStringT strAccelT=S_CW2T(strAccel);
	if(m_accel)
	{
		CAccelerator acc(m_accel);
		GetContainer()->GetAcceleratorMgr()->UnregisterAccelerator(acc,this);
	}
    m_accel=CAccelerator::TranslateAccelKey(strAccelT);
    if(m_accel)
    {
		CAccelerator acc(m_accel);
		GetContainer()->GetAcceleratorMgr()->RegisterAccelerator(acc,this);
    }
    return S_FALSE;
}

CSize SButton::GetDesiredSize( LPCRECT pRcContainer )
{
    SASSERT(m_pBgSkin);
    CSize szRet=m_pBgSkin->GetSkinSize();
    if(szRet.cx==0 || szRet.cy==0)
        szRet=__super::GetDesiredSize(pRcContainer);
    return szRet;
}

void SButton::OnStateChanged( DWORD dwOldState,DWORD dwNewState )
{
    __super::OnStateChanged(dwOldState,dwNewState);
    StopCurAnimate();

    if(GetCapture()==m_swnd)    //点击中
        return;

    if(m_bAnimate &&
        ((dwOldState==WndState_Normal && dwNewState==WndState_Hover)
        ||(dwOldState==WndState_Hover && dwNewState==WndState_Normal)))
    {//启动动画
		m_byAlphaAni = 50;        
        GetContainer()->RegisterTimelineHandler(this);
    }
}

void SButton::OnSize( UINT nType, CSize size )
{
    __super::OnSize(nType,size);
    StopCurAnimate();
}

//中止原来的动画
void SButton::StopCurAnimate()
{
    GetContainer()->UnregisterTimelineHandler(this);
    m_byAlphaAni=0xFF;
}

void SButton::OnNextFrame()
{
	m_byAlphaAni += m_nAniStep;
	if (m_byAlphaAni >= 0xFF)
	{
		m_byAlphaAni = 0xFF;
		StopCurAnimate();
	}
    Invalidate();
}

//////////////////////////////////////////////////////////////////////////
// Image Control
// Use src attribute specify a resource id
//
// Usage: <img skin="skin" sub="0"/>
//
SImageWnd::SImageWnd()
    : m_iFrame(0)
    , m_pSkin(NULL)
    , m_fl(kNone_FilterLevel)
    , m_bManaged(FALSE)
	, m_iTile(0)
	, m_bKeepAspect(0)
{
    m_bMsgTransparent=TRUE;
}

SImageWnd::~SImageWnd()
{
    if(m_bManaged && m_pSkin)
    {
        m_pSkin->Release();
    }
    m_pSkin=NULL;
}

void SImageWnd::OnPaint(IRenderTarget *pRT)
{
    CRect rcWnd = GetWindowRect();
	if (rcWnd.IsRectEmpty())
		return;
	if (m_bKeepAspect)
	{
		CSize szImg;
		if (m_pImg) szImg = m_pImg->Size();
		else if(m_pSkin) szImg = m_pSkin->GetSkinSize();
		if (szImg.cx == 0 || szImg.cy == 0)
			return;

		float fWndRatio = rcWnd.Width()*1.0f / rcWnd.Height();
		float fImgRatio = szImg.cx*1.0f / szImg.cy;
		if (fWndRatio > fImgRatio)
		{
			int nWid = (int)(rcWnd.Height()*fImgRatio);
			rcWnd.DeflateRect((rcWnd.Width() - nWid) / 2, 0);
		}
		else
		{
			int nHei = (int)(rcWnd.Width() / fImgRatio);
			rcWnd.DeflateRect(0,(rcWnd.Height() - nHei) / 2);
		}
	}

    if(m_pImg)
    {
        CRect rcImg(CPoint(0,0),m_pImg->Size());
		if (m_iTile == 0)
			pRT->DrawBitmapEx(rcWnd, m_pImg, &rcImg, MAKELONG(EM_STRETCH, m_fl));
		else if (m_iTile == 1)
			pRT->DrawBitmapEx(rcWnd, m_pImg, &rcImg, MAKELONG(EM_NULL, m_fl));
		else if (m_iTile == 2)
			pRT->DrawBitmapEx(rcWnd, m_pImg, &rcImg, MAKELONG(EM_TILE, m_fl));
	}
	else if (m_pSkin)
	{
		m_pSkin->Draw(pRT, rcWnd, m_iFrame);
	}
}

BOOL SImageWnd::SetSkin(ISkinObj *pSkin,int iFrame/*=0*/,BOOL bAutoFree/*=TRUE*/)
{
    if(IsVisible(TRUE)) Invalidate();
    if(m_bManaged && m_pSkin)
    {
        m_pSkin->Release();
        m_bManaged=FALSE;
    }
    if(!pSkin) return FALSE;
    m_pSkin=pSkin;
    m_iFrame=iFrame;
	m_pImg = NULL;
    if(bAutoFree)
    {
        m_pSkin->AddRef();
        m_bManaged=TRUE;
    }else
    {
        m_bManaged=FALSE;
    }

    SASSERT(GetParent());

    if(GetLayoutParam()->IsWrapContent(Any)  && GetParent())
    {
        //重新计算坐标
        RequestRelayout();
    }
    if(IsVisible(TRUE)) Invalidate();
    return TRUE;
}

void SImageWnd::SetImage(IBitmap * pBitmap,FilterLevel fl)
{
    m_pImg = pBitmap;
    m_fl = fl;
	OnContentChanged();
}

IBitmap* SImageWnd::GetImage()
{
	return m_pImg;
}

BOOL SImageWnd::SetIcon( int nSubID )
{
    if(!m_pSkin) return FALSE;
    if(nSubID<0 || nSubID>m_pSkin->GetStates()-1) return FALSE;
    m_iFrame=nSubID;
    Invalidate();
    return TRUE;
}

CSize SImageWnd::GetDesiredSize(LPCRECT pRcContainer)
{
    CSize szRet;
    if(m_pImg) szRet = m_pImg->Size();
    else if(m_pSkin) szRet=m_pSkin->GetSkinSize();
    return szRet;
}

void SImageWnd::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
    if(m_pSkin) m_pSkin->OnColorize(cr);
}

void SImageWnd::OnScaleChanged(int scale)
{
	__super::OnScaleChanged(scale);
	GetScaleSkin(m_pSkin,scale);
}

SAnimateImgWnd::SAnimateImgWnd()
:m_pSkin(NULL)
,m_iCurFrame(0)
,m_nSpeed(50)
,m_bAutoStart(TRUE)
,m_bPlaying(FALSE)
,m_iTimeFrame(0)
,m_nRepeat(-1)
,m_iRepeat(0)
{
    m_bMsgTransparent=TRUE;
	GetEventSet()->addEvent(EVENTID(EventAnimateStart));
	GetEventSet()->addEvent(EVENTID(EventAnimateStop));
}

void SAnimateImgWnd::OnPaint(IRenderTarget *pRT)
{
    if (m_pSkin)
        m_pSkin->Draw(pRT, GetWindowRect(), m_iCurFrame);
}


void SAnimateImgWnd::Start()
{
    if(!m_bPlaying)
    {
        if(IsVisible(TRUE)) GetContainer()->RegisterTimelineHandler(this);
		m_iRepeat = 0;
        m_bPlaying=TRUE;
		EventAnimateStart evt(this);
		FireEvent(evt);
    }
}

void SAnimateImgWnd::Stop()
{
    if(m_bPlaying)
    {
        if(IsVisible(TRUE)) GetContainer()->UnregisterTimelineHandler(this);
        m_bPlaying=FALSE;
		EventAnimateStop evt(this);
		FireEvent(evt);
    }
}

void SAnimateImgWnd::OnDestroy()
{
    Stop();
	__super::OnDestroy();
}

CSize SAnimateImgWnd::GetDesiredSize(LPCRECT pRcContainer)
{
    CSize szRet;
    if(m_pSkin) szRet=m_pSkin->GetSkinSize();
    return szRet;
}

void SAnimateImgWnd::OnShowWindow( BOOL bShow, UINT nStatus )
{
    __super::OnShowWindow(bShow,nStatus);
    if(!bShow)
    {
        if(IsPlaying()) GetContainer()->UnregisterTimelineHandler(this);
    }else
    {
        if(IsPlaying()) GetContainer()->RegisterTimelineHandler(this);
        else if(m_bAutoStart) Start();
    }
}

void SAnimateImgWnd::OnNextFrame()
{
    if(!m_pSkin) GetContainer()->UnregisterTimelineHandler(this);
    else
    {
        if(m_iTimeFrame > (m_nSpeed/10)) m_iTimeFrame=0;
        if(m_iTimeFrame==0)
        {
            int nStates=m_pSkin->GetStates();
            m_iCurFrame++;
			Invalidate();

			if(m_iCurFrame==nStates)
			{
				m_iCurFrame = 0;
				if(m_nRepeat != -1 && ++ m_iRepeat == m_nRepeat)
				{//检查重复次数
					Stop();
				}
			}
        }
        m_iTimeFrame++;
    }
}

void SAnimateImgWnd::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
    if(m_pSkin) m_pSkin->OnColorize(cr);
}

//////////////////////////////////////////////////////////////////////////
// Progress Control
// Use id attribute to process click event
//
// Usage: <progress bgskin=xx posskin=xx min=0 max=100 value=10,showpercent=0/>
//

SProgress::SProgress()
    : m_nMinValue(0)
    , m_nMaxValue(100)
    , m_nValue(0)
    , m_bShowPercent(FALSE)
    , m_pSkinBg(NULL)
    , m_pSkinPos(NULL)
    , m_bVertical(FALSE)
{
	m_bFocusable=TRUE;
}


CSize SProgress::GetDesiredSize(LPCRECT pRcContainer)
{
    CSize szRet;
    SIZE sizeBg = m_pSkinBg->GetSkinSize();
	CRect rcMargin = GetStyle().GetMargin();
    if(IsVertical())
    {
        szRet.cx = sizeBg.cx + rcMargin.left + rcMargin.right;
        if(GetLayoutParam()->IsSpecifiedSize(Vert))
            szRet.cy=GetLayoutParam()->GetSpecifiedSize(Vert).toPixelSize(GetScale());
        else
            szRet.cy = sizeBg.cy + rcMargin.top + rcMargin.bottom;
    }else
    {
        szRet.cy = sizeBg.cy + rcMargin.top + rcMargin.bottom;
        if(GetLayoutParam()->IsSpecifiedSize(Horz))
            szRet.cx=GetLayoutParam()->GetSpecifiedSize(Horz).toPixelSize(GetScale());
        else
            szRet.cx = sizeBg.cx + rcMargin.left + rcMargin.right;
    }
    return szRet;
}

void SProgress::OnPaint(IRenderTarget *pRT)
{
    SPainter painter;

    BeforePaint(pRT, painter);

    SASSERT(m_pSkinBg && m_pSkinPos);
    
    CRect rcClient;
    GetClientRect(&rcClient);
    m_pSkinBg->Draw(pRT, rcClient, WndState_Normal);
    CRect rcValue=rcClient;

    if(IsVertical())
    {
        rcValue.bottom=rcValue.top+(int)(((__int64)rcValue.Height())*(m_nValue-m_nMinValue)/(__int64)(m_nMaxValue-m_nMinValue));
    }
    else
    {
        rcValue.right=rcValue.left+(int)(((__int64)rcValue.Width())*(m_nValue-m_nMinValue)/(__int64)(m_nMaxValue-m_nMinValue));
    }
    if(m_nValue>m_nMinValue)
    {
        m_pSkinPos->Draw(pRT, rcValue, WndState_Normal);
    }


    if (m_bShowPercent && !IsVertical())
    {
        SStringT strPercent;
        strPercent.Format(_T("%d%%"), (int)((m_nValue-m_nMinValue) * 100/(m_nMaxValue-m_nMinValue)));
        pRT->DrawText(strPercent, strPercent.GetLength(), GetWindowRect(), DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }
    AfterPaint(pRT, painter);
}

int SProgress::OnCreate( void* )
{
    if(!m_pSkinBg)  m_pSkinBg=GETBUILTINSKIN(IsVertical()?SKIN_SYS_VERT_PROG_BKGND:SKIN_SYS_PROG_BKGND);
    if(!m_pSkinPos) m_pSkinPos=GETBUILTINSKIN(IsVertical()?SKIN_SYS_VERT_PROG_BAR:SKIN_SYS_PROG_BAR);
    SASSERT(m_pSkinBg && m_pSkinPos);
    return (m_pSkinBg && m_pSkinPos)?0:-1;
}

BOOL SProgress::SetValue(int dwValue)
{
    if(dwValue<m_nMinValue) dwValue =m_nMinValue;
    if(dwValue>m_nMaxValue) dwValue =m_nMaxValue;
    m_nValue=dwValue;
    
    Invalidate();
    return TRUE;
}

void SProgress::SetRange( int nMin,int nMax )
{
    SASSERT(nMax>nMin);
    m_nMaxValue=nMax;
    m_nMinValue=nMin;
    if(m_nValue>m_nMaxValue) m_nValue=m_nMaxValue;
    if(m_nValue<nMin) m_nValue=m_nMinValue;
    Invalidate();
}

void SProgress::GetRange( int *pMin,int *pMax )
{
    if(pMin) *pMin=m_nMinValue;
    if(pMax) *pMax=m_nMaxValue;
}

void SProgress::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
    if(m_pSkinBg) m_pSkinBg->OnColorize(cr);
    if(m_pSkinPos) m_pSkinPos->OnColorize(cr);
}

//////////////////////////////////////////////////////////////////////////
// Line Control
// Simple HTML "HR" tag

SLine::SLine()
    : m_nLineStyle(PS_SOLID)
    , m_nLineSize(1)
    , m_mode(HR_HORZ)
    , m_crLine(RGBA(0,0,0,255))
{
}

void SLine::OnPaint(IRenderTarget *pRT)
{
    CPoint pts[2];
    CRect rcWnd = GetWindowRect();
    pts[0]=rcWnd.TopLeft();
    switch(m_mode)
    {
    case HR_HORZ:pts[1].x=rcWnd.right,pts[1].y=rcWnd.top;break;
    case HR_VERT:pts[1].x=rcWnd.left,pts[1].y=rcWnd.bottom;break;
    case HR_TILT:pts[1]=rcWnd.BottomRight();break;
    }
    CAutoRefPtr<IPen> curPen,oldPen;
    pRT->CreatePen(m_nLineStyle,m_crLine,m_nLineSize,&curPen);
    pRT->SelectObject(curPen,(IRenderObj**)&oldPen);
    pRT->DrawLines(pts,2);
    pRT->SelectObject(oldPen);
}

//////////////////////////////////////////////////////////////////////////
// Check Box

SCheckBox::SCheckBox()
    : m_pSkin(GETBUILTINSKIN(SKIN_SYS_CHECKBOX))
    , m_pFocusSkin(GETBUILTINSKIN(SKIN_SYS_FOCUSCHECKBOX))
{
    m_style.SetAttribute(L"align",L"left");
    m_bFocusable=TRUE;
}


CRect SCheckBox::GetCheckRect()
{
    CRect rcClient;
    GetClientRect(rcClient);
    SASSERT(m_pSkin);
    CSize szCheck=m_pSkin->GetSkinSize();
    CRect rcCheckBox(rcClient.TopLeft(),szCheck);
    rcCheckBox.OffsetRect(0,(rcClient.Height()-szCheck.cy)/2);
    return rcCheckBox;
}

void SCheckBox::GetTextRect( LPRECT pRect )
{
    GetClientRect(pRect);
    SASSERT(m_pSkin);
    CSize szCheck=m_pSkin->GetSkinSize();
    pRect->left+=szCheck.cx+CheckBoxSpacing;    
}

void SCheckBox::OnPaint(IRenderTarget *pRT)
{
    CRect rcCheckBox=GetCheckRect();
    m_pSkin->Draw(pRT, rcCheckBox, _GetDrawState());
    __super::OnPaint(pRT);
}

void SCheckBox::DrawFocus(IRenderTarget *pRT)
{
    if(m_pFocusSkin)
    {
        CRect rcCheckBox=GetCheckRect();
        m_pFocusSkin->Draw(pRT,rcCheckBox,0);
    }else
    {
        __super::DrawFocus(pRT);
    }
}

CSize SCheckBox::GetDesiredSize(LPCRECT pRcContainer)
{
    SASSERT(m_pSkin);
    CSize szCheck=m_pSkin->GetSkinSize();
    CSize szRet=__super::GetDesiredSize(pRcContainer);
    szRet.cx+=szCheck.cx + CheckBoxSpacing;
    szRet.cy=(std::max)(szRet.cy, szCheck.cy);
    return szRet;
}


UINT SCheckBox::_GetDrawState()
{
    DWORD dwState = GetState();
    UINT uState = 0;

    if (m_pSkin)
    {
        if (dwState & WndState_Check)
        {
            if (dwState & WndState_Disable)
                uState = CBS_CHECKEDDISABLED;
            else if (dwState & WndState_PushDown)
                uState = CBS_CHECKEDPRESSED;
            else if (dwState & WndState_Hover)
                uState = CBS_CHECKEDHOT;
            else
                uState = CBS_CHECKEDNORMAL;
        }
        else
        {
            if (dwState & WndState_Disable)
                uState = CBS_UNCHECKEDDISABLED;
            else if (dwState & WndState_PushDown)
                uState = CBS_UNCHECKEDPRESSED;
            else if (dwState & WndState_Hover)
                uState = CBS_UNCHECKEDHOT;
            else
                uState = CBS_UNCHECKEDNORMAL;
        }
    }

    --uState;    // 减1

    return uState;
}

void SCheckBox::OnLButtonUp(UINT nFlags, CPoint point)
{
    if((GetState()&WndState_PushDown) && GetWindowRect().PtInRect(point))
    {
        if (IsChecked())
            ModifyState(0, WndState_Check,TRUE);
        else
            ModifyState(WndState_Check, 0,TRUE);
    }

    SWindow::OnLButtonUp(nFlags,point);
}

void SCheckBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if(nChar==VK_SPACE)
    {
        if (IsChecked())
            ModifyState(0, WndState_Check,TRUE);
        else
            ModifyState(WndState_Check, 0,TRUE);

        FireCommand();
    }
}

HRESULT SCheckBox::OnAttrCheck( const SStringW& strValue, BOOL bLoading )
{
    SetCheck(STRINGASBOOL(strValue));
    return S_FALSE;
}

void SCheckBox::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
    if(m_pFocusSkin) m_pFocusSkin->OnColorize(cr);
    if(m_pSkin) m_pSkin->OnColorize(cr);
}

void SCheckBox::OnScaleChanged(int nScale)
{
	__super::OnScaleChanged(nScale);
	GetScaleSkin(m_pSkin, nScale);
	GetScaleSkin(m_pFocusSkin, nScale);
}

//////////////////////////////////////////////////////////////////////////
// Icon Control

SIconWnd::SIconWnd():m_theIcon(0)
{

}

SIconWnd::~SIconWnd()
{
    if(m_theIcon) DestroyIcon(m_theIcon);
}

void SIconWnd::OnPaint(IRenderTarget *pRT)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    if(m_theIcon) pRT->DrawIconEx(rcClient.left,rcClient.top,m_theIcon,rcClient.Width(),rcClient.Height(),DI_NORMAL);
}


CSize SIconWnd::GetDesiredSize(LPCRECT pRcContainer)
{
    if(!m_theIcon) return CSize();
    ICONINFO iconInfo={0};
    GetIconInfo(m_theIcon,&iconInfo);
    if(iconInfo.hbmColor) DeleteObject(iconInfo.hbmColor);
    if(iconInfo.hbmMask) DeleteObject(iconInfo.hbmMask);

    return CSize(iconInfo.xHotspot*2,iconInfo.yHotspot*2);
}

void SIconWnd::SetIcon(HICON hIcon)
{
	if(m_theIcon == hIcon) return;
    if(m_theIcon) DestroyIcon(m_theIcon);
    m_theIcon = hIcon;
    Invalidate();
}

//////////////////////////////////////////////////////////////////////////
// Radio Box
//
// Usage: <radio state=1>This is a check-box</radio>
//

SRadioBox::SRadioBox()
    : m_pSkin(GETBUILTINSKIN(SKIN_SYS_RADIO))
    , m_pFocusSkin(GETBUILTINSKIN(SKIN_SYS_FOCUSRADIO))
	, m_uIconAlign(SwndStyle::Align_Left)
	, m_uIconVAlign(SwndStyle::VAlign_Middle)
{
    m_style.SetAttribute(L"align",L"left");
    m_bFocusable=TRUE;
}


CRect SRadioBox::GetRadioRect()
{
    CRect rcClient;
    GetClientRect(rcClient);
    SASSERT(m_pSkin);
    CSize szRadioBox = m_pSkin->GetSkinSize();
	CRect rcRadioBox = rcClient;

	switch (m_uIconAlign)
	{
	case SwndStyle::Align_Center:
		rcRadioBox.left += (rcClient.Width() - szRadioBox.cx) / 2;
		break;
	case SwndStyle::Align_Right: 
		rcRadioBox.left = rcClient.right - szRadioBox.cx;
		break;
	}
	switch (m_uIconVAlign)
	{
	case SwndStyle::VAlign_Middle:
		rcRadioBox.top += (rcClient.Height() - szRadioBox.cy) / 2;
		break;
	case SwndStyle::VAlign_Bottom:
		rcRadioBox.top = rcClient.bottom - szRadioBox.cy;
		break;
	}
	rcRadioBox.right = rcRadioBox.left + szRadioBox.cx;
	rcRadioBox.bottom = rcRadioBox.top + szRadioBox.cy;
    
    return rcRadioBox;
}


void SRadioBox::GetTextRect( LPRECT pRect )
{
    GetClientRect(pRect);
    SASSERT(m_pSkin);
    CSize szRadioBox = m_pSkin->GetSkinSize();

	switch (m_uIconAlign)
	{
	case SwndStyle::Align_Left:
		pRect->left += (szRadioBox.cx + RadioBoxSpacing);
		break;
	case SwndStyle::Align_Right:
		pRect->right -= (szRadioBox.cx + RadioBoxSpacing);
		break;
	}
	switch (m_uIconVAlign)
	{
	case SwndStyle::VAlign_Top:
		pRect->top += (szRadioBox.cy + RadioBoxSpacing);
		break;
	case SwndStyle::VAlign_Bottom:
		pRect->bottom -= (szRadioBox.cy + RadioBoxSpacing);
		break;
	}
}

void SRadioBox::OnPaint(IRenderTarget *pRT)
{
    SASSERT(m_pSkin);
    CRect rcRadioBox=GetRadioRect();
    m_pSkin->Draw(pRT, rcRadioBox, _GetDrawState());
    __super::OnPaint(pRT);
}

void SRadioBox::DrawFocus(IRenderTarget *pRT)
{
    if(m_pFocusSkin && m_bDrawFocusRect && IsFocusable())
	{
        CRect rcCheckBox=GetRadioRect();
        m_pFocusSkin->Draw(pRT,rcCheckBox,0);
    }else
    {
        __super::DrawFocus(pRT);
    }
}


CSize SRadioBox::GetDesiredSize(LPCRECT pRcContainer)
{
    CSize szRet=__super::GetDesiredSize(pRcContainer);
    CSize szRaio=m_pSkin->GetSkinSize();
    szRet.cx+=szRaio.cx + RadioBoxSpacing;
    szRet.cy=(std::max)(szRet.cy,szRaio.cy);
    return szRet;
}


UINT SRadioBox::_GetDrawState()
{
    DWORD dwState = GetState();
    UINT uState = 0;

    if (dwState & WndState_Check)
    {
        if (dwState & WndState_Disable)
            uState = RBS_CHECKEDDISABLED;
        else if (dwState & WndState_PushDown)
            uState = RBS_CHECKEDPRESSED;
        else if (dwState & WndState_Hover)
            uState = RBS_CHECKEDHOT;
        else
            uState = RBS_CHECKEDNORMAL;
    }
    else
    {
        if (dwState & WndState_Disable)
            uState = RBS_UNCHECKEDDISABLED;
        else if (dwState & WndState_PushDown)
            uState = RBS_UNCHECKEDPRESSED;
        else if (dwState & WndState_Hover)
            uState = RBS_UNCHECKEDHOT;
        else
            uState = RBS_UNCHECKEDNORMAL;
    }

    --uState;

    return uState;
}

BOOL SRadioBox::NeedRedrawWhenStateChange()
{
    return TRUE;
}

void SRadioBox::OnSetFocus(SWND wndOld,CFocusManager::FocusChangeReason reason)
{
	if(reason != CFocusManager::kReasonFocusRestore)
	{
		if(!IsChecked()) SetCheck(TRUE);
	}
    Invalidate();
}

void SRadioBox::OnLButtonUp(UINT nFlags,CPoint pt)
{
    __super::OnLButtonUp(nFlags, pt);
    if(!IsChecked()) SetCheck(TRUE);
}

SWindow * SRadioBox::GetSelectedSiblingInGroup()
{
    SWindow *pParent=GetParent();
    SASSERT(pParent);
    SWindow *pSibling=pParent->GetWindow(GSW_FIRSTCHILD);
    while(pSibling)
    {
        if(pSibling->IsClass(GetClassName()))
        {
            SRadioBox * pRadio=(SRadioBox*)pSibling;
            if(pRadio->IsChecked()) return pRadio;
        }
        pSibling=pSibling->GetWindow(GSW_NEXTSIBLING);
    }
    return NULL;
}


HRESULT SRadioBox::OnAttrCheck( const SStringW& strValue, BOOL bLoading )
{
    if(bLoading)
    {
        GetEventSet()->setMutedState(true);
        SetCheck(strValue != L"0");
        GetEventSet()->setMutedState(false);
    }else
    {
        SetCheck(strValue != L"0");
    }
    return S_FALSE;
}

void SRadioBox::OnStateChanging( DWORD dwOldState,DWORD dwNewState )
{
    if((dwNewState & WndState_Check) && !(dwOldState & WndState_Check))
    {
        SRadioBox *pCurChecked=(SRadioBox*)GetSelectedSiblingInGroup();
        if(pCurChecked)
        {
            pCurChecked->GetEventSet()->setMutedState(true);
            pCurChecked->SetCheck(FALSE);
            pCurChecked->GetEventSet()->setMutedState(false);
        }
    }
}

void SRadioBox::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
    if(m_pFocusSkin) m_pFocusSkin->OnColorize(cr);
    if(m_pSkin) m_pSkin->OnColorize(cr);
}

void SRadioBox::OnScaleChanged(int nScale)
{
	__super::OnScaleChanged(nScale);
	GetScaleSkin(m_pSkin, nScale);
	GetScaleSkin(m_pFocusSkin, nScale);
}

//////////////////////////////////////////////////////////////////////////
// CDuiToggle
SToggle::SToggle():m_bToggled(FALSE)
{
    m_pSkin = GETBUILTINSKIN(SKIN_SYS_TREE_TOGGLE);
}

void SToggle::SetToggle(BOOL bToggle,BOOL bUpdate/*=TRUE*/)
{
    m_bToggled=bToggle;
    if(bUpdate) Invalidate();
}

BOOL SToggle::GetToggle()
{
    return m_bToggled;
}

void SToggle::OnPaint(IRenderTarget *pRT)
{
    SASSERT(m_pSkin);
    DWORD nState=0;
    if(GetState()&WndState_PushDown) nState=2;
    else if(GetState()&WndState_Hover) nState=1;
    if(m_bToggled) nState+=3;
    m_pSkin->Draw(pRT,GetWindowRect(),nState);
}

void SToggle::OnLButtonUp(UINT nFlags,CPoint pt)
{
    if(GetWindowRect().PtInRect(pt)) m_bToggled=!m_bToggled;
    __super::OnLButtonUp(nFlags,pt);
}

CSize SToggle::GetDesiredSize(LPCRECT pRcContainer)
{
    CSize sz;
    if(m_pSkin) sz=m_pSkin->GetSkinSize();
    return sz;
}

void SToggle::OnColorize(COLORREF cr)
{
    __super::OnColorize(cr);
    if(m_pSkin) m_pSkin->OnColorize(cr);
}

void SToggle::OnScaleChanged(int nScale)
{
	__super::OnScaleChanged(nScale);
	GetScaleSkin(m_pSkin, nScale);
}

#define GROUP_HEADER        20
#define GROUP_ROUNDCORNOR    4

SGroup::SGroup()
:m_crLine1(RGBA(0xF0,0xF0,0xF0,0xFF))
,m_crLine2(RGBA(0xA0,0xA0,0xA0,0xFF))
{
	m_nRound.setSize(GROUP_ROUNDCORNOR, SLayoutSize::dp);
	m_nHeaderHeight.setSize(GROUP_HEADER, SLayoutSize::dp);
}

void SGroup::OnPaint(IRenderTarget *pRT)
{

    SPainter painter;

    BeforePaint(pRT, painter);

    CSize szFnt;
    pRT->MeasureText(m_strText.GetText(FALSE), m_strText.GetText(FALSE).GetLength(),&szFnt);

    CRect rcText=GetWindowRect();
    rcText.left+=GROUP_HEADER,rcText.right-=GROUP_HEADER;
    rcText.bottom=rcText.top+szFnt.cy+2;
    if(GetTextAlign() & DT_CENTER)
    {
        rcText.left+=(rcText.Width()-szFnt.cx)/2;
        rcText.right=rcText.left+szFnt.cx;
    }
    else if(GetTextAlign() & DT_RIGHT)
    {
        rcText.left=rcText.right-szFnt.cx;
    }
    else
    {
        rcText.right=rcText.left+szFnt.cx;
    }

    
    if(!m_strText.GetText(FALSE).IsEmpty())
    {
        CRect rcClip=rcText;
        rcClip.InflateRect(5,5,5,5);
        pRT->PushClipRect(&rcClip,RGN_DIFF);
    }

    {
        CRect rcGroupBox = GetWindowRect();

        if(!m_strText.GetText(FALSE).IsEmpty()) rcGroupBox.top+=szFnt.cy/2;
        rcGroupBox.DeflateRect(1,1,1,0);
        
        CAutoRefPtr<IPen> pen1,pen2,oldPen;
        pRT->CreatePen(PS_SOLID,m_crLine1,1,&pen1);
        pRT->CreatePen(PS_SOLID,m_crLine2,1,&pen2);
        pRT->SelectObject(pen1,(IRenderObj**)&oldPen);

		int nRound = m_nRound.toPixelSize(GetScale());
        pRT->DrawRoundRect(&rcGroupBox,CPoint(nRound, nRound));
              
        pRT->SelectObject(pen2);
        rcGroupBox.InflateRect(1,1,1,-1);
        pRT->DrawRoundRect(&rcGroupBox,CPoint(nRound, nRound));

        pRT->SelectObject(oldPen);
    }

    if(!m_strText.GetText(FALSE).IsEmpty())
    {
        pRT->PopClip();
        pRT->DrawText(m_strText.GetText(FALSE), m_strText.GetText(FALSE).GetLength(), rcText, DT_SINGLELINE|DT_VCENTER);
    }

    AfterPaint(pRT, painter);
}

CRect SGroup::GetChildrenLayoutRect()
{
    CRect rcLayout = SWindow::GetChildrenLayoutRect();
    rcLayout.top += m_nHeaderHeight.toPixelSize(GetScale());
    return rcLayout;
}

CSize SGroup::GetDesiredSize(int nParentWid, int nParentHei)
{
	CSize szRet = __super::GetDesiredSize(nParentWid,nParentHei);
	szRet.cy += m_nHeaderHeight.toPixelSize(GetScale());
	return szRet;
}

}//namespace SOUI