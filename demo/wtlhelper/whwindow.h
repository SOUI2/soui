#pragma once


template <class T>
class CWHRoundRectFrameHelper
{
protected:

    SIZE m_sizeWnd;

    void OnSize(UINT nType, CSize size)
    {
        T *pT = static_cast<T*>(this);

        if (nType == SIZE_MINIMIZED)
            return;

        if (size == m_sizeWnd)
            return;

        CRect rcWindow, rcClient;
        CRgn rgnWindow, rgnMinus, rgnAdd;

        pT->CSimpleWnd::GetWindowRect(rcWindow);
        pT->CSimpleWnd::GetClientRect(rcClient);
        pT->CSimpleWnd::ClientToScreen(rcClient);

        rcClient.OffsetRect(- rcWindow.TopLeft());

        rgnWindow.CreateRectRgn(rcClient.left, rcClient.top + 2, rcClient.right, rcClient.bottom - 2);
        rgnAdd.CreateRectRgn(rcClient.left + 2, rcClient.top, rcClient.right - 2, rcClient.top + 1);
        rgnWindow.CombineRgn(rgnAdd, RGN_OR);
        rgnAdd.OffsetRgn(0, rcClient.Height() - 1);
        rgnWindow.CombineRgn(rgnAdd, RGN_OR);
        rgnAdd.SetRectRgn(rcClient.left + 1, rcClient.top + 1, rcClient.right - 1, rcClient.top + 2);
        rgnWindow.CombineRgn(rgnAdd, RGN_OR);
        rgnAdd.OffsetRgn(0, rcClient.Height() - 3);
        rgnWindow.CombineRgn(rgnAdd, RGN_OR);
        pT->CSimpleWnd::SetWindowRgn(rgnWindow);
		pT->SetMsgHandled(FALSE);
        m_sizeWnd = size;
    }

public:

    BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0)
    {
        BOOL bHandled = TRUE;

        switch(dwMsgMapID)
        {
        case 0:
            if (uMsg == WM_SIZE)
            {
                OnSize((UINT)wParam, _WTYPES_NS::CSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)));
                lResult = 0;
            }
            break;
        }
        return FALSE;
    }
};
