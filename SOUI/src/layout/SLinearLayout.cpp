#include "souistd.h"
#include "layout\SLinearLayout.h"
#include "helper\SplitString.h"
#include <algorithm>

namespace SOUI
{

    SLinearLayoutParam::SLinearLayoutParam()
	{
		Clear();
	}

    bool SLinearLayoutParam::IsMatchParent(ORIENTATION orientation) const
    {
        switch(orientation)
        {
        case Horz:
            return width.isMatchParent();
        case Vert:
            return height.isMatchParent();
        case Any:
            return IsMatchParent(Horz)|| IsMatchParent(Vert);
        case Both:
        default:
            return IsMatchParent(Horz) && IsMatchParent(Vert);
        }
    }

	bool SLinearLayoutParam::IsWrapContent(ORIENTATION orientation) const
	{
        switch(orientation)
        {
        case Horz:
            return width.isWrapContent();
        case Vert:
            return height.isWrapContent();
        case Any:
            return IsWrapContent(Horz)|| IsWrapContent(Vert);
        case Both:
        default:
            return IsWrapContent(Horz) && IsWrapContent(Vert);
        }
	}

    bool SLinearLayoutParam::IsSpecifiedSize(ORIENTATION orientation) const
    {
        switch(orientation)
        {
        case Horz:
            return width.isSpecifiedSize();
        case Vert:
            return height.isSpecifiedSize();
        case Any:
            return IsSpecifiedSize(Horz)|| IsSpecifiedSize(Vert);
        case Both:
        default:
            return IsSpecifiedSize(Horz) && IsSpecifiedSize(Vert);
        }

    }

	SLayoutSize SLinearLayoutParam::GetSpecifiedSize(ORIENTATION orientation) const
    {
        switch(orientation)
        {
        case Horz:
            return width;
        case Vert:
            return height;
        case Any: 
        case Both:
        default:
            SASSERT_FMTA(FALSE,"GetSpecifiedSize can only be applied for Horz or Vert");
            return SLayoutSize();
        }
    }


    HRESULT SLinearLayoutParam::OnAttrSize(const SStringW & strValue,BOOL bLoading)
    {
		SStringWList szStr ;
		if(2!=SplitString(strValue,L',',szStr)) return E_FAIL;

		OnAttrWidth(szStr[0],bLoading);
		OnAttrHeight(szStr[1],bLoading);
        return S_OK;
    }

    HRESULT SLinearLayoutParam::OnAttrWidth(const SStringW & strValue,BOOL bLoading)
    {
        if(strValue.CompareNoCase(L"matchParent") == 0)
			width.setMatchParent();
        else if(strValue.CompareNoCase(L"wrapContent") == 0)
			width.setWrapContent();
        else
			width = GETLAYOUTSIZE(strValue);
        return S_OK;
    }

    HRESULT SLinearLayoutParam::OnAttrHeight(const SStringW & strValue,BOOL bLoading)
    {
        if(strValue.CompareNoCase(L"matchParent") == 0)
            height.setMatchParent();
        else if(strValue.CompareNoCase(L"wrapContent") == 0)
            height.setWrapContent();
        else
			height = GETLAYOUTSIZE(strValue);
        return S_OK;
    }


	HRESULT SLinearLayoutParam::OnAttrExtend(const SStringW & strValue,BOOL bLoading)
	{
		SStringWList strList;
		size_t nSeg = SplitString(strValue,L',',strList);
		if(nSeg==1)
		{
			extend_left = 
				extend_top =
				extend_right =
				extend_bottom = GETLAYOUTSIZE(strList[0]);
			return S_OK;
		}else if(nSeg == 4)
		{
			extend_left = GETLAYOUTSIZE(strList[0]);
			extend_top = GETLAYOUTSIZE(strList[1]);
			extend_right = GETLAYOUTSIZE(strList[2]);
			extend_bottom = GETLAYOUTSIZE(strList[3]);
			return S_OK;
		}
		return E_INVALIDARG;
	}

