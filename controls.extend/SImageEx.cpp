#include "stdafx.h"
#include "SImageEx.h"
#include "SSkinMutiFrameImg.h"

SImageEx::SImageEx()
{
}

SImageEx::~SImageEx()
{
}

HRESULT SImageEx::OnSrc(const SStringW& strValue, BOOL bLoading)
{
	SSkinAni *pSkin = (SSkinAni*)SApplication::getSingleton().CreateSkinByName(SSkinMutiFrameImg::GetClassName());
	if(!pSkin) return FALSE;
	if(0==pSkin->LoadFromFile(strValue))
	{
		pSkin->Release();
		return FALSE;
	}

	pSkin->SetAttribute(L"filterLevel", L"high");
	m_pBgSkin = pSkin;
	return TRUE;
}
