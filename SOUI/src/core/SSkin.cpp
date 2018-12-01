//////////////////////////////////////////////////////////////////////////
//   File Name: SSkinPool
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "souistd.h"
#include "core/Sskin.h"
#include "helper/SDIBHelper.h"

namespace SOUI
{

//////////////////////////////////////////////////////////////////////////
// SSkinImgList
SSkinImgList::SSkinImgList()
	: m_nStates(1)
	, m_bTile(FALSE)
	, m_bVertical(FALSE)
	, m_filterLevel(kNone_FilterLevel)
	, m_bAutoFit(TRUE)
{
	
}

SSkinImgList::~SSkinImgList()
{
}

SIZE SSkinImgList::GetSkinSize()
{
	SIZE ret = {0, 0};
	if(m_pImg)
		ret = m_pImg->Size();
	if(m_bVertical)
		ret.cy /= m_nStates;
    else 
		ret.cx /= m_nStates;
	return ret;
}

BOOL SSkinImgList::IgnoreState()
{
    return GetStates()==1;
}

int SSkinImgList::GetStates()
{
	int nStates = (int)m_arrStateMap.GetCount();
	if (nStates > 0) return nStates;
	
	return m_nStates;
}

HRESULT SSkinImgList::OnAttrStateMap(const SStringW & strValue, BOOL bLoading)
{
	m_arrStateMap.RemoveAll();
	
	SStringWList strLst;
	SplitString(strValue, '-', strLst);
	
	for(size_t i=0; i<strLst.GetCount(); ++i)
	{
		m_arrStateMap.Add(_wtoi(strLst[i]));
	}

	return S_OK;
}

void SSkinImgList::_Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)
{
    if(!m_pImg) return;
		
	size_t nCount = m_arrStateMap.GetCount();
	if(dwState < nCount)
		dwState = m_arrStateMap[dwState];

    SIZE sz = GetSkinSize();
    RECT rcSrc = {0, 0, sz.cx, sz.cy};
    if(m_bVertical) 
        OffsetRect(&rcSrc, 0, dwState*sz.cy);
    else
        OffsetRect(&rcSrc, dwState*sz.cx, 0);
    pRT->DrawBitmapEx(rcDraw ,m_pImg, &rcSrc, GetExpandMode(), byAlpha);
}

UINT SSkinImgList::GetExpandMode()
{
    if(m_bAutoFit)
        return MAKELONG(m_bTile?EM_TILE:EM_STRETCH,m_filterLevel);
    else
        return MAKELONG(EM_NULL,m_filterLevel);
}

void SSkinImgList::OnColorize(COLORREF cr)
{
    if(!m_bEnableColorize) return;
    if(cr == m_crColorize) return;
	m_crColorize = cr;

    if(m_imgBackup)
    {//restore
        LPCVOID pSrc = m_imgBackup->GetPixelBits();
        LPVOID pDst = m_pImg->LockPixelBits();
        memcpy(pDst,pSrc,m_pImg->Width()*m_pImg->Height()*4);
        m_pImg->UnlockPixelBits(pDst);
    }else
    {
		if (!m_pImg) 
			return;
		if(S_OK != m_pImg->Clone(&m_imgBackup)) return;
    }
    
	if(cr!=0)
		SDIBHelper::Colorize(m_pImg,cr);
	else
		m_imgBackup = NULL;//free backup
}

void SSkinImgList::_Scale(ISkinObj * skinObj, int nScale)
{
	__super::_Scale(skinObj,nScale);
	SSkinImgList *pRet = sobj_cast<SSkinImgList>(skinObj);
	pRet->m_nStates = m_nStates;
	pRet->m_bTile = m_bTile;
	pRet->m_bVertical = m_bVertical;
	pRet->m_filterLevel = m_filterLevel;
	pRet->m_bAutoFit = m_bAutoFit;
	pRet->m_arrStateMap = m_arrStateMap;
	
	CSize szSkin = GetSkinSize();
	szSkin.cx = MulDiv(szSkin.cx, nScale, 100);
	szSkin.cy = MulDiv(szSkin.cy, nScale, 100);
	if(m_bVertical)
	{
		szSkin.cy *= m_nStates;
	}else
	{
		szSkin.cx *= m_nStates;
	}

	if(m_imgBackup)
	{
		m_imgBackup->Scale(&pRet->m_imgBackup, szSkin.cx, szSkin.cy, kHigh_FilterLevel);
	}
	if(m_pImg)
	{
		m_pImg->Scale(&pRet->m_pImg, szSkin.cx, szSkin.cy, kHigh_FilterLevel);
	}
}

