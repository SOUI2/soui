#include "souistd.h"
#include "control/SComboBox.h"
#include <algorithm>

namespace SOUI
{


	//////////////////////////////////////////////////////////////////////////
	SComboBox::SComboBox()
		:m_pListBox(NULL)
	{

	}

	SComboBox::~SComboBox()
	{
		if(m_pListBox)
		{
			m_pListBox->SSendMessage(WM_DESTROY);
			delete m_pListBox;
		}
	}

	BOOL SComboBox::CreateListBox( pugi::xml_node xmlNode )
	{
		SASSERT(xmlNode);
		//创建列表控件
		pugi::xml_node listStyle = xmlNode.child(L"listStyle");
		SStringW strListClass = listStyle.attribute(L"wndclass").as_string(SListBox::GetClassName());
		m_pListBox=sobj_cast<SListBox>(SApplication::getSingleton().CreateWindowByName(strListClass));
		SASSERT(m_pListBox);

		m_pListBox->SetContainer(GetContainer());
		m_pListBox->InitFromXml(listStyle);
		m_pListBox->SetAttribute(L"pos", L"0,0,-0,-0", TRUE);
		m_pListBox->SetAttribute(L"hotTrack",L"1",TRUE);
		m_pListBox->SetOwner(this);    //chain notify message to combobox
		m_pListBox->SetID(IDC_DROPDOWN_LIST);
        m_pListBox->SSendMessage(UM_SETSCALE, GetScale());

		//初始化列表数据
		pugi::xml_node xmlNode_Items=xmlNode.child(L"items");
		if(xmlNode_Items)
		{
			pugi::xml_node xmlNode_Item=xmlNode_Items.child(L"item");
			while(xmlNode_Item)
			{

				SStringW strText=xmlNode_Item.attribute(L"text").value();
				int iIcon=xmlNode_Item.attribute(L"icon").as_int(0);
				LPARAM lParam=xmlNode_Item.attribute(L"data").as_int(0);
				m_pListBox->AddString(S_CW2T(strText),iIcon,lParam);
				xmlNode_Item=xmlNode_Item.next_sibling(L"item");
			}
		}

		if(m_iInitSel!=-1)
		{
			SetCurSel(m_iInitSel);
		}
		return TRUE;
	}

	int SComboBox::GetListBoxHeight()
	{
		int nDropHeight=m_nDropHeight.toPixelSize(GetScale());
		if(GetCount()) 
		{
			int nItemHeight=m_pListBox->GetItemHeight();
			CRect rcMargin = m_pListBox->GetStyle().GetMargin();
			nDropHeight = (std::min)(nDropHeight,(int)(nItemHeight*GetCount()+rcMargin.top + rcMargin.bottom));
		}
		return nDropHeight;    
	}

	void SComboBox::OnCreateDropDown( SDropDownWnd *pDropDown)
	{
		__super::OnCreateDropDown(pDropDown);
		pDropDown->InsertChild(m_pListBox);
		pDropDown->UpdateChildrenPosition();

		m_pListBox->SetVisible(TRUE);
		m_pListBox->SetFocus();
		m_pListBox->EnsureVisible(GetCurSel());
	}

	void SComboBox::OnDestroyDropDown( SDropDownWnd *pDropDown)
	{
		pDropDown->RemoveChild(m_pListBox);
		m_pListBox->SetVisible(FALSE);
		m_pListBox->SetContainer(GetContainer());
		__super::OnDestroyDropDown(pDropDown);
	}

	void SComboBox::OnSelChanged()
	{
		m_pListBox->GetCurSel();
		if(m_pEdit && !m_pEdit->GetEventSet()->isMuted())
		{
			SStringT strText=GetLBText(m_pListBox->GetCurSel());
			m_pEdit->GetEventSet()->setMutedState(true);
			m_pEdit->SetWindowText(strText);
			m_pEdit->GetEventSet()->setMutedState(false);
		}
		Invalidate();
		__super::OnSelChanged();
	}

	BOOL SComboBox::FireEvent( EventArgs &evt )
	{
		if(evt.idFrom == IDC_DROPDOWN_LIST && m_pDropDownWnd)
		{
			if(evt.GetID()==EventLBSelChanged::EventID)
			{
				OnSelChanged();
				return TRUE;
			}
			if(evt.GetID() == EventCmd::EventID)
			{
				CloseUp();
				return TRUE;
			}
		}
		return SComboBase::FireEvent(evt);
	}

    void SComboBox::OnScaleChanged(int nScale)
    {
        __super::OnScaleChanged(nScale);
        if(m_pListBox)
            m_pListBox->SSendMessage(UM_SETSCALE, GetScale());
    }

	HRESULT SComboBox::OnLanguageChanged()
	{
		HRESULT hr = __super::OnLanguageChanged();
		if(m_pListBox)
			m_pListBox->SSendMessage(UM_SETLANGUAGE);
		return hr;
	}


}//namespace SOUI

