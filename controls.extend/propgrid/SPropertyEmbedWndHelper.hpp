#pragma once

namespace SOUI
{
    template<class T>
    class TplPropEmbedWnd : public T
    {
    public:
        template<class P1>
        TplPropEmbedWnd(P1 p1):T(p1),m_bUpdateData(TRUE){}
        template<class P1,class P2>
        TplPropEmbedWnd(P1 p1,P2 p2):T(p1,p2),m_bUpdateData(TRUE){}

        void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
        {
            if(nChar == VK_ESCAPE)
            {
                m_bUpdateData=FALSE;
                T::GetParent()->SetFocus();
            }else
            {
                T::OnKeyDown(nChar,nRepCnt,nFlags);
            }
        }

        void OnKillFocus(SWND wndFocus)
        {
            if(m_bUpdateData) T::UpdateData();
            T::OnKillFocus(wndFocus);
            T::GetOwner()->OnInplaceActive(false);
        }

        SOUI_MSG_MAP_BEGIN()
            MSG_WM_KEYDOWN(OnKeyDown)
            MSG_WM_KILLFOCUS_EX(OnKillFocus)
        SOUI_MSG_MAP_END()

        virtual void OnFinalRelease()
        {
            delete this;
        }
    private:
        BOOL m_bUpdateData;
    };

}