	void SLinearLayoutParam::Clear()
	{
		width.setWrapContent();
		height.setWrapContent();
		weight = 0.0f;
		gravity = G_Undefined;
	}

	void SLinearLayoutParam::SetMatchParent(ORIENTATION orientation)
	{
        switch(orientation)
        {
        case Horz:
            width.setMatchParent();
            break;
        case Vert:
            height.setMatchParent();
            break;
        case Both:
            width.setMatchParent();
			height.setMatchParent();
            break;
        }
	}

	void SLinearLayoutParam::SetWrapContent(ORIENTATION orientation)
	{
        switch(orientation)
        {
        case Horz:
            width.setWrapContent();
            break;
        case Vert:
            height.setWrapContent();
            break;
        case Both:
            width.setWrapContent();
			height.setWrapContent();
            break;
        }
	}

	void SLinearLayoutParam::SetSpecifiedSize(ORIENTATION orientation, const SLayoutSize& layoutSize)
	{
        switch(orientation)
        {
        case Horz:
            width = layoutSize;
            break;
        case Vert:
            height = layoutSize;
            break;
        case Both:
            width = height = layoutSize;
            break;
        }

	}

	void * SLinearLayoutParam::GetRawData()
	{
		return (SLinearLayoutParamStruct*)this;
	}


	//////////////////////////////////////////////////////////////////////////
    SLinearLayout::SLinearLayout(void):m_gravity(G_Undefined)
    {
    }

    SLinearLayout::~SLinearLayout(void)
    {
    }