//////////////////////////////////////////////////////////////////////////
//  SSkinImgCenter
void SSkinImgCenter::_Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState, BYTE byAlpha)
{
	SIZE szSkin = GetSkinSize();
	CRect rcTarget = *rcDraw;
	CPoint pt;
	pt.x = rcTarget.left + (rcTarget.Width() - szSkin.cx) / 2;
	pt.y = rcTarget.top + (rcTarget.Height() - szSkin.cy) / 2;

	rcTarget = CRect(pt, szSkin);

	RECT rcSrc = { 0, 0, szSkin.cx, szSkin.cy };
	if (m_bVertical)
		OffsetRect(&rcSrc, 0, dwState*szSkin.cy);
	else
		OffsetRect(&rcSrc, dwState*szSkin.cx, 0);

	pRT->DrawBitmapEx(rcTarget, m_pImg, &rcSrc, GetExpandMode(), byAlpha);
}

//////////////////////////////////////////////////////////////////////////
//  SSkinImgFrame
SSkinImgFrame::SSkinImgFrame()
    : m_rcMargin(0,0,0,0)
{
}

void SSkinImgFrame::_Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)
{
    if(!m_pImg) return;
    SIZE sz=GetSkinSize();
    CPoint pt;
    if(IsVertical())
        pt.y=sz.cy*dwState;
    else
        pt.x=sz.cx*dwState;
    CRect rcSour(pt,sz);
    pRT->DrawBitmap9Patch(rcDraw,m_pImg,&rcSour,&m_rcMargin,GetExpandMode(),byAlpha);
}

UINT SSkinImgFrame::GetExpandMode()
{
    return MAKELONG(m_bTile?EM_TILE:EM_STRETCH,m_filterLevel);
}

void SSkinImgFrame::_Scale(ISkinObj *skinObj, int nScale)
{
	SSkinImgList::_Scale(skinObj,nScale);
	SSkinImgFrame * pClone = sobj_cast<SSkinImgFrame>(skinObj);
	pClone->m_rcMargin.left = MulDiv(m_rcMargin.left , nScale, 100);
	pClone->m_rcMargin.top = MulDiv(m_rcMargin.top , nScale, 100);
	pClone->m_rcMargin.right = MulDiv(m_rcMargin.right , nScale, 100);
	pClone->m_rcMargin.bottom = MulDiv(m_rcMargin.bottom , nScale, 100);
}


//////////////////////////////////////////////////////////////////////////
// SSkinButton
SSkinButton::SSkinButton()
    : m_nCornerRadius(2)
	, m_fCornerPercent(0.0)
{
    m_colors.m_crBorder[0] = RGB(0x70, 0x70, 0x70);
	m_colors.m_crBorder[1] = CR_INVALID;		// 不改变 原有的效果
	m_colors.m_crBorder[2] = CR_INVALID;
	m_colors.m_crBorder[3] = CR_INVALID;

    m_colors.m_crUp[0] = (RGB(0xEE, 0xEE, 0xEE));
    m_colors.m_crDown[0] = (RGB(0xD6, 0xD6, 0xD6));
    m_colors.m_crUp[1] = (RGB(0xEE, 0xEE, 0xEE));
    m_colors.m_crDown[1] = (RGB(0xE0, 0xE0, 0xE0));
    m_colors.m_crUp[2] = (RGB(0xCE, 0xCE, 0xCE));
    m_colors.m_crDown[2] = (RGB(0xC0, 0xC0, 0xC0));
    m_colors.m_crUp[3] = (RGB(0x8E, 0x8E, 0x8E));
    m_colors.m_crDown[3] = (RGB(0x80, 0x80, 0x80));
}

