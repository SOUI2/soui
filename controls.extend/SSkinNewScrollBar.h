#pragma once

namespace SOUI{
//实现一个只有纵向滚动条皮肤
/*img format: 1-normal, 2-hover, 3-pushdown, 4-disable
1 2 3 4 //for thumb
1 2 3 4 //for rail
*/
class SSkinNewScrollbar : public SSkinScrollbar
{
    SOUI_CLASS_NAME(SSkinNewScrollbar, L"newScrollbar")

public:

    SSkinNewScrollbar():m_pImgVert(NULL),m_pImgHorz(NULL)
    {
        m_nStates=4;
    }

    ~SSkinNewScrollbar()
    {
        if(m_pImgVert) m_pImgVert->Release();
        if(m_pImgHorz) m_pImgHorz->Release();
    }

    //不支持显示上下箭头
    virtual BOOL HasArrow(){return FALSE;}
    virtual int GetIdealSize(){
        SASSERT(m_pImgHorz && m_pImgVert);
        return m_pImgVert->Width()/m_nStates;
    }
protected:
	virtual void OnColorize(COLORREF cr){

	}

    virtual void _Draw(IRenderTarget *pRT, LPCRECT prcDraw, DWORD dwState,BYTE byAlpha)
    {
        if(!m_pImgHorz || !m_pImgVert) return;
        int nSbCode=LOWORD(dwState);
        int nState=LOBYTE(HIWORD(dwState));
        BOOL bVertical=HIBYTE(HIWORD(dwState));

        CRect rcSour=GetPartRect(nSbCode,nState,bVertical);
        CRect rcMargin(0,0,0,0);
        if(bVertical)
        {
            rcMargin.top=m_nMargin,rcMargin.bottom=m_nMargin;
            pRT->DrawBitmap9Patch(prcDraw,m_pImgVert,&rcSour,&rcMargin,m_bTile?EM_TILE:EM_STRETCH,byAlpha);
        }
        else
        {
            rcMargin.left=m_nMargin,rcMargin.right=m_nMargin;
            pRT->DrawBitmap9Patch(prcDraw,m_pImgHorz,&rcSour,&rcMargin,m_bTile?EM_TILE:EM_STRETCH,byAlpha);
        }

    }


    //返回源指定部分在原位图上的位置。
    virtual CRect GetPartRect(int nSbCode, int nState,BOOL bVertical)
    {
        CRect rc;
        if(
            nSbCode==SB_LINEDOWN 
            || nSbCode==SB_LINEUP 
            || nSbCode == SB_CORNOR 
            || nSbCode==SB_THUMBGRIPPER) 
            return rc;
        
        if(bVertical)
        {
            SASSERT(m_pImgVert);
            rc.right = m_pImgVert->Width()/m_nStates;
            rc.bottom = m_pImgVert->Height()/2;

            rc.OffsetRect(rc.Width()*nState,0);
            if(nSbCode == SB_PAGEUP || nSbCode == SB_PAGEDOWN)
            {
                rc.OffsetRect(0,rc.Height());
            }

        }else
        {
            SASSERT(m_pImgHorz);
            rc.bottom = m_pImgHorz->Height()/m_nStates;
            rc.right = m_pImgHorz->Width()/2;

            rc.OffsetRect(0,rc.Height()*nState);
            if(nSbCode == SB_PAGEUP || nSbCode == SB_PAGEDOWN)
            {
                rc.OffsetRect(rc.Width(),0);
            }

        }
        return rc;
    }

    IBitmap *m_pImgVert,        //垂直滚动条图片,子图垂直排列
            *m_pImgHorz;        //水平滚动条图片,子图水平排列

    SOUI_ATTRS_BEGIN()
        ATTR_IMAGE(L"srcVert", m_pImgVert,FALSE)    //skinObj引用的图片文件定义在uires.idx中的name属性。
        ATTR_IMAGE(L"srcHorz", m_pImgHorz,FALSE)    //skinObj引用的图片文件定义在uires.idx中的name属性。
    SOUI_ATTRS_END()
};

}