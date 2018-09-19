#include "souistd.h"
#include "control/SComboBase.h"

namespace SOUI
{

	const wchar_t * KStyle_Dropdown = L"dropdownStyle";//下拉列表风格，只包含root节点
	const wchar_t * KStyle_Edit		= L"editStyle";		//编辑框风格

    //////////////////////////////////////////////////////////////////////////
    // CComboEdit
    SComboEdit::SComboEdit( SWindow *pOwner )
    {
        SetOwner(pOwner);
    }

    void SComboEdit::OnMouseHover( WPARAM wParam, CPoint ptPos )
    {
        __super::OnMouseHover(wParam,ptPos);
        GetOwner()->SSendMessage(WM_MOUSEHOVER,wParam,MAKELPARAM(ptPos.x,ptPos.y));
    }

    void SComboEdit::OnMouseLeave()
    {
        __super::OnMouseLeave();
        GetOwner()->SSendMessage(WM_MOUSELEAVE);
    }

    void SComboEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        SWindow *pOwner = GetOwner();
        if (pOwner && (nChar == VK_DOWN || nChar == VK_ESCAPE))
        {
            pOwner->SSendMessage(WM_KEYDOWN, nChar, MAKELONG(nFlags, nRepCnt));
            return;
        }

        SetMsgHandled(FALSE);
    }

    BOOL SComboEdit::FireEvent(EventArgs & evt)
    {
        if(evt.GetID()==EVT_RE_NOTIFY)
        {//转发richedit的txNotify消息
            evt.idFrom=GetOwner()->GetID();
            evt.nameFrom=GetOwner()->GetName();
        }
        return SEdit::FireEvent(evt);
    }

    //////////////////////////////////////////////////////////////////////////
    // SDropDownWnd_ComboBox
    BOOL SDropDownWnd_ComboBox::PreTranslateMessage( MSG* pMsg )
    {
        if(SDropDownWnd::PreTranslateMessage(pMsg))
            return TRUE;
        if(pMsg->message==WM_MOUSEWHEEL 
            || ((pMsg->message == WM_KEYDOWN || pMsg->message==WM_KEYUP) && (pMsg->wParam == VK_UP || pMsg->wParam==VK_DOWN || pMsg->wParam==VK_RETURN || pMsg->wParam==VK_ESCAPE)))
        {//截获滚轮及上下键消息
            CSimpleWnd::SendMessage(pMsg->message,pMsg->wParam,pMsg->lParam);
            return TRUE;    
        }
        return FALSE;
    }


    //////////////////////////////////////////////////////////////////////////
    // SComboBoxBase
    SComboBase::SComboBase(void)
        :m_pSkinBtn(GETBUILTINSKIN(SKIN_SYS_DROPBTN))
        ,m_pEdit(NULL)
        ,m_bDropdown(TRUE)
        ,m_nDropHeight(200, SLayoutSize::dp)
        ,m_dwBtnState(WndState_Normal)
        ,m_nAnimTime(200)
        ,m_pDropDownWnd(NULL)
        ,m_iInitSel(-1)
		,m_bAutoFitDropBtn(TRUE)
    {
        m_bFocusable=TRUE;
        m_style.SetAttribute(L"align",L"left",TRUE);
        m_style.SetAttribute(L"valign",L"middle",TRUE);

        m_evtSet.addEvent(EVENTID(EventCBSelChange));
        m_evtSet.addEvent(EVENTID(EventRENotify));
		m_evtSet.addEvent(EVENTID(EventCBDropdown));
    }

    SComboBase::~SComboBase(void)
    {
    }


    BOOL SComboBase::CreateChildren( pugi::xml_node xmlNode )
    {
        SASSERT(m_pSkinBtn);
		m_xmlDropdownStyle.append_copy(xmlNode.child(KStyle_Dropdown));
        //创建edit对象
        if(!m_bDropdown)
        {            
            m_pEdit=new SComboEdit(this);
            SApplication::getSingleton().SetSwndDefAttr(m_pEdit);

            InsertChild(m_pEdit);
            pugi::xml_node xmlEditStyle=xmlNode.child(KStyle_Edit);
            m_pEdit->GetEventSet()->setMutedState(true);
            if(xmlEditStyle)
                m_pEdit->InitFromXml(xmlEditStyle);
            else
                m_pEdit->SSendMessage(WM_CREATE);
            m_pEdit->GetEventSet()->setMutedState(false);
			
            m_pEdit->SetID(IDC_CB_EDIT);
            m_pEdit->SSendMessage(EM_SETEVENTMASK,0 ,ENM_CHANGE );

        }
        return CreateListBox(xmlNode);
    }


    void SComboBase::GetDropBtnRect(LPRECT prc)
    {
        SIZE szBtn=m_pSkinBtn->GetSkinSize();
        GetClientRect(prc);
		int nHei = prc->bottom - prc->top;
        prc->left= prc->right-nHei*szBtn.cx/szBtn.cy;
		if (!m_bAutoFitDropBtn) {
			prc->top += (prc->bottom - prc->top - szBtn.cy) / 2;
			prc->left += (prc->right - prc->left - szBtn.cx) / 2;
			prc->right = prc->left + szBtn.cx;
			prc->bottom = prc->top + szBtn.cy;
		}
    }

    void SComboBase::GetTextRect( LPRECT pRect )
    {
        CRect rc = GetClientRect();
        rc.DeflateRect(GetStyle().GetPadding());
        //SIZE szBtn=m_pSkinBtn->GetSkinSize();
		CRect rcBtn;
		GetDropBtnRect(&rcBtn);
        rc.right-= rcBtn.Width();
        *pRect = rc;
    }

    void SComboBase::OnPaint(IRenderTarget * pRT )
    {
        SPainter painter;

        BeforePaint(pRT, painter);
        if(GetCurSel() != -1 && m_pEdit==NULL)
        {
            CRect rcText;
            GetTextRect(rcText);
            SStringT strText=GetWindowText();
            DrawText(pRT,strText, strText.GetLength(), rcText, GetTextAlign());
        }
        //draw focus rect
        if(IsFocused())
        {
            DrawFocus(pRT);
        }
        AfterPaint(pRT, painter);
        CRect rcBtn;
        GetDropBtnRect(&rcBtn);
        m_pSkinBtn->Draw(pRT,rcBtn,IIF_STATE3(m_dwBtnState,WndState_Normal,WndState_Hover,WndState_PushDown));
    }

    void SComboBase::OnLButtonDown( UINT nFlags,CPoint pt )
    {
		if (m_bFocusable)
			SetFocus();
        DropDown();
    }

    void SComboBase::OnMouseMove( UINT nFlags,CPoint pt )
    {
        if(m_dwBtnState==WndState_PushDown) return;

        __super::OnMouseHover(nFlags,pt);
        CRect rcBtn;
        GetDropBtnRect(&rcBtn);
        if(rcBtn.PtInRect(pt))
        {
            m_dwBtnState=WndState_Hover;
            InvalidateRect(rcBtn);
        }else if(m_dwBtnState==WndState_Hover)
        {
            m_dwBtnState=WndState_Normal;
            InvalidateRect(rcBtn);
        }
    }

    void SComboBase::OnMouseLeave()
    {
        if(m_dwBtnState==WndState_PushDown) return;

        if(GetState()&WndState_Hover) 
            __super::OnMouseLeave();
        if(m_dwBtnState==WndState_Hover)
        {
            m_dwBtnState=WndState_Normal;
            CRect rcBtn;
            GetDropBtnRect(&rcBtn);
            InvalidateRect(rcBtn);
        }
    }

    void SComboBase::OnKeyDown( TCHAR nChar, UINT nRepCnt, UINT nFlags )
    {    
        //if ( nChar == VK_DOWN)
        //    DropDown();
		
		//方向键改变当前选项
        switch (nChar) 
        {
        case VK_DOWN:
        case VK_RIGHT:
            {
                int iSel = GetCurSel();
                iSel += 1;
                if ( iSel < GetCount() )
                    SetCurSel(iSel);
            }
            break;
        case VK_UP:
        case VK_LEFT:
            {
                int iSel = GetCurSel();
                iSel -= 1;
                if ( iSel < GetCount() && iSel >= 0 )
                    SetCurSel(iSel);
            }
            break;
        }
    }

    BOOL SComboBase::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
    {
		//鼠标滚轮改变当前选项
        if (zDelta > 0)			// 上滚 
        {
            int iSel = GetCurSel();
            iSel -= 1;
            if ( iSel < GetCount() && iSel >= 0 )
                SetCurSel(iSel);
        }
        else					// 下滚 
        {
            int iSel = GetCurSel();
            iSel += 1;
            if ( iSel < GetCount() )
                SetCurSel(iSel);
        }
        return TRUE;
    }

    void SComboBase::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        if (!m_bDropdown)
        {
            SComboEdit *pEdit = static_cast<SComboEdit *>(FindChildByID(IDC_CB_EDIT));
            if (pEdit)
                pEdit->SSendMessage(WM_CHAR, nChar, MAKELONG(nFlags, nRepCnt));
            return;
        }
    }

    UINT SComboBase::OnGetDlgCode()
    {        
        return SC_WANTARROWS;
    }

    BOOL SComboBase::IsFocusable()
    {
        if (m_bDropdown && m_bFocusable)
            return TRUE;
        return FALSE;
    }


    SWindow* SComboBase::GetDropDownOwner()
    {
        return this;
    }


	static const wchar_t * KAttrTrCtx = L"trCtx";
    void SComboBase::OnCreateDropDown( SDropDownWnd *pDropDown )
    {
		pugi::xml_node xmlDropdownStyleNode = m_xmlDropdownStyle.child(KStyle_Dropdown);
		if(xmlDropdownStyleNode)
		{
			if(!xmlDropdownStyleNode.attribute(KAttrTrCtx))
			{
				xmlDropdownStyleNode.append_attribute(KAttrTrCtx).set_value(GetTrCtx());
			}
			pDropDown->InitFromXml(xmlDropdownStyleNode);
		}
		else
		{
			pDropDown->GetHostAttr().SetTrCtx(GetTrCtx());
		}
		m_dwBtnState=WndState_PushDown;
        CRect rcBtn;
        GetDropBtnRect(&rcBtn);
        InvalidateRect(rcBtn);
    }

    void SComboBase::OnDestroyDropDown(SDropDownWnd *pDropDown)
    {
        if (!m_bDropdown && m_pEdit)
        {
            m_pEdit->SetFocus();
        }

        m_dwBtnState = WndState_Normal;
        m_pDropDownWnd=NULL;
        CRect rcBtn;
        GetDropBtnRect(&rcBtn);
        InvalidateRect(rcBtn);
        ModifyState(0,WndState_Hover,TRUE);
        CPoint pt;
        GetCursorPos(&pt);
        ScreenToClient(GetContainer()->GetHostHwnd(),&pt);
        ::PostMessage(GetContainer()->GetHostHwnd(),WM_MOUSEMOVE,0,MAKELPARAM(pt.x,pt.y));

        if(pDropDown->GetExitCode()==IDOK)
        {
            OnSelChanged();
        }

    }

    BOOL SComboBase::CalcPopupRect( int nHeight,CRect & rcPopup )
    {
        CRect rcWnd=GetWindowRect();
        GetContainer()->FrameToHost(rcWnd);
        
        ClientToScreen(GetContainer()->GetHostHwnd(),(LPPOINT)&rcWnd);
        ClientToScreen(GetContainer()->GetHostHwnd(),((LPPOINT)&rcWnd)+1);

        HMONITOR hMonitor = ::MonitorFromWindow(GetContainer()->GetHostHwnd(), MONITOR_DEFAULTTONULL);
        CRect rcMonitor;
        if (hMonitor)
        {
            MONITORINFO mi = {sizeof(MONITORINFO)};
            ::GetMonitorInfo(hMonitor, &mi);
            rcMonitor = mi.rcMonitor;
        }
        else
        {
            rcMonitor.right   =   GetSystemMetrics(   SM_CXSCREEN   );   
            rcMonitor.bottom  =   GetSystemMetrics(   SM_CYSCREEN   );
        }
        if(rcWnd.bottom+nHeight<=rcMonitor.bottom)
        {
            rcPopup = CRect(rcWnd.left,rcWnd.bottom,rcWnd.right,rcWnd.bottom+nHeight);
            return TRUE;
        }else
        {
            rcPopup = CRect(rcWnd.left,rcWnd.top-nHeight,rcWnd.right,rcWnd.top);
            return FALSE;
        }
    }


    void SComboBase::DropDown()
    {
        if(m_dwBtnState==WndState_PushDown) return;


        if(!m_pDropDownWnd)
        {
            m_pDropDownWnd = new SDropDownWnd_ComboBox(this);
            m_pDropDownWnd->SDispatchMessage(UM_SETSCALE, GetScale(), 0);
		}

		EventCBDropdown evt(this);
		evt.pDropDown = m_pDropDownWnd;
		FireEvent(evt);

		CRect rcPopup;
		BOOL bDown=CalcPopupRect(GetListBoxHeight(),rcPopup);
		m_pDropDownWnd->Create(rcPopup,0);

		if(m_nAnimTime>0)
			m_pDropDownWnd->AnimateHostWindow(m_nAnimTime,AW_SLIDE|(bDown?AW_VER_POSITIVE:AW_VER_NEGATIVE));
		else
			m_pDropDownWnd->SetWindowPos(HWND_TOP,0,0,0,0,SWP_SHOWWINDOW|SWP_NOMOVE|SWP_NOZORDER|SWP_NOSIZE|SWP_NOACTIVATE);

		m_pDropDownWnd->CSimpleWnd::SetCapture();
	}

    void SComboBase::CloseUp()
    {
        if(m_pDropDownWnd)
        {
            m_pDropDownWnd->EndDropDown(IDCANCEL);
        }
    }


    void SComboBase::OnSetFocus(SWND wndOld)
    {
        if(m_pEdit) 
            m_pEdit->SetFocus();
        else
            __super::OnSetFocus(wndOld);
    }


    void SComboBase::OnDestroy()
    {
        CloseUp();
        __super::OnDestroy();
    }

    void SComboBase::OnSelChanged()
    {
        EventCBSelChange evt(this);
		evt.nCurSel = GetCurSel();
        FireEvent(evt);
    }

    BOOL SComboBase::FireEvent( EventArgs &evt )
    {
        if(evt.GetID() == EventRENotify::EventID)
        {
            EventRENotify *evtRe = (EventRENotify*)&evt;
            if(evtRe->iNotify == EN_CHANGE && !m_pEdit->GetEventSet()->isMuted())
            {
                m_pEdit->GetEventSet()->setMutedState(true);
                SetCurSel(-1);
                m_pEdit->GetEventSet()->setMutedState(false);
            }
        }
        return SWindow::FireEvent(evt);
    }

    int SComboBase::FindString( LPCTSTR pszFind,int nAfter/*=0*/ )
    {
        for(int i=nAfter;i<GetCount();i++)
        {
            SStringT strItem = GetLBText(i,TRUE);
            if(strItem == pszFind) return i;
        }
        return -1;
    }

    SStringT SComboBase::GetWindowText(BOOL bRawText/*=TRUE*/)
    {
        if(!m_bDropdown)
        {
            return GetEditText();
        }
        if(GetCurSel()==-1) return _T("");
        return GetLBText(GetCurSel(),bRawText);
    }

    void SComboBase::SetWindowText(LPCTSTR pszText)
    {
        SWindow::SetWindowText(pszText);
        SetCurSel(-1);
        if(!m_bDropdown)
        {
            m_pEdit->SetWindowText(pszText);
        }
    }

    void SComboBase::OnKillFocus(SWND wndFocus)
    {
        __super::OnKillFocus(wndFocus);
        CloseUp();
    }

	void SComboBase::UpdateChildrenPosition()
	{
		__super::UpdateChildrenPosition();
		if (!m_pEdit)
			return;
		SIZE szBtn = m_pSkinBtn->GetSkinSize();		
		CRect rcPadding = GetStyle().GetPadding();
		CRect rcEdit = GetClientRect();
		int nHei = rcEdit.Height();
		int nBtnWid = nHei *szBtn.cx/ szBtn.cy;
		rcPadding.right += nBtnWid;
		rcEdit.DeflateRect(rcPadding);
		m_pEdit->Move(rcEdit);
	}
	
	
    void SComboBase::OnColorize(COLORREF cr)
    {
        __super::OnColorize(cr);
        if(m_pSkinBtn) m_pSkinBtn->OnColorize(cr);
		if (m_pDropDownWnd)
		{
			m_pDropDownWnd->SDispatchMessage(UM_SETCOLORIZE, cr, 0);
		}
    }

	HRESULT SComboBase::OnLanguageChanged()
	{
		HRESULT hr =__super::OnLanguageChanged();
		if (m_pDropDownWnd)
		{
			m_pDropDownWnd->SDispatchMessage(UM_SETLANGUAGE, 0, 0);
		}
		return hr;
	}

	void SComboBase::OnScaleChanged(int nScale)
	{
		__super::OnScaleChanged(nScale);
		if (m_pDropDownWnd)
		{
			m_pDropDownWnd->SDispatchMessage(UM_SETSCALE, nScale, 0);
		}
        GetScaleSkin(m_pSkinBtn, nScale);
    }

}