void SSkinButton::_Draw(IRenderTarget *pRT, LPCRECT prcDraw, DWORD dwState,BYTE byAlpha)
{
	int nCorner = m_nCornerRadius;
	if (m_fCornerPercent > 0.0)
	{
		int nW = prcDraw->right - prcDraw->left;
		int nH = prcDraw->bottom - prcDraw->top;
		nCorner = (nW < nH) ? (int)(nW * m_fCornerPercent) : (int)(nH * m_fCornerPercent);
	}
   	// 只有 在 需要渐变的情况下 才 需要 这个
	if (m_colors.m_crUp[dwState] != m_colors.m_crDown[dwState])
	{
		CAutoRefPtr<IRegion> rgnClip;
		if (nCorner > 2)
		{
			GETRENDERFACTORY->CreateRegion(&rgnClip);
			//the last two params of CreateRoundRectRgn are width and height of ellipse, thus we should multiple corner radius by 2.
			HRGN hRgn = ::CreateRoundRectRgn(prcDraw->left, prcDraw->top, prcDraw->right+1, prcDraw->bottom+1, nCorner*2, nCorner*2);
			rgnClip->SetRgn(hRgn);
			DeleteObject(hRgn);

			pRT->PushClipRegion(rgnClip);
		}
		CRect rcDraw = *prcDraw;
		rcDraw.DeflateRect(1, 1);
		pRT->GradientFill(rcDraw, TRUE, m_colors.m_crUp[dwState], m_colors.m_crDown[dwState], byAlpha);
		if (nCorner > 2)
		{
			pRT->PopClip();
		}
	}
    else
    {
        SColor cr(m_colors.m_crDown[dwState]);
        cr.updateAlpha(byAlpha);
		if(nCorner > 0)
			pRT->FillSolidRoundRect(prcDraw, CPoint(nCorner, nCorner), cr.toCOLORREF());
		else
			pRT->FillSolidRect(prcDraw,cr.toCOLORREF());
    }

	
	if (CR_INVALID == m_colors.m_crBorder[dwState])	//  不改变 原因的 效果
		dwState = 0;
	// 画 边框
    CAutoRefPtr<IPen> pPen, pOldPen;
    pRT->CreatePen(PS_SOLID, m_colors.m_crBorder[dwState] ,1, &pPen);
    pRT->SelectObject(pPen, (IRenderObj**)&pOldPen);
    pRT->DrawRoundRect(prcDraw, CPoint(nCorner, nCorner));
    pRT->SelectObject(pOldPen);   
}

BOOL SSkinButton::IgnoreState()
{
    return FALSE;
}

int SSkinButton::GetStates()
{
    return 4;
}

void SSkinButton::SetColors( COLORREF crUp[4],COLORREF crDown[4],COLORREF crBorder[4] )
{
    memcpy(m_colors.m_crUp, crUp, 4 * sizeof(COLORREF));
    memcpy(m_colors.m_crDown, crDown, 4 * sizeof(COLORREF));
	memcpy(m_colors.m_crBorder, crBorder, 4 * sizeof(COLORREF));
}

void SSkinButton::OnColorize(COLORREF cr)
{
    if(!m_bEnableColorize) return;
    if(m_crColorize == cr) return;
    if(cr == 0)
    {
        memcpy(&m_colors,&m_colorsBackup,sizeof(BTNCOLORS));
        m_crColorize = 0;
    }else
    {
        if(m_crColorize!=0)
        {//从备份里获取数据
            memcpy(&m_colors,&m_colorsBackup,sizeof(BTNCOLORS));
        }else
        {//将数据备份
            memcpy(&m_colorsBackup,&m_colors,sizeof(BTNCOLORS));
        }
        m_crColorize = cr;

        //调整颜色值
        for(int i=0;i<4;i++)
        {
			SDIBHelper::Colorize(m_colors.m_crBorder[i], m_crColorize);
            SDIBHelper::Colorize(m_colors.m_crDown[i],m_crColorize);
            SDIBHelper::Colorize(m_colors.m_crUp[i],m_crColorize);
        }
    }
}

