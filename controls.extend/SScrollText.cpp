#include "stdafx.h"
#include "SScrollText.h"

namespace SOUI
{
    SScrollText::SScrollText(void):m_nSpeed(20),m_nOffset(0),m_nScrollWidth(0),m_nRollType(0)
    {
    }

    SScrollText::~SScrollText(void)
    {
    }

    void SScrollText::OnPaint(IRenderTarget *pRT)
    {
        SPainter painter;
        BeforePaint(pRT,painter);
        CRect rcClient = GetClientRect();
        if(m_nScrollWidth==0)
        {
            pRT->DrawText(m_strText.GetText(),m_strText.GetText().GetLength(),&rcClient,DT_SINGLELINE|DT_CENTER|DT_VCENTER);
        }else
        {
            pRT->PushClipRect(&rcClient);
                        
            CRect rcText = rcClient;
            rcText.left -= m_nOffset;
            pRT->DrawText(m_strText.GetText(),m_strText.GetText().GetLength(),&rcText,DT_SINGLELINE|DT_VCENTER);
			if (m_nRollType==0)
			{
				if(m_nScrollWidth - m_nOffset < rcClient.Width())
				{
					rcText.left += m_nScrollWidth;
					//                 pRT->SetTextColor(RGBA(0,0,0,255));
					pRT->DrawText(m_strText.GetText(),m_strText.GetText().GetLength(),&rcText,DT_SINGLELINE|DT_VCENTER);
				}
			}
            
            
            pRT->PopClip();
        }
        AfterPaint(pRT,painter);
    }

    void SScrollText::OnSize(UINT nType, CSize size)
    {
        __super::OnSize(nType,size);
        UpdateScrollInfo(size);
    }

    void SScrollText::OnShowWindow(BOOL bShow, UINT nStatus)
    {
        __super::OnShowWindow(bShow,nStatus);
        if(m_nScrollWidth>0)
        {
            if(IsVisible(TRUE))
            {
                GetContainer()->RegisterTimelineHandler(this);
                m_nNextInterval = m_nSpeed;
            }
            else
            {
                GetContainer()->UnregisterTimelineHandler(this);
            }
        }
    }

    void SScrollText::SetWindowText(const SStringT & strText)
    {
		__super::SetWindowText(strText);
        UpdateScrollInfo(GetClientRect().Size());//重新计算滚动长度
    }

    void SScrollText::UpdateScrollInfo(CSize size)
    {
        CAutoRefPtr<IRenderTarget> pRT;
        GETRENDERFACTORY->CreateRenderTarget(&pRT,0,0);
        BeforePaintEx(pRT);
        SIZE sz;
        pRT->MeasureText(m_strText.GetText(),m_strText.GetText().GetLength(),&sz);

        if(sz.cx - size.cx>0)
        {
            m_nScrollWidth = sz.cx;
			if (m_nRollType==1)
			{
				m_nOffset=-size.cx;
			}
            if(m_nScrollWidth>0)
            {
                if(IsVisible(TRUE))
                {
                    GetContainer()->RegisterTimelineHandler(this);
                    m_nNextInterval = m_nSpeed;
                }
                else
                {
                    GetContainer()->UnregisterTimelineHandler(this);
                }
            }

        }
        else
        {
            m_nOffset = 0;
            m_nScrollWidth = 0;
        }
    }

    void SScrollText::OnNextFrame()
    {
        m_nNextInterval -= 10;
        if(m_nNextInterval < 0)
        {
            m_nNextInterval = m_nSpeed;
			if (m_nScrollWidth>0)
			{ 
				m_nOffset ++;
				if(m_nOffset>m_nScrollWidth)
				{
					if (m_nRollType==0)
					{
						m_nOffset = 0;
					}
					else if(m_nRollType==1)
					{
						m_nOffset = -GetClientRect().Width();
					}
				}
				Invalidate();
			}
        }
    }

    void SScrollText::OnDestroy()
    {
        GetContainer()->UnregisterTimelineHandler(this);
        SStatic::OnDestroy();
    }
}
