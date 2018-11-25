#include "stdafx.h"
#include "sdemoskin.h"
#include "helper/SDIBHelper.h"

namespace SOUI
{	
	SDemoSkin::SDemoSkin():m_csSize(0, 0),m_bkColor(RGB(255, 255, 255)), m_bIsColor(false), m_ISetOrLoadSkinHandler(NULL)
	{
	}

	SDemoSkin::SDemoSkin(ISetOrLoadSkinHandler *handle) : m_csSize(0, 0), m_bkColor(RGB(255, 255, 255)), m_bIsColor(false), m_ISetOrLoadSkinHandler(handle)
	{
	}

	bool SDemoSkin::SetImage(IBitmap *pImg)
	{
		if (m_pImg)
			m_pImg->Release();
		m_pImg = pImg;
		if (m_pImg)
		 m_pImg->AddRef();
		return true;
	}

	bool SDemoSkin::SetImage(SStringW imgfile)
	{
		m_bIsColor = false;
		m_FilePath = imgfile;
		IBitmap *image = LOADIMAGE2(L"file:" + imgfile);
		if (image)
		{
			SetImage(image);
			image->Release();
			return true;
		}
		return false;
	}

	bool SDemoSkin::SetColor(COLORREF bkColor)
	{
		m_FilePath.Empty();
		m_bIsColor = true;		
		m_bkColor = bkColor;		
		return true;
	}

	void SDemoSkin::ClearSkin()
	{
		m_FilePath.Empty();
		m_bIsColor = false;
		m_pImg = NULL;
	}

	SIZE SDemoSkin::GetSkinSize()
	{		
		SIZE ret = { 0, 0 };
		if (m_pImg)
			ret = m_pImg->Size();
		return ret;
	}

	BOOL SDemoSkin::IgnoreState()
	{
		return TRUE;
	}

	bool SDemoSkin::SaveSkin()
	{
		if (m_ISetOrLoadSkinHandler == NULL)
			return false;
		SkinSaveInf saveInf;
		if (m_bIsColor)
		{
			saveInf.color = m_bkColor;
			return m_ISetOrLoadSkinHandler->SaveSkin(SkinType::color, saveInf);
		}
		else if(!m_FilePath.IsEmpty())
		{
			saveInf.margin = m_rcMargin;
			saveInf.filepath = m_FilePath;
			return m_ISetOrLoadSkinHandler->SaveSkin(SkinType::sys, saveInf);
		}
		else
		{
			return m_ISetOrLoadSkinHandler->SaveSkin(SkinType::builtin, saveInf);
		}
		return false;
	}

	bool SDemoSkin::LoadSkin(SkinType skinType, SkinLoadInf &skinLoadInf)
	{
		switch (skinType)
		{
		case color:
			return	SetColor(skinLoadInf.color);
		case sys:
			m_rcMargin = skinLoadInf.margin;
			return SetImage(skinLoadInf.filepath);
		default:
			break;
		}
		return false;
	}
	
	int SDemoSkin::GetStates()
	{
		return 1;
	}

	void SDemoSkin::SetHander(ISetOrLoadSkinHandler* skinhander)
	{
		m_ISetOrLoadSkinHandler = skinhander;
	}

	void SDemoSkin::_Draw(IRenderTarget * pRT, LPCRECT rcDraw, DWORD dwState, BYTE byAlpha)
	{
		if (m_bIsColor)
		{			
			COLORREF bkColor = m_bkColor | (byAlpha << 24);
			pRT->FillSolidRect(rcDraw, bkColor);
		}
		else if (m_pImg)
		{
			SIZE sz = GetSkinSize();
			CPoint pt(0, 0);
			CRect rcSour(pt, sz);
			pRT->DrawBitmap9Patch(rcDraw, m_pImg, &rcSour, &m_rcMargin, GetExpandMode(), byAlpha);
		}		
	}	

	COLORREF SDemoSkin::GetThemeColor() const
	{
		if (m_bIsColor)
			return m_bkColor;
		else if (m_pImg)
			return SDIBHelper::CalcAvarageColor(m_pImg);
		else
			return CR_INVALID;
	}

}