ISkinObj * SSkinButton::Scale(int nScale)
{
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// SSkinGradation
SSkinGradation::SSkinGradation()
    : m_bVert(TRUE)
    , m_crFrom(CR_INVALID)
    , m_crTo(CR_INVALID)
    , m_crColorize(0)
{
}

void SSkinGradation::_Draw(IRenderTarget *pRT, LPCRECT prcDraw, DWORD dwState,BYTE byAlpha)
{
    pRT->GradientFill(prcDraw,m_bVert,m_crFrom,m_crTo,byAlpha);
}

void SSkinGradation::OnColorize(COLORREF cr)
{
    if(!m_bEnableColorize) return;
    if(m_crColorize == cr) return;
    if(m_crColorize!=0)
    {
        m_crFrom = m_crFromBackup;
        m_crTo = m_crToBackup;
    }else
    {
        m_crFromBackup = m_crFrom;
        m_crToBackup = m_crTo;
    }
    m_crColorize = cr;
    SDIBHelper::Colorize(m_crFrom,cr);
    SDIBHelper::Colorize(m_crTo,cr);
}

ISkinObj * SSkinGradation::Scale(int nScale)
{
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// SScrollbarSkin
SSkinScrollbar::SSkinScrollbar():m_nMargin(0),m_bHasGripper(FALSE),m_bHasInactive(FALSE)
{
    
}

CRect SSkinScrollbar::GetPartRect(int nSbCode, int nState,BOOL bVertical)
{
    CSize sz=GetSkinSize();
    CSize szFrame(sz.cx/9,sz.cx/9);
    if(nSbCode==SB_CORNOR)
    {
        return CRect(CPoint(szFrame.cx*8,0),szFrame);
    }else if(nSbCode==SB_THUMBGRIPPER)
    {
        return CRect(CPoint(szFrame.cx*8,(1+(bVertical?0:1))*szFrame.cy),szFrame);
    }else
    {
        if(nState==SBST_INACTIVE && !m_bHasInactive)
        {
            nState=SBST_NORMAL;
        }
        CRect rcRet;
        int iPart=-1;
        switch(nSbCode)
        {
        case SB_LINEUP:
            iPart=0;
            break;
        case SB_LINEDOWN:
            iPart=1;
            break;
        case SB_THUMBTRACK:
            iPart=2;
            break;
        case SB_PAGEUP:
        case SB_PAGEDOWN:
            iPart=3;
            break;
        }
        if(!bVertical) iPart+=4;
        
        return CRect(CPoint(szFrame.cx*iPart,szFrame.cy*nState),szFrame);
    }
}

void SSkinScrollbar::_Draw(IRenderTarget *pRT, LPCRECT prcDraw, DWORD dwState,BYTE byAlpha)
{
    if(!m_pImg) return;
    int nSbCode=LOWORD(dwState);
    int nState=LOBYTE(HIWORD(dwState));
    BOOL bVertical=HIBYTE(HIWORD(dwState));
    CRect rcMargin(0,0,0,0);
    if(bVertical)
        rcMargin.top=m_nMargin,rcMargin.bottom=m_nMargin;
    else
        rcMargin.left=m_nMargin,rcMargin.right=m_nMargin;

    CRect rcSour=GetPartRect(nSbCode,nState,bVertical);
    
    pRT->DrawBitmap9Patch(prcDraw,m_pImg,&rcSour,&rcMargin,m_bTile?EM_TILE:EM_STRETCH,byAlpha);
    
    if(nSbCode==SB_THUMBTRACK && m_bHasGripper)
    {
        rcSour=GetPartRect(SB_THUMBGRIPPER,0,bVertical);
        CRect rcDraw=*prcDraw;
        
        if (bVertical)
            rcDraw.top+=(rcDraw.Height()-rcSour.Height())/2,rcDraw.bottom=rcDraw.top+rcSour.Height();
        else
            rcDraw.left+=(rcDraw.Width()-rcSour.Width())/2,rcDraw.right=rcDraw.left+rcSour.Width();
        pRT->DrawBitmap9Patch(&rcDraw,m_pImg,&rcSour,&rcMargin,m_bTile?EM_TILE:EM_STRETCH,byAlpha);
    }
}

void SSkinScrollbar::_Scale(ISkinObj *skinObj, int nScale)
{
	__super::_Scale(skinObj,nScale);

	SSkinScrollbar *pRet = sobj_cast<SSkinScrollbar>(skinObj);
	pRet->m_nMargin = MulDiv(m_nMargin,nScale,100);
	pRet->m_bHasInactive = m_bHasInactive;
	pRet->m_bHasGripper = m_bHasGripper;
}


//////////////////////////////////////////////////////////////////////////
// SSkinColor
SSkinColorRect::SSkinColorRect()
	: m_nRadius(0)
	, m_fCornerPercent(0.0)
	, m_nBorderWidth(0)
{
    m_crStates[0]=RGBA(255,255,255,255);
    m_crStates[1]=CR_INVALID;
    m_crStates[2]=CR_INVALID;
    m_crStates[3]=CR_INVALID;
	m_crBorders[0] = m_crBorders[1] = m_crBorders[2] = m_crBorders[3] = CR_INVALID;
}

SSkinColorRect::~SSkinColorRect()
{
}

void SSkinColorRect::_Draw(IRenderTarget *pRT, LPCRECT prcDraw, DWORD dwState,BYTE byAlpha)
{
	if(dwState > 3) return;

	int nCorner = m_nRadius;
	if (m_fCornerPercent > 0.0)
	{
		int nW = prcDraw->right - prcDraw->left;
		int nH = prcDraw->bottom - prcDraw->top;
		nCorner = (nW < nH) ? (int)(nW * m_fCornerPercent) : (int)(nH * m_fCornerPercent);
	}
	    
    if(m_crStates[dwState] == CR_INVALID)
        dwState = 0;
    SColor cr(m_crStates[dwState]);
    cr.updateAlpha(byAlpha);
	if (nCorner > 0)
		pRT->FillSolidRoundRect(prcDraw, CPoint(nCorner, nCorner), cr.toCOLORREF());
	else
		pRT->FillSolidRect(prcDraw, cr.toCOLORREF());

	if (m_crBorders[dwState] != CR_INVALID && m_nBorderWidth>0)
	{
		CAutoRefPtr<IPen> pen, oldPen;
		pRT->CreatePen(PS_SOLID, m_crBorders[dwState], m_nBorderWidth, (IPen**)&pen);
		pRT->SelectObject(pen, (IRenderObj**)&oldPen);
		if (nCorner > 0)
			pRT->DrawRoundRect(prcDraw, CPoint(nCorner, nCorner));
		else
			pRT->DrawRectangle(prcDraw);
		pRT->SelectObject(oldPen);
	}
}

int SSkinColorRect::GetStates()
{
	int nStates = 4;
	for(int i=3;i>=0;i--)
	{
		if(m_crStates[i] == CR_INVALID) nStates--;
		else break;
	}
	return nStates;
}

ISkinObj * SSkinColorRect::Scale(int nScale)
{
	return NULL;
}


//////////////////////////////////////////////////////////////////////////

SSkinShape::SSkinShape() :m_crSolid(CR_INVALID),m_shape(rectangle)
{

}

void SSkinShape::OnInitFinished(pugi::xml_node xmlNode)
{
	pugi::xml_node xmlSolid = xmlNode.child(L"solid");
	if(xmlSolid)
	{
		m_crSolid = GETCOLOR(xmlSolid.attribute(L"color").as_string());
	}
	pugi::xml_node xmlGrident = xmlNode.child(SGradient::GetClassName());
	if(xmlGrident)
	{
		if (!m_gradient) m_gradient.Attach(new SGradient());
		m_gradient->InitFromXml(xmlGrident);
	}
	pugi::xml_node xmlSize = xmlNode.child(SShapeSize::GetClassName());
	if(xmlSize)
	{
		if (!m_shapeSize) m_shapeSize.Attach(new SShapeSize());
		m_shapeSize->InitFromXml(xmlSize);
	}
	pugi::xml_node xmlStoke = xmlNode.child(SStroke::GetClassName());
	if(xmlStoke)
	{
		if (!m_stroke) m_stroke.Attach(new SStroke());
		m_stroke->InitFromXml(xmlStoke);
	}
	pugi::xml_node xmlConner = xmlNode.child(SCornerSize::GetClassName());
	if(xmlConner)
	{
		if (!m_cornerSize) m_cornerSize.Attach(new SCornerSize());
		m_cornerSize->InitFromXml(xmlConner);
	}
	pugi::xml_node xmlRing = xmlNode.child(SShapeRing::GetClassName());
	if (xmlRing)
	{
		if (!m_ringParam) m_ringParam.Attach(new SShapeRing());
		m_ringParam->InitFromXml(xmlRing);
	}
}

void SSkinShape::_Scale(ISkinObj * pObj, int nScale)
{
	__super::_Scale(pObj, nScale);

	SSkinShape * pRet = sobj_cast<SSkinShape>(pObj);
	SASSERT(pRet);
	pRet->m_shape = m_shape;
	pRet->m_crSolid = m_crSolid;
	pRet->m_shapeSize = m_shapeSize;
	pRet->m_cornerSize = m_cornerSize;
	pRet->m_stroke = m_stroke;
	pRet->m_gradient = m_gradient;
	pRet->m_ringParam = m_ringParam;
}


SIZE SSkinShape::GetSkinSize()
{
	if (!m_shapeSize) return CSize();

	SIZE szRet = {m_shapeSize->m_width.toPixelSize(GetScale()),m_shapeSize->m_height.toPixelSize(GetScale())};
	return szRet;
}

void SSkinShape::_Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)
{
	POINT ptConner = {0,0};
	if(m_cornerSize)
	{
		ptConner = m_cornerSize->GetConner(GetScale());
	}

	RECT rcDest = *rcDraw;
	if(m_crSolid != CR_INVALID)
	{
		CAutoRefPtr<IBrush> brush,oldBrush;
		pRT->CreateSolidColorBrush(m_crSolid,&brush);
		pRT->SelectObject(brush,(IRenderObj**)&oldBrush);
		
		switch(m_shape)
		{
		case rectangle:
			if(m_cornerSize)
				pRT->FillSolidRoundRect(&rcDest,ptConner,m_crSolid);
			else
				pRT->FillSolidRect(&rcDest,m_crSolid);
			break;
		case oval:
			pRT->FillEllipse(&rcDest);
			break;
		case ring:
			break;
		}
		pRT->SelectObject(oldBrush);
	}

	if(m_gradient!=NULL)
	{
		CAutoRefPtr<IRegion> region;
		pRT->CreateRegion(&region);
		RECT rcGradient = rcDest;
		//set clip
		switch(m_shape)
		{
		case rectangle:
			if(m_cornerSize)
			{
				region->CombineRoundRect(&rcGradient,ptConner,RGN_COPY);
			}
			break;
		case oval:
			region->CombineEllipse(&rcGradient,RGN_COPY);
			break;
		case ring:
			break;
		}
		if(!region->IsEmpty())
			pRT->PushClipRegion(region);

		
		m_gradient->Draw(pRT,&rcGradient,byAlpha,GetScale());

		if(!region->IsEmpty())
			pRT->PopClip();
	}

	if(m_stroke != NULL)
	{
		CAutoRefPtr<IPen> pPen,oldPen;
		int nPenWidth = m_stroke->m_width.toPixelSize(GetScale());
		pRT->CreatePen(m_stroke->m_style,m_stroke->m_color,nPenWidth,&pPen);
		pRT->SelectObject(pPen,(IRenderObj**)&oldPen);
		RECT rcStroke = rcDest;
		::InflateRect(&rcStroke, -nPenWidth / 2, -nPenWidth / 2);
		switch(m_shape)
		{
		case rectangle:
			if(m_cornerSize)
				pRT->DrawRoundRect(&rcStroke,ptConner);
			else
				pRT->DrawRectangle(&rcStroke);
			break;
		case oval:
			pRT->DrawEllipse(&rcStroke);
			break;
		case ring:
			if(m_ringParam){
				POINT ptCenter = { (rcStroke.left + rcStroke.right) / 2,(rcStroke.top + rcStroke.bottom) / 2 };
				int   nRadius = smin(rcStroke.right - rcStroke.left, rcStroke.bottom - rcStroke.top) / 2;
				RECT rcRing = { ptCenter.x - nRadius,ptCenter.y - nRadius,ptCenter.x + nRadius,ptCenter.y + nRadius };
				pRT->DrawArc(&rcRing, m_ringParam->m_startAngle, m_ringParam->m_sweepAngle, false);
			}
			break;
		}
		pRT->SelectObject(oldPen);
	}

}



void SSkinShape::SGradient::Draw(IRenderTarget *pRT, LPCRECT rcDraw, BYTE byAlpha,int nScale)
{
	pRT->GradientFill2(rcDraw,m_Type,m_crStart,m_crCenter,m_crEnd,m_angle,m_centerX,m_centerY,m_radius.toPixelSize(nScale),byAlpha);
}


HRESULT SSkinShape::SCornerSize::OnAttrRadius(const SStringW strValue,BOOL bLoading)
{
	SStringWList values;
	size_t nValues = SplitString(strValue,L',',values);
	if(nValues == 1)
	{
		m_radiusX=
		m_radiusY= GETLAYOUTSIZE(values[0]);
		return S_OK;
	}else if(nValues == 2)
	{
		m_radiusX= GETLAYOUTSIZE(values[0]);
		m_radiusY= GETLAYOUTSIZE(values[1]);
		return S_OK;
	}
	return E_INVALIDARG;
}


//////////////////////////////////////////////////////////////////////////
// SSKinGroup
int SSKinGroup::GetStates()
{
	return 4;
}

void SSKinGroup::_Draw(IRenderTarget *pRT, LPCRECT rcDraw, DWORD dwState,BYTE byAlpha)
{
	if((int)dwState>= GetStates())
		return;
	ISkinObj *pSkin = m_skins[dwState];
	if(!pSkin) return;
	pSkin->Draw(pRT,rcDraw,0,byAlpha);
}

SIZE SSKinGroup::GetSkinSize()
{
	for(int i=0;i<GetStates();i++)
	{
		if(m_skins[i]) return m_skins[i]->GetSkinSize();
	}
	return CSize();
}

/*
IBitmap中的内存为RGBA格式，.9中使用alpha通道==0或者255来确定如何拉伸
*/
HRESULT SSkinImgFrame2::OnAttrSrc(const SStringW & strValue,BOOL bLoading)
{
	IBitmap *pImg=LOADIMAGE2(strValue); 
	if(!pImg) return E_FAIL;
	int nWid=pImg->Width();
	int nHei = pImg->Height();
	if(nWid<2 || nHei<2) return E_INVALIDARG;
	LPBYTE pBuf = (LPBYTE)pImg->LockPixelBits();

	int left,right,top,bottom;
	//检测第一扫描行中定义的left,right位置
	LPBYTE p = pBuf;
	int i=1;
	while(i<nWid-1 && p[3]==0) i++,p+=4;
	left = i-1;

	while(i<nWid-1 && p[3]!=0) i++,p+=4;
	right = i-1;

	//检测第一列中定义的top,bottom位置
	i=1;
	p = pBuf+(nWid*4);
	while(i<nHei-1 && p[3]==0) i++,p+=nWid*4;
	top = i-1;
	while(i<nHei-1 && p[3]!=0) i++,p+=nWid*4;
	bottom = i-1;

	
	m_rcMargin.left = left;
	m_rcMargin.right = nWid - 2 - right;
	m_rcMargin.top = top;
	m_rcMargin.bottom = nHei - 2 - bottom;

	HRESULT hRet = S_OK;
	IBitmap *pImgCenter=NULL;
	if(!pImg->GetRenderFactory()->CreateBitmap(&pImgCenter))
	{
		return E_OUTOFMEMORY;
	}
	hRet = pImgCenter->Init(nWid-2,nHei-2);
	if(hRet!=S_OK)
	{
		return hRet;
	}
	LPBYTE pBuf2= (LPBYTE)pImgCenter->LockPixelBits();


	LPBYTE pSrc=pBuf+(nWid*4);
	LPBYTE pDst = pBuf2;

	i=1;
	pSrc+=4;
	while(i<nHei-1)
	{
		memcpy(pDst,pSrc,(nWid-2)*4);
		pDst += (nWid-2)*4;
		pSrc += nWid*4;
		i++;
	}

	pImg->UnlockPixelBits(pBuf);
	pImgCenter->UnlockPixelBits(pBuf2);

	m_pImg = pImgCenter;
	pImgCenter->Release();
	pImg->Release();

	return hRet;
}

}//namespace SOUI