#include "stdafx.h"
#include "SGifPlayer.h"
#include "sskingif.h"
#include "SSkinAPNG.h"

namespace SOUI
{

SGifPlayer::SGifPlayer() :m_aniSkin(NULL), m_iCurFrame(0),m_nNextInterval(0)
{

}

SGifPlayer::~SGifPlayer()
{
}


void SGifPlayer::OnPaint( IRenderTarget *pRT )
{	
	__super::OnPaint(pRT);
	if(m_aniSkin)
	{		
		m_aniSkin->Draw(pRT, GetWindowRect(),m_iCurFrame);
	}
}

void SGifPlayer::OnShowWindow( BOOL bShow, UINT nStatus )
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

void SGifPlayer::OnNextFrame()
{
    m_nNextInterval -= 10;
    if(m_nNextInterval <= 0 && m_aniSkin)
    {
        int nStates=m_aniSkin->GetStates();
        m_iCurFrame++;
        m_iCurFrame%=nStates;
        Invalidate();

        if(m_aniSkin->GetFrameDelay()==0)
            m_nNextInterval = 90;
        else
            m_nNextInterval =m_aniSkin->GetFrameDelay()*10;	
    }
}

HRESULT SGifPlayer::OnAttrSkin( const SStringW & strValue, BOOL bLoading )
{
	ISkinObj *pSkin = SSkinPoolMgr::getSingleton().GetSkin(strValue,GetScale());
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

CSize SGifPlayer::GetDesiredSize( LPCRECT /*pRcContainer*/ )
{
	CSize sz;
	if(m_aniSkin) sz=m_aniSkin->GetSkinSize();
	return sz;
}

BOOL SGifPlayer::PlayGifFile( LPCTSTR pszFileName )
{
    return _PlayFile(pszFileName,TRUE);
}

BOOL SGifPlayer::PlayAPNGFile( LPCTSTR pszFileName )
{
    return _PlayFile(pszFileName,FALSE);
}

BOOL SGifPlayer::_PlayFile( LPCTSTR pszFileName, BOOL bGif )
{
	SStringW key=S_CT2W(pszFileName);

	SSkinAni *pGifSkin = (SSkinAni*)SApplication::getSingleton().CreateSkinByName(bGif?SSkinGif::GetClassName():SSkinAPNG::GetClassName());
	if(!pGifSkin) return FALSE;
	if(0==pGifSkin->LoadFromFile(pszFileName))
	{
		pGifSkin->Release();
		return FALSE;
	}

	m_aniSkin = pGifSkin;

	if(GetLayoutParam()->IsWrapContent(Any))
	{
		GetParent()->UpdateChildrenPosition();
	}
	if(IsVisible(TRUE))
	{
		GetContainer()->RegisterTimelineHandler(this);
	}
	return TRUE;
}

void SGifPlayer::OnDestroy()
{
    GetContainer()->UnregisterTimelineHandler(this);
    __super::OnDestroy();
}
}
