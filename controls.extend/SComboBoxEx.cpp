#include "stdafx.h"
#include "SComboBoxEx.h"

namespace SOUI
{

    SComboBoxEx::SComboBoxEx():m_uTxtID(0),m_uIconID(0),m_pListBox(NULL)
    {
        m_evtSet.addEvent(EVENTID(EventOfComoboxExItem));
    }

    SComboBoxEx::~SComboBoxEx()
    {
        if(m_pListBox)
        {
            m_pListBox->SSendMessage(WM_DESTROY);
            delete m_pListBox;
        }
    }

    BOOL SComboBoxEx::CreateListBox( pugi::xml_node xmlNode )
    {
        SASSERT(xmlNode);
        //创建列表控件
        m_pListBox=(SListBoxEx*)SApplication::getSingleton().CreateWindowByName(SListBoxEx::GetClassName());
        m_pListBox->SetContainer(GetContainer());

        m_pListBox->InitFromXml(xmlNode.child(L"liststyle"));
        m_pListBox->SetAttribute(L"pos", L"0,0,-0,-0", TRUE);
        m_pListBox->SetAttribute(L"hotTrack",L"1",TRUE);
        m_pListBox->SetOwner(this);         //chain notify message to combobox
        m_pListBox->SetID(IDC_DROPDOWN_LIST);

        //初始化列表数据
        pugi::xml_node xmlNode_Items=xmlNode.child(L"items");
        if(xmlNode_Items)
        {
            int nItems=0;
            pugi::xml_node xmlNode_Item=xmlNode_Items.child(L"item");
            while(xmlNode_Item)
            {
                nItems++;
                xmlNode_Item=xmlNode_Item.next_sibling(L"item");
            }

            m_pListBox->SetItemCount(nItems);
            int iItem=0;
            xmlNode_Item=xmlNode_Items.child(L"item");
            while(xmlNode_Item)
            {
                LPARAM lParam=xmlNode_Item.attribute(L"data").as_int(0);
                m_pListBox->SetItemData(iItem,lParam);
                SWindow *pWnd=m_pListBox->GetItemPanel(iItem);
                if(m_uTxtID!=0)
                {
                    SWindow *pText=pWnd->FindChildByID(m_uTxtID);
                    if(pText)
                    {
                        SStringT strText=S_CW2T(xmlNode_Item.attribute(L"text").value());
                        pText->SetWindowText(strText);
                    }
                }
                if(m_uIconID!=0)
                {
                    SImageWnd * pImg = pWnd->FindChildByID2<SImageWnd>(m_uIconID);
                    if(pImg) 
                    {
                        int iIcon=xmlNode_Item.attribute(L"icon").as_int(0);
                        pImg->SetIcon(iIcon);
                    }
                }
                iItem++;
                xmlNode_Item=xmlNode_Item.next_sibling(L"item");
            }
        }

        if(m_iInitSel!=-1)
        {
            SetCurSel(m_iInitSel);
        }
        return TRUE;
    }

    int SComboBoxEx::GetListBoxHeight()
    {
        int nDropHeight=m_nDropHeight;
        if(GetCount()) 
        {
            int nItemHeight=m_pListBox->GetItemHeight();
            nDropHeight = min(nDropHeight,nItemHeight*GetCount()+m_pListBox->GetStyle().m_rcMargin.top + m_pListBox->GetStyle().m_rcMargin.bottom);
        }
        return nDropHeight;    
    }

    void SComboBoxEx::OnCreateDropDown( SDropDownWnd *pDropDown )
    {
        __super::OnCreateDropDown(pDropDown);
        pDropDown->InsertChild(m_pListBox);
        pDropDown->UpdateChildrenPosition();

        m_pListBox->SetVisible(TRUE);
        m_pListBox->SetFocus();
        m_pListBox->EnsureVisible(GetCurSel());
    }

    void SComboBoxEx::OnDestroyDropDown( SDropDownWnd *pDropDown)
    {
        pDropDown->RemoveChild(m_pListBox);
        m_pListBox->SetVisible(FALSE);
        m_pListBox->SetContainer(GetContainer());
        __super::OnDestroyDropDown(pDropDown);
    }

    void SComboBoxEx::OnSelChanged()
    {
        int iSel=m_pListBox->GetCurSel();
        if(m_pEdit && !m_pEdit->GetEventSet()->isMuted())
        {
            SStringT strText=GetLBText(iSel);
            m_pEdit->GetEventSet()->setMutedState(true);
            m_pEdit->SetWindowText(strText);
            m_pEdit->GetEventSet()->setMutedState(false);
        }
        Invalidate();
        __super::OnSelChanged();
    }

    BOOL SComboBoxEx::FireEvent( EventArgs &evt )
    {
        if(evt.idFrom == IDC_DROPDOWN_LIST && m_pDropDownWnd)
        {
            if(evt.GetID()==EventLBSelChanged::EventID)
            {//列表选中项改变事件
                OnSelChanged();
                return TRUE;
            }
        }
        if(evt.GetID() == EventOfPanel::EventID)
        {
            EventOfPanel *pEvtOfPanel = (EventOfPanel*) &evt;
            if(pEvtOfPanel->pOrgEvt->GetID() == EventCmd::EventID)
            {
                EventOfComoboxExItem evt2(this,(EventCmd*)pEvtOfPanel->pOrgEvt);
                __super::FireEvent(evt2);
                if(!evt2.bCancel)
                {//可以关闭下拉列表
                    CloseUp();
                }
                return TRUE;        
            }
        }
        return SComboBase::FireEvent(evt);
    }

    SStringT SComboBoxEx::GetLBText( int iItem )
    {
        if(m_uTxtID == 0 || iItem<0 || iItem>= GetCount()) return _T("");
        SWindow *pItem=m_pListBox->GetItemPanel(iItem);
        SWindow *pText=pItem->FindChildByID(m_uTxtID);
        if(!pText) return _T("");
        return pText->GetWindowText();
    }

}