    void SLinearLayout::LayoutChildren(SWindow * pParent)
    {
        CRect rcParent = pParent->GetChildrenLayoutRect();
		        
        CSize *pSize = new CSize [pParent->GetChildrenCount()];
		SWindow ** pChilds = new SWindow * [pParent->GetChildrenCount()];
		int nChilds = 0;
		
        int offset = 0;
        float fWeight= 0.0f;

        {//assign width or height

            int iChild = 0;

			SWindow *pChild=pParent->GetNextLayoutChild(NULL);
            while(pChild)
            {
				SLinearLayoutParam *pLinearLayoutParam = pChild->GetLayoutParamT<SLinearLayoutParam>();

				int nScale = pChild->GetScale();

                CSize szChild(SIZE_WRAP_CONTENT,SIZE_WRAP_CONTENT);
                if(pLinearLayoutParam->IsMatchParent(Horz))
                    szChild.cx = rcParent.Width();
                else if(pLinearLayoutParam->IsSpecifiedSize(Horz))
                {
                    szChild.cx = pLinearLayoutParam->GetSpecifiedSize(Horz).toPixelSize(nScale);
                    szChild.cx += pLinearLayoutParam->extend_left.toPixelSize(nScale) + pLinearLayoutParam->extend_right.toPixelSize(nScale);
                }

                if(pLinearLayoutParam->IsMatchParent(Vert))
                    szChild.cy = rcParent.Height();
                else if(pLinearLayoutParam->IsSpecifiedSize(Vert))
                {
                    szChild.cy = pLinearLayoutParam->GetSpecifiedSize(Vert).toPixelSize(nScale);
                    szChild.cy += pLinearLayoutParam->extend_top.toPixelSize(nScale) + pLinearLayoutParam->extend_bottom.toPixelSize(nScale);
                }
                
                if(pLinearLayoutParam->weight > 0.0f)
                {
                    fWeight += pLinearLayoutParam->weight;
                }

                if(szChild.cx == SIZE_WRAP_CONTENT || szChild.cy == SIZE_WRAP_CONTENT)
                {
                    CSize szCalc = pChild->GetDesiredSize(szChild.cx,szChild.cy);
                    if(szChild.cx == SIZE_WRAP_CONTENT)
                    {
                        szChild.cx = szCalc.cx;
                        szChild.cx += pLinearLayoutParam->extend_left.toPixelSize(nScale) + pLinearLayoutParam->extend_right.toPixelSize(nScale);
                    }
                    if(szChild.cy == SIZE_WRAP_CONTENT) 
                    {
                        szChild.cy = szCalc.cy;
                        szChild.cy += pLinearLayoutParam->extend_top.toPixelSize(nScale) + pLinearLayoutParam->extend_bottom.toPixelSize(nScale);
                    }
                }

				pChilds[iChild] = pChild;
                pSize [iChild] = szChild;
                offset += m_orientation == Vert ? szChild.cy:szChild.cx;

				iChild++;
				pChild=pParent->GetNextLayoutChild(pChild);
            }

			nChilds = iChild;
        }

        int size = m_orientation == Vert? rcParent.Height():rcParent.Width();
        if(fWeight > 0.0f && size > offset)
        {//assign size by weight
            int nRemain = size - offset;
			for(int iChild = 0;iChild < nChilds;iChild ++)
            {
				SWindow *pChild = pChilds[iChild];

				SLinearLayoutParam *pLinearLayoutParam = pChild->GetLayoutParamT<SLinearLayoutParam>();

                if(pLinearLayoutParam->weight > 0.0f)
                {
                    LONG & szChild = m_orientation == Vert? pSize[iChild].cy:pSize[iChild].cx;
                    szChild += (int)(nRemain*pLinearLayoutParam->weight/fWeight);
                }
            }
        }
        {//assign position
            offset = 0;
			for(int iChild = 0;iChild < nChilds;iChild ++)
			{
				SWindow *pChild = pChilds[iChild];


                SLinearLayoutParam *pLinearLayoutParam = pChild->GetLayoutParamT<SLinearLayoutParam>();
				int nScale = pChild->GetScale();
                Gravity gravity = pLinearLayoutParam->gravity == G_Undefined? m_gravity:pLinearLayoutParam->gravity;
                if(gravity == G_Undefined) gravity = G_Left;

                if(m_orientation == Vert)
                {
                    CRect rcChild(CPoint(0,offset),pSize[iChild]);
                    rcChild.OffsetRect(rcParent.TopLeft());
                    if(gravity == G_Center)
                        rcChild.OffsetRect((rcParent.Width()-rcChild.Width())/2,0);
                    else if(gravity == G_Right)
                        rcChild.OffsetRect(rcParent.Width()-rcChild.Width(),0);
                    
                    CRect rcChild2 = rcChild;
                    rcChild2.DeflateRect(pLinearLayoutParam->extend_left.toPixelSize(nScale),
						pLinearLayoutParam->extend_top.toPixelSize(nScale),
						pLinearLayoutParam->extend_right.toPixelSize(nScale),
						pLinearLayoutParam->extend_bottom.toPixelSize(nScale)
						);

                    pChild->OnRelayout(rcChild2);

                    offset += rcChild.Height();
                }else
                {
                    CRect rcChild(CPoint(offset,0),pSize[iChild]);
                    rcChild.OffsetRect(rcParent.TopLeft());
                    if(gravity == G_Center)
                        rcChild.OffsetRect(0,(rcParent.Height()-rcChild.Height())/2);
                    else if(gravity == G_Right)
                        rcChild.OffsetRect(0,rcParent.Height()-rcChild.Height());

                    CRect rcChild2 = rcChild;
					rcChild2.DeflateRect(pLinearLayoutParam->extend_left.toPixelSize(nScale),
						pLinearLayoutParam->extend_top.toPixelSize(nScale),
						pLinearLayoutParam->extend_right.toPixelSize(nScale),
						pLinearLayoutParam->extend_bottom.toPixelSize(nScale));

                    pChild->OnRelayout(rcChild2);

                    offset += rcChild.Width();
                }

            }

        }

		delete []pChilds;
		delete []pSize;

    }

	//nWidth,nHeight == -1:wrap_content
	CSize SLinearLayout::MeasureChildren(SWindow * pParent,int nWidth,int nHeight) const
	{
		CSize *pSize = new CSize [pParent->GetChildrenCount()];


        ILayoutParam * pParentLayoutParam = pParent->GetLayoutParam();

		int iChild = 0;

		SWindow *pChild = pParent->GetNextLayoutChild(NULL);
		while(pChild)
		{
			SLinearLayoutParam *pLinearLayoutParam = pChild->GetLayoutParamT<SLinearLayoutParam>();
			int nScale = pChild->GetScale();
			CSize szChild(SIZE_WRAP_CONTENT,SIZE_WRAP_CONTENT);
			if(pLinearLayoutParam->IsMatchParent(Horz))
            {
                if(!pParentLayoutParam->IsWrapContent(Horz))
                    szChild.cx = nWidth;
            }
			else if(pLinearLayoutParam->IsSpecifiedSize(Horz))
            {
                szChild.cx = pLinearLayoutParam->GetSpecifiedSize(Horz).toPixelSize(nScale);
                szChild.cx += pLinearLayoutParam->extend_left.toPixelSize(nScale) + pLinearLayoutParam->extend_right.toPixelSize(nScale);
            }
			if(pLinearLayoutParam->IsMatchParent(Vert))
            {
                if(!pParentLayoutParam->IsWrapContent(Vert))
                    szChild.cy = nHeight;
            }
			else if(pLinearLayoutParam->IsSpecifiedSize(Vert))
            {
                szChild.cy = pLinearLayoutParam->GetSpecifiedSize(Vert).toPixelSize(nScale);
                szChild.cy += pLinearLayoutParam->extend_top.toPixelSize(nScale) + pLinearLayoutParam->extend_bottom.toPixelSize(nScale);
            }
			if(szChild.cx == SIZE_WRAP_CONTENT || szChild.cy == SIZE_WRAP_CONTENT)
			{
                int nWid = szChild.cx, nHei = szChild.cy;
                if(nWid == SIZE_WRAP_CONTENT)
                    nWid = nWidth * pParentLayoutParam->IsWrapContent(Horz)?-1:1; //把父窗口的WrapContent属性通过-1标志传递给GetDesiredSize
                if(nHei == SIZE_WRAP_CONTENT)
                    nHei = nHeight * pParentLayoutParam->IsWrapContent(Vert)?-1:1;//把父窗口的WrapContent属性通过-1标志传递给GetDesiredSize

				CSize szCalc = pChild->GetDesiredSize(nWid,nHei);
				if(szChild.cx == SIZE_WRAP_CONTENT) 
                {
                    szChild.cx = szCalc.cx;
                    szChild.cx += pLinearLayoutParam->extend_left.toPixelSize(nScale) + pLinearLayoutParam->extend_right.toPixelSize(nScale);
                }
				if(szChild.cy == SIZE_WRAP_CONTENT) 
                {
                    szChild.cy = szCalc.cy;
                    szChild.cy += pLinearLayoutParam->extend_top.toPixelSize(nScale) + pLinearLayoutParam->extend_bottom.toPixelSize(nScale);
                }
			}

			pSize [iChild++] = szChild;

			pChild = pParent->GetNextLayoutChild(pChild);
		}
		

		CSize szRet;
		for(int i=0;i<iChild;i++)
		{
			if(m_orientation == Horz)
			{
				szRet.cx += pSize[i].cx;
				szRet.cy = (std::max)(szRet.cy,pSize[i].cy);
			}else
			{
				szRet.cx = (std::max)(szRet.cx,pSize[i].cx);
				szRet.cy += pSize[i].cy;
			}
		}
		delete []pSize;
		return szRet;
	}

	bool SLinearLayout::IsParamAcceptable(ILayoutParam *pLayoutParam) const
	{
		return !!pLayoutParam->IsClass(SLinearLayoutParam::GetClassName());
	}

	ILayoutParam * SLinearLayout::CreateLayoutParam() const
	{
		return new SLinearLayoutParam();
	}

}
