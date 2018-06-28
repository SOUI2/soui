#pragma once
namespace SOUI{

//实现一个只有纵向滚动条皮肤
/*img format: 1-normal, 2-hover, 3-pushdown, 4-disable
1 2 3 4 //for thumb
1 2 3 4 //for rail
*/
class SSkinVScrollbar : public SSkinScrollbar
{
	SOUI_CLASS_NAME(SSkinVScrollbar, L"vscrollbar")

public:

	SSkinVScrollbar()
	{
        m_nStates=4;
	}

	//不支持显示上下箭头
	virtual BOOL HasArrow(){return FALSE;}
	virtual int GetIdealSize(){
        if(!GetImage()) return 0;
        return GetImage()->Width()/m_nStates;
	}
protected:
	//返回源指定部分在原位图上的位置。
	virtual CRect GetPartRect(int nSbCode, int nState,BOOL bVertical)
	{
		CRect rc;
		if(!bVertical 
            || nSbCode==SB_LINEDOWN 
            || nSbCode==SB_LINEUP 
            || nSbCode == SB_CORNOR 
            || nSbCode==SB_THUMBGRIPPER) 
            return rc;

        if(nState==SBST_INACTIVE && !m_bHasInactive)
        {
            nState=SBST_NORMAL;
        }

        SASSERT(GetImage());
        rc.right = GetImage()->Width()/m_nStates;
        rc.bottom = GetImage()->Height()/2;
        
		rc.OffsetRect(rc.Width()*nState,0);
        if(nSbCode == SB_PAGEUP || nSbCode == SB_PAGEDOWN)
        {
            rc.OffsetRect(0,rc.Height());
        }
		return rc;
	}
};

}