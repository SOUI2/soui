#include "stdafx.h"
#include "SImagePlayer.h"

namespace SOUI
{

SImagePlayer::SImagePlayer() :m_aniSkin(NULL), m_iCurFrame(0),m_nNextInterval(0)
{

}

SImagePlayer::~SImagePlayer()
{
}


void SImagePlayer::OnPaint( IRenderTarget *pRT )
{
	__super::OnPaint(pRT);
	if(m_aniSkin)
	{		
		m_aniSkin->Draw(pRT, GetWindowRect(),m_iCurFrame);
	}
}

void SImagePlayer::OnShowWindow( BOOL bShow, UINT nStatus )
{
	__super::OnShowWindow(bShow,nStatus);
	if(!bShow)
	{
        GetContainer()->UnregisterTimelineHandler(this);
	}else if(m_aniSkin && m_aniSkin->GetStates()>1)
	{
        GetContainer()->RegisterTimelineHandler(this);
        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 90;
        else
            m_nNextInterval = m_aniSkin->GetFrameDelay()*10;
	}
}

void SImagePlayer::OnNextFrame()
{
    m_nNextInterval -= 10;
    if(m_nNextInterval <= 0 && m_aniSkin)
    {
        int nStates=m_aniSkin->GetStates();
        m_iCurFrame++;
        m_iCurFrame%=nStates;
        Invalidate();

        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 60;
        else
            m_nNextInterval =m_aniSkin->GetFrameDelay()*10;	
    }
}

HRESULT SImagePlayer::OnAttrSkin( const SStringW & strValue, BOOL bLoading )
{
	ISkinObj *pSkin = SSkinPoolMgr::getSingleton().GetSkin(strValue);
	if(!pSkin) return E_FAIL;
	if(!pSkin->IsClass(SSkinAni::GetClassName())) return S_FALSE;
	m_aniSkin=static_cast<SSkinAni*>(pSkin);
    if(!bLoading)
    {
        m_iCurFrame = 0;
        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 90;
        else
            m_nNextInterval =m_aniSkin->GetFrameDelay()*10;	
    }
	return bLoading?S_OK:S_FALSE;
}

CSize SImagePlayer::GetDesiredSize( LPCRECT /*pRcContainer*/ )
{
	CSize sz;
	if(m_aniSkin) sz=m_aniSkin->GetSkinSize();
	return sz;
}

BOOL SImagePlayer::ShowImageFile( LPCTSTR pszFileName )
{
    GetContainer()->UnregisterTimelineHandler(this);
    return _PlayFile(pszFileName,TRUE);
}

int  SImagePlayer::GetFrameCount()
{
    if (m_aniSkin)
    {
        return m_aniSkin->GetStates();
    }
    return 0;
}

SIZE SImagePlayer::GetImageSize()
{
    CSize size = GetDesiredSize(NULL);
    SIZE sizeRet = {size.cx, size.cy};
    return sizeRet;
}

void SImagePlayer::Pause()
{
    if (m_aniSkin && m_aniSkin->GetStates() > 1)
    {
        GetContainer()->UnregisterTimelineHandler(this);
    }
}

void SImagePlayer::Resume()
{
    if (m_aniSkin && m_aniSkin->GetStates() > 1)
    {
        GetContainer()->RegisterTimelineHandler(this);
    }
}

BOOL SImagePlayer::_PlayFile( LPCTSTR pszFileName, BOOL bGif )
{
    SStringW key=S_CT2W(pszFileName);
    SSkinPool *pBuiltinSkinPool = SSkinPoolMgr::getSingletonPtr()->GetBuiltinSkinPool();
    ISkinObj *pSkin=pBuiltinSkinPool->GetSkin(key);
    if(pSkin)
    {
        if(!pSkin->IsClass(SSkinAni::GetClassName())) return FALSE;
        m_aniSkin=static_cast<SSkinAni*>(pSkin);
    }else
    {
        SSkinAni *pGifSkin = (SSkinAni*)SApplication::getSingleton().CreateSkinByName(SSkinMutiFrameImg::GetClassName());
        if(!pGifSkin) return FALSE;
        if(0==pGifSkin->LoadFromFile(pszFileName))
        {
            pGifSkin->Release();
            return FALSE;
        }

        pBuiltinSkinPool->AddKeyObject(key,pGifSkin);//将创建的skin交给skinpool管理
        m_aniSkin = pGifSkin;
    }
    if (GetLayoutParam()->IsWrapContent(Any))
    {
        GetParent()->UpdateChildrenPosition();
    }
    OnShowWindow(TRUE, NormalShow);
    return TRUE;
}

void SImagePlayer::OnDestroy()
{
    GetContainer()->UnregisterTimelineHandler(this);
    __super::OnDestroy();
